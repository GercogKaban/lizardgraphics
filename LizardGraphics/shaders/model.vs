#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec2 TexCoordsNormal;
out vec2 TexCoords;
out vec3 projCoords;
out vec4 eyeSpacePosition;
out mat3 TBN;
out mat4 model;
out vec3 Normal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

uniform mat4 model_;
uniform vec2 offset;
uniform vec2 textureSize;
uniform vec2 offsetNormal;
uniform vec2 textureSizeNormal;
uniform int normalMapping;

void main()
{
    vec4 temp = model_ * vec4(position, 1.0);
    eyeSpacePosition = view*temp;
    FragPos = vec3(temp);

    if (normalMapping)
    {
        vec3 Tangent = normalize(model_* vec4(tangent,0.0)).xyz;
        Normal = normalize(model_*  vec4(normals,0.0)).xyz;
        vec3 Bitangent = normalize(model_ * vec4(bitangent, 0.0)).xyz;
        Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
        TBN = mat3(Tangent, Bitangent, Normal);       
    }
    else
        Normal = normalize(mat3(transpose(inverse(model_))) * normals); 

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);
    TexCoordsNormal = vec2(
		textureCoords.x *textureSizeNormal.x + offsetNormal.x , 
		textureCoords.y*textureSizeNormal.y + offsetNormal.y);

    gl_Position = proj * eyeSpacePosition;
    model = model_;

    // perform perspective divide
    vec3 projCoords_ = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords_ * 0.5 + 0.5;
}