#version 430 core
#define MAX_LIGHTS 128

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
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec2 TexCoordsNormal;
out vec2 TexCoordsParallax;
out vec3 projCoords;
out vec4 eyeSpacePosition;
out mat3 TBN;
out mat4 model;
out vec3 Normal;
out vec3 viewPos_; 
out vec2 TexCoords_;
out vec2 off_;
out vec2 sz_;
out vec2 maxParallax;

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

void main()
{
    vec4 temp = model_ * vec4(position, 1.0);
    eyeSpacePosition = view*temp;
    vec3 FragPos_ = vec3(temp);

    vec3 Tangent =   normalize(mat3(model_)*tangent);
    vec3 Bitangent = normalize(mat3(model_)*bitangent);
    Normal =         normalize(mat3(model_)*normals);
    Tangent =        normalize(Tangent - dot(Tangent, Normal) * Normal);
    TBN = transpose (mat3(Tangent, Bitangent, Normal));    

    FragPos  = TBN * FragPos_;
    viewPos_ = TBN * viewPos;

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    //const float crop = 0.0012;
    TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);
    TexCoordsNormal = vec2(
		textureCoords.x *textureSizeNormal.x + offsetNormal.x, 
		textureCoords.y*textureSizeNormal.y + offsetNormal.y);
    TexCoordsParallax = vec2(
		textureCoords.x *textureSizeParallax.x + offsetParallax.x, 
		textureCoords.y *textureSizeParallax.y + offsetParallax.y);

    maxParallax = vec2(offsetParallax.x + textureSizeParallax.x, offsetParallax.y + textureSizeParallax.y);
    off_ = offsetParallax;
    sz_ = textureSizeParallax;
    model = model_;
    TexCoords_ = textureCoords;

    // perform perspective divide
    vec3 projCoords_ = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords_ * 0.5 + 0.5;
    gl_Position = proj * eyeSpacePosition;
}