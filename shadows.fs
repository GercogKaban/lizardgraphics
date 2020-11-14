#version 330 core
out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

//uniform bool LIGHT;

//uniform bool COLOR_R;
//uniform bool COLOR_GB;

//uniform bool POINT_LIGHT_1;
//uniform bool POINT_LIGHT_2;
//uniform bool DIR_LIGHT;
//uniform bool SPOT_LIGHT;
//uniform int NR_POINT_LIGHTS;

uniform vec3 viewPos;
//uniform DirLight dirLight;
//uniform PointLight pointLights[4];
//uniform SpotLight spotLight;
//uniform Material material;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;
uniform vec4 color_;

uniform bool sampleTexture;
//uniform vec3 lightPos;

vec3 lightPos = vec3(4.0f, 2.0f, 3.0f);
// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(vec3(0.0f,0.0f,1.0f));
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
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

void main()
{    

    DirLight dirLight;
    dirLight.ambient = vec3(0.5f,0.5f,0.5f);
    dirLight.diffuse = vec3(0.7f,0.7f,0.7f);
    dirLight.specular = vec3(0.3f,0.3f,0.3f);
    dirLight.direction = vec3(0.5f,0.0f,-1.0f);
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    PointLight pointLight;
    pointLight.position = lightPos;
        
    pointLight.constant = 0.7f;
    pointLight.linear = 0.22f;
    pointLight.quadratic = 0.04f;
    	
    pointLight.ambient = dirLight.ambient * vec3(1.2f, 1.1f, 1);
    pointLight.diffuse = dirLight.diffuse * vec3(1.2f, 1.1f, 1);
    pointLight.specular = dirLight.specular * vec3(1.2f, 1.1f, 1);
     
    vec3 result;

    result += CalcPointLight(pointLight, norm, FragPos, viewDir);

    //pointLight.position.y += 12.0f;
    //pointLight.position.z = 0;

    //result += CalcPointLight(pointLight, norm, FragPos, viewDir);
   
    color = vec4(result,1.0f)* texture(ourTexture, TexCoords);

    if (color.a - 0.1f < 0) discard;

}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient; //* vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff; //* vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
    //return vec3(0.0f,0.0f,0.0f);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f); //material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse;// * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular;// * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = ShadowCalculation(FragPosLightSpace); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}
