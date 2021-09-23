#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in mat4 model_;
layout (location = 7) in vec2 offset;
layout (location = 8) in vec2 textureSize;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 projCoords;

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
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);
    //TexCoords = textureCoords;
    gl_Position = proj * view * model_ * vec4(position, 1.0);
    model = model_;

    // perform perspective divide
    vec3 projCoords_ = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords_ * 0.5 + 0.5;
    //objId = objId_;
    //diffuseMap = diffuseMap_;
    //shadowMap = shadowMap_;
}