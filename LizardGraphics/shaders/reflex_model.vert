#version 430 core
#define MAX_LIGHTS 128
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

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

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 textureCoords;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

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

uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;

uniform mat4 model_;
uniform vec2 offset;
uniform vec2 textureSize;
uniform vec2 offsetNormal;
uniform vec2 textureSizeNormal;
uniform vec2 offsetParallax;
uniform vec2 textureSizeParallax;
uniform vec2 offsetReflex;
uniform vec2 textureSizeReflex;

uniform int parallaxMapping;
uniform int normalMapping;
uniform int reflexMapping;

uniform bool playAnimation;

uniform mat4 finalBonesTrans[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    if (playAnimation == true)
    {
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(position,1.0f);
            break;
        }
        totalPosition += finalBonesTrans[boneIds[i]] * vec4(position,1.0f) * weights[i];
        }
    }
    else
       totalPosition = vec4(position,1.0f);

    vec4 temp = model_ * vec4(vec3(totalPosition), 1.0);
    vs.FragPos = vec3(temp);
   
    vs.TexCoordsDiffuse = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);   
    vs.BaseTexCoords = textureCoords;
    vs.eyeSpacePosition = view*temp;
    position_ = totalPosition;
}