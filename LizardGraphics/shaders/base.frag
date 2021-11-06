#version 430 core
#define MAX_LIGHTS 100

struct DirLight 
{
	vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool calculateShadow;
};

struct PointLight 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;

    bool calculateShadow;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;  

    float cutOff;
    float outerCutOff; 
    float constant;
    float linear;
    float quadratic;
 
    bool calculateShadow;   
};

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

in VS_OUT
{
in vec3 Normal;
in vec2 BaseTexCoords;
in vec3 FragPos;
in vec3 FragPosTBN;
in vec3 viewPosTBN;
in vec2 TexCoordsDiffuse;
in vec2 TexCoordsNormal;
in vec2 TexCoordsParallax;
in vec2 TexCoordsReflex;
in vec3 projCoords;
in vec4 eyeSpacePosition;
in mat3 TBN;
in mat4 model;
in vec2 off_;
in vec2 sz_;
in vec2 maxParallax;
in flat ivec3 mapping;
} vs;

out vec4 color;

vec2 TexCoord;
vec2 TexCoordNormal;
vec2 TexCoordReflex;
vec3 Normal_;

uniform vec3 viewPos;
uniform mat4 view;

uniform samplerCube shadowCubeMap;
uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;
uniform sampler2D parallaxMap;
uniform sampler2D reflexMap;

uniform samplerCube environment;

uniform int selfShading;
uniform float heightScale;
uniform ivec2 screenSize;

uniform PointLight pointSources[MAX_LIGHTS];
uniform SpotLight spotSources[MAX_LIGHTS];
uniform DirLight dirSources[MAX_LIGHTS];

uniform int pointSourcesCount = 0;
uniform int spotSourcesCount = 0;
uniform int dirSourcesCount = 0;
uniform Fog fog;
uniform bool drawingReflex;
uniform float farPlane;

vec3 CalcDirLight(DirLight light, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 viewDir);      

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
 // number of depth layers
 viewDir.y = - viewDir.y;
 const float minLayers = 8.0;
 const float maxLayers = 32.0;
 float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
 // calculate the size of each layer
 float layerDepth = 1.0 / numLayers;
 // depth of current layer
 float currentLayerDepth = 0.0;
 // the amount to shift the texture coordinates per layer (From vector P)
 vec2 P = viewDir.xy * heightScale;
 vec2 deltaTexCoords = P / numLayers;

 // get initial values
 vec2 currentTexCoords = texCoords;
 float currentDepthMapValue = 1.0 - texture(parallaxMap, currentTexCoords).r;
 
 while(currentLayerDepth < currentDepthMapValue)
 {
   // shift texture coordinates along direction of P
  currentTexCoords -= deltaTexCoords;
  // get depthmap value at current texture coordinates
  currentDepthMapValue = 1.0 - texture(parallaxMap, currentTexCoords).r;
  // get depth of next layer
  currentLayerDepth += layerDepth;
 }

 // Parallax Occlusion Mapping
 
 // get texture coordinates before collision (reverse operations)
 vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
 
 // get depth after and before collision for linear interpolation
 float afterDepth = currentDepthMapValue - currentLayerDepth;
 float beforeDepth = 1.0 - texture(parallaxMap, prevTexCoords).r - currentLayerDepth + layerDepth;

 // interpolation of texture coordinates
 float weight = afterDepth / (afterDepth - beforeDepth);
 vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

 return finalTexCoords;
}

