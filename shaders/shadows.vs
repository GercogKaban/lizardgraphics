#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in mat4 model_;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

out mat4 model;
//out int objId;
//out sampler2D diffuseMap;
//out sampler2D shadowMap;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

void main()
{
    FragPos = vec3(model_ * vec4(position, 1.0));
    Normal = normalize(mat3(transpose(inverse(model_))) * normals);  
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    TexCoords = textureCoords;
    gl_Position = proj * view * model_ * vec4(position, 1.0);
    model = model_;
    //objId = objId_;
    //diffuseMap = diffuseMap_;
    //shadowMap = shadowMap_;
}