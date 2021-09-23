#version 430 core

#define MAX_LIGHTS 128

out vec4 color;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight 
{
	vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 projCoords;

in mat4 model;
uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;

uniform vec3 viewPos;

uniform ivec2 screenSize;

uniform int pointSourcesCount = 0;
uniform int spotSourcesCount = 0;
uniform PointLight pointSources[MAX_LIGHTS];
uniform SpotLight spotSources[MAX_LIGHTS];
uniform DirLight dirLight;
//uniform int objId;

//layout(std430, binding = 3) buffer Test
//{
//    int data_SSBO[];
//};

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 viewDir);

float ShadowCalculation(vec3 lightDir)
{
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    //vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //if(projCoords.z > 1.0)
        //shadow = 0.0;
        
    return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 viewDir)
{
    // diffuse shading
    float diff = max(dot(light.direction,Normal), 0.0);
    vec3 diffuse = light.diffuse * diff; //* vec3(texture(material.diffuse, TexCoords));
    // specular shading
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient; //* vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, TexCoords));
    //float shadow = 0.0f;
    float shadow = ShadowCalculation(lightDir); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 viewDir)
{
    //vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(Normal, viewDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-viewDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f); //material.shininess);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse;// * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular;// * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = ShadowCalculation(light.position); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction)); 
    vec3 result = vec3(0.0f);
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient;
        
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff; //* texture(1.0f, TexCoords).rgb;  
        
        // specular
        vec3 reflectDir = reflect(-lightDir, Normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f);
        vec3 specular = light.specular * spec;//* texture(1.0f, TexCoords).rgb;  
        
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   

        float shadow = ShadowCalculation(light.position); 
            
        result = ambient + (1.0 - shadow) + diffuse + specular;
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        result = light.ambient;
    }
	return result;
}

void main()
{    
    //if (objId == -1)
    //    discard;
    //data_SSBO[screenSize.x * int(gl_FragCoord.y) + int(gl_FragCoord.x)] = objId;
    const bool lighting = true;
    vec3 result = vec3(0.0f);
    if (lighting)
    {
        vec3 viewDir = normalize(viewPos - FragPos);   
        result += CalcDirLight(dirLight, viewDir);
        for (int i = 0; i < pointSourcesCount; ++i)
        	result += CalcPointLight(pointSources[i],viewDir);
        for (int i = 0; i < spotSourcesCount; ++i)
        	result += CalcSpotLight(spotSources[i],viewDir);
    }

    color = vec4(result,1.0f) * texture(diffuseMap, TexCoords);
    if (color.a - 0.1f < 0) discard;
}
