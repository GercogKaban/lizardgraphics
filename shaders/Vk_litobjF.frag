#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 lightPos;
layout(location = 3) in vec3 lightColor;
layout(location = 4) in float ambient;
layout(location = 5) in float specular;
layout(location = 6) in vec4 Color;
layout(location = 7) in vec3 FragPos;
layout(location = 8) in vec3 viewPos;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 ambientVec = ambient * lightColor;
  	
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularVec = specular * spec * lightColor;  
        
    vec4 result = vec4(ambientVec + diffuse + specularVec,1.0) * Color;
	outColor = result*texture(texSampler, TexCoord);
}