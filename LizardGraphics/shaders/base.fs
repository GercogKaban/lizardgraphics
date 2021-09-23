#version 330 core

struct DirLight 
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Light 
{
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};


//struct Material 
//{
    //sampler2D diffuse;
    //sampler2D normalMap;
    //vec3      specular;
    //float     shininess;
//}; 

out vec4 color;

in vec2 TexCoord;
in vec3 FragPos;  
in vec3 Normal;  

uniform sampler2D diffuse;
//uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform DirLight dirLight;


void DirLight_()
{
// нужно будет добавить в юниформы (проблемы с Vulkan)

	const vec3 lightColor =  vec3(1.0f);

	// Ambient
    vec3 ambient = dirLight.ambient * texture(diffuse, TexCoord).rgb;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(diffuse, TexCoord).rgb;
    
    // Specular
    vec3 specular = vec3(0.0f);
    //if (FragPos.x >= viewPos.x)
    {
    	vec3 viewDir = normalize(viewPos - FragPos);

    	vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        specular = dirLight.specular * spec;
    }
        
    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}


void main()
{
	 DirLight_();
}