float parallaxSoftShadowMultiplier(in vec3 lightDir, in vec2 initialTexCoords, in float initialHeight)
{
   return 0.0;
 // shadowMultiplier = 0.0, no shadow -> in light
 // shadowMultiplier = 1.0, shadow -> outside light
 float shadowMultiplier = 0.0;
 const float minLayers = 16.0;
 const float maxLayers = 30.0;

 //const float eps = 0.0;
 //if (abs(initialTexCoords.x) + eps >= abs(maxParallax.x) && abs(initialTexCoords.y) + eps >= abs(maxParallax.y)
 //|| abs(initialTexCoords.x) <= abs(off_.x + eps) && abs(initialTexCoords.y) <= abs(off_.y + eps))
 //{
  //   const float borderVal = abs(dot(lightDir.xy, vec2(0.05,0.05)));
  //   if (initialTexCoords.x + borderVal >= maxParallax.x && initialTexCoords.y + borderVal >= maxParallax.y 
  //   || initialTexCoords.x <= off_.x + borderVal && initialTexCoords.y <= off_.y + borderVal)
 //         return 0.0;
 //}
 // calculate lighting only for surface oriented to the light source
 if(dot(vec3(0, 0, 1), lightDir) > 0)
 {
  // calculate initial parameters
  float numSamplesUnderSurface = 0;
  shadowMultiplier = 0;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), lightDir)));
  float layerHeight = initialHeight / numLayers;
  vec2 texStep = heightScale * lightDir.xy / numLayers;

  // current parameters
  float currentLayerHeight = initialHeight - layerHeight;
  vec2 currentTexCoords = initialTexCoords + texStep;
  float depthFromTexture = texture(parallaxMap, currentTexCoords).r;
  int stepIndex = 1;
  
  // while point is below depth 0.0
  while(currentLayerHeight > 0.0)
  { 
   // if point is under the surface
   if(depthFromTexture < currentLayerHeight)
   {
    // calculate partial shadowing factor
    numSamplesUnderSurface += 1;
    float newShadowMultiplier = (currentLayerHeight - depthFromTexture) * (1.0 - stepIndex / numLayers);
    shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
   }

   // offset to the next layer
   stepIndex += 1;
   currentLayerHeight -= layerHeight;
   currentTexCoords += texStep;
   depthFromTexture = texture(parallaxMap, currentTexCoords).r;
}

  // Shadowing factor should be 0 if there were no points under the surface
  if(numSamplesUnderSurface < 1)
   shadowMultiplier = 0; 
 }

 return shadowMultiplier;
}

