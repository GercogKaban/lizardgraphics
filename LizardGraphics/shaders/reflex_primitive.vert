#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 5) in mat4 model_;
layout (location = 9)  in vec4 diffuseCoords;

layout (location = 0) out vec4 position_;

out VS_OUT
{
out vec3 Normal;
out vec2 BaseTexCoords;
out vec3 FragPos;
out vec3 FragPosTBN;
out vec3 viewPosTBN;
out vec2 TexCoordsDiffuse;
out vec2 TexCoordsNormal;
out vec2 TexCoordsParallax;
out vec2 TexCoordsReflex;
out vec3 projCoords;
out vec4 eyeSpacePosition;
out mat3 TBN;
out mat4 model;
out vec2 off_;
out vec2 sz_;
out vec2 maxParallax;
out flat ivec3 mapping;
} vs;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec4 temp = model_ * vec4(position, 1.0);
    position_ = temp;
    vs.eyeSpacePosition = view*temp;
    vs.FragPos = temp.xyz;
    //vs.viewPos = viewPos;

        vs.TexCoordsDiffuse = vec2(
		    textureCoords.x *diffuseCoords[2] + diffuseCoords[0], 
		    textureCoords.y *diffuseCoords[3] + diffuseCoords[1]);

    vs.BaseTexCoords = textureCoords;
}