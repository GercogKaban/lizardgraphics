#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 projCoords;
out vec4 eyeSpacePosition;

out mat4 model;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

uniform mat4 model_;
uniform vec2 offset;
uniform vec2 textureSize;

void main()
{
    vec4 temp = model_ * vec4(position, 1.0);
    eyeSpacePosition = view*temp;
    FragPos = vec3(temp);
    Normal = normalize(mat3(transpose(inverse(model_))) * normals);  
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);
    gl_Position = proj * eyeSpacePosition;
    model = model_;

    // perform perspective divide
    vec3 projCoords_ = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords_ * 0.5 + 0.5;
}