float getFogFactor(float fogCoordinate)
{
	float result = exp(-pow(fog.density * fogCoordinate, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

float ShadowCalculation(vec3 lightDir)
{
    float closestDepth = texture(shadowMap, vs.projCoords.xy).r; 
    float currentDepth = vs.projCoords.z;
    float bias = max(0.05 * (1.0 - dot(Normal_, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vs.projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float ShadowCalculationOmni(vec3 lightPos)
{
    // get vector between fragment position and light position
    vec3 lightToFrag = vs.FragPos - lightPos;
    //fragToLight.y = - fragToLight.y;
    // use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(shadowCubeMap, lightToFrag).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(lightToFrag);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    return currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 viewDir)
{
    vec3 LightPos = vs.TBN * light.position;
    vec3 lightDir = normalize(LightPos - vs.FragPosTBN);
    // diffuse shading
    float diff = max(dot(lightDir,Normal_), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, Normal_);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(Normal_, halfwayDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient; //* vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff; //* vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, TexCoords));
    float shadow = 0.0;
    if (light.calculateShadow == true)
       shadow = ShadowCalculation(lightDir); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 viewDir)
{
    vec3 LightPos = light.position;
    if (vs.mapping[1] != 0 || vs.mapping[0] != 0)
        LightPos = vs.TBN* light.position;
    vec3 lightDir = normalize(LightPos - vs.FragPosTBN);
    // diffuse shading
    float diff = max(dot(Normal_, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(Normal_, halfwayDir), 0.0), 32.0);
    // attenuation
    float distance = length(LightPos - vs.FragPosTBN);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * attenuation;
    vec3 diffuse = light.diffuse * attenuation * diff;
    vec3 specular = light.specular * attenuation * spec;
    float shadow = ShadowCalculationOmni(light.position);
    //if (selfShading)
        //shadow = parallaxSoftShadowMultiplier(lightDir, TexCoord, texture(parallaxMap, TexCoord).r);
    //ShadowCalculation(light.position); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir)
{
    vec3 lightPos = vs.TBN * light.position;
	vec3 lightDir = normalize(lightPos - vs.FragPosTBN);
    float theta = dot(lightDir, normalize(-light.direction)); 
    vec3 result = vec3(0.0f);
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient;
        
        // diffuse 
        float diff = max(dot(Normal_, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff; //* texture(1.0f, TexCoords).rgb;  
        
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        float spec = pow(max(dot(Normal_, halfwayDir), 0.0), 32.0);
        //vec3 reflectDir = reflect(-lightDir, Normal_);  
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f);
        vec3 specular = light.specular * spec;//* texture(1.0f, TexCoords).rgb;  
        
        // attenuation
        float distance    = length(lightPos - vs.FragPosTBN);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   

        float shadow = 0.0f;
        if (light.calculateShadow)
            shadow = ShadowCalculation(lightPos);      
        result = ambient + (1.0 - shadow) + diffuse + specular;
    }
    else 
    {
        result = light.ambient;
    }
	return result;
}

void main()
{    
    float ambient = 0.2f;
    float fogCoordinate = abs(vs.eyeSpacePosition.z / vs.eyeSpacePosition.w);
    if(fog.isEnabled)
    {    
        if (fogCoordinate == 1.0)
        {
            color = vec4(fog.color, 1.0);
            return;
        }
    }

    vec3 viewDir = normalize(vs.viewPosTBN - vs.FragPosTBN);

    if (vs.mapping[1])
    {
        vec2 texCoords = ParallaxMapping(vs.TexCoordsParallax, viewDir); 
    
        if(texCoords.x > vs.maxParallax.x)
             texCoords.x-= vs.sz_.x;
        else if (texCoords.x < vs.off_.x)
             texCoords.x += vs.sz_.x;  
        if(texCoords.y > vs.maxParallax.y)
            texCoords.y-= vs.sz_.y;
        else if (texCoords.y < vs.off_.y)
            texCoords.y+= vs.sz_.y;

        TexCoordNormal = texCoords;
        TexCoord = texCoords;
        TexCoordReflex = texCoords;
    }
    else
    {
        TexCoord = vs.TexCoordsDiffuse;
        TexCoordNormal = vs.TexCoordsNormal;
        TexCoordReflex = vs.TexCoordsReflex;
    }

    vec3 Normal0;
    if (vs.mapping[0])
    {
        vec3 BumpMapNormal = texture(normalMap, TexCoordNormal).rgb;
        Normal0 = normalize(2.0 * BumpMapNormal - vec3(1.0,1.0,1.0)); 
    }  
    else
        Normal0 = vs.Normal;

    // need turning on for each object individually
    //if (!gl_FrontFacing)
        //Normal_ = -Normal0;

    Normal_ = Normal0;

    vec3 result = vec3(0.0);
    if (!drawingReflex)
    {
    for (int i = 0; i < pointSourcesCount; ++i)
        result += CalcPointLight(pointSources[i],viewDir);
    for (int i = 0; i < spotSourcesCount; ++i)
        result += CalcSpotLight(spotSources[i],viewDir);
    for (int i = 0; i < dirSourcesCount; ++i)
        result += CalcDirLight(dirSources[i],viewDir);
    }
    else
        result =  vec3(1.0f);
    if (!drawingReflex && vs.mapping[2] != 0)
    {
        vec4 reflex = texture(reflexMap, TexCoordReflex);
        const float refCoef = (reflex.r + reflex.g + reflex.b)/3.0;
        vec3 I = normalize(vs.FragPos - viewPos);
        vec3 R = reflect(I, vs.Normal);
        color = mix( (vec4(result,1.0) * texture(diffuseMap, TexCoord)),(vec4(texture(environment, R).rgb, 1.0)),refCoef);
    }
    else
        color = vec4(result,1.0) * texture(diffuseMap, TexCoord);
    if(fog.isEnabled)
       color = mix(color, vec4(fog.color, 1.0), getFogFactor(fogCoordinate));
    if (color.a - 0.1 < 0) discard;
}
