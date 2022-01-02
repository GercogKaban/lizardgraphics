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

layout (location = 0) out vec4 color;
layout (location = 1) out vec3 normal;
//layout (location = 2) out float depth;

uniform vec3 viewPos;
uniform mat4 view;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;

uniform int selfShading;
uniform float heightScale;
uniform ivec2 screenSize;

uniform PointLight pointSources[MAX_LIGHTS];
uniform SpotLight spotSources[MAX_LIGHTS];
uniform DirLight dirSources[MAX_LIGHTS];

uniform int pointSourcesCount = 0;
uniform int spotSourcesCount = 0;
uniform int dirSourcesCount = 0;
uniform bool drawingReflex;
uniform float farPlane;

uniform mat4 lightSpaceMatrix[6];

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

uniform Fog fog;

float getFogFactor(float fogCoordinate)
{
	float result = exp(-pow(fog.density * fogCoordinate, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

vec3 CalcDirLight(DirLight light, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 viewDir);      

in vec2 TexCoords;

float ShadowCalculation(vec3 lightDir, vec3 FragPos)
{
    vec4 FragPosLightSpace = lightSpaceMatrix[0] * vec4(FragPos, 1.0);
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float ShadowCalculationOmni(vec3 lightPos, vec3 FragPos)
{
    vec3 lightToFrag = FragPos - lightPos;
    float closestDepth = texture(shadowCubeMap, lightToFrag).r;
    closestDepth *= farPlane;
    float currentDepth = length(lightToFrag);
    //float bias = 0.05;
    return currentDepth > closestDepth ? 1.0 : 0.0;        
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 FragPos)
{
    vec3 ambient = light.ambient;
    vec3 LightPos = light.position;
    vec3 lightDir = normalize(LightPos - FragPos);

    float dot_ = dot(lightDir,normal);
    if (dot_ < 0.0f)
        return ambient;
        
    // diffuse shading
    float diff = max(dot_, 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    // combine results
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    float shadow = 0.0;
    if (light.calculateShadow == true)
       shadow = ShadowCalculation(lightDir, FragPos); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 FragPos)
{
    vec3 LightPos = light.position;
    LightPos = light.position;
    vec3 lightDir = normalize(LightPos - FragPos);
    float dot_ = dot(normal, lightDir);
    if (dot_ < 0.0f)
        return light.ambient;
    // diffuse shading
    float diff = max(dot_, 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    // attenuation
    float distance = length(LightPos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * attenuation;
    vec3 diffuse = light.diffuse * attenuation * diff;
    vec3 specular = light.specular * attenuation * spec;
    float shadow = ShadowCalculationOmni(light.position, FragPos);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir, vec3 FragPos)
{
    vec3 lightPos = light.position;
	vec3 lightDir = normalize(lightPos - FragPos);
    float theta = dot(lightDir, normalize(-light.direction)); 
    vec3 result = vec3(0.0f);
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient;
        
        float dot_ = dot(normal, lightDir);
        if (dot_ < 0.0f)
            return light.ambient;
        // diffuse 
        float diff = max(dot_, 0.0);
        vec3 diffuse = light.diffuse * diff; //* texture(1.0f, TexCoords).rgb;  
        
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specular = light.specular * spec;//* texture(1.0f, TexCoords).rgb;  
        
        // attenuation
        float distance    = length(lightPos - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        diffuse   *= attenuation;
        specular *= attenuation;   

        float shadow = 0.0f;
        if (light.calculateShadow)
            shadow = ShadowCalculation(lightPos, FragPos);      
        result = ambient + (1.0 - shadow) + diffuse + specular;
    }
    else 
        result = light.ambient;
	return result;
}

bool compareFloats(float a, float b, float eps)
{
    float absA = abs(a);
    float absB = abs(b);
    float diff = abs(a - b);

    if (a == b)
        return true;
    else if (a == 0 || b == 0 || diff < 1.175494351e-38)
        return diff < eps * 1.175494351e-38;
    else
        return diff / (absA + absB) < eps;
}

bool compareVec3(vec3 a, vec3 b, float eps)
{
    return compareFloats(a.x,b.x,eps) && compareFloats(a.y,b.y,eps) && compareFloats(a.z,b.z,eps);
}

void main()
{
    color = texture(gAlbedo, TexCoords); 
    normal = texture(gNormal,TexCoords).rgb;
    if(fog.isEnabled && normal.x == 0.0 && normal.y == 0.0 && normal.z == 0.0)
        discard;

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 eyeSpacePosition = view * vec4(FragPos,1.0f);
    float fogCoordinate = abs(eyeSpacePosition.z / eyeSpacePosition.w);

    vec3 result = vec3(0.0);
    for (int i = 0; i < pointSourcesCount; ++i)
        result += CalcPointLight(pointSources[i],viewDir,FragPos);
    for (int i = 0; i < spotSourcesCount; ++i)
        result += CalcSpotLight(spotSources[i],viewDir,FragPos);
    for (int i = 0; i < dirSourcesCount; ++i)
        result += CalcDirLight(dirSources[i],viewDir,FragPos);

    //if (!drawingReflex && vs.mapping[2] != 0)
    //{
        //vec4 reflex = texture(reflexMap, TexCoordReflex);
        //const float refCoef = (reflex.r + reflex.g + reflex.b)/3.0;
        //vec3 I = normalize(vs.FragPos - viewPos);
        //vec3 R = reflect(I, vs.Normal);
        //color = mix( (vec4(result,1.0) * texture(diffuseMap, TexCoord)),(vec4(texture(environment, R).rgb, 1.0)),refCoef);
    //}
    //else
    color*= vec4(result,1.0);
    if(fog.isEnabled)
       color = mix(color, vec4(fog.color, 1.0), getFogFactor(fogCoordinate));
}