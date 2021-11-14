#version 430 core
#define MAX_LIGHTS 100

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in mat4 model_;
layout (location = 9)  in vec4 diffuseCoords;
layout (location = 10) in vec4 normalCoords;
layout (location = 11) in vec4 heightCoords;
layout (location = 12) in vec4 reflexCoords;
layout (location = 13) in ivec3 mapping;
layout (location = 14) in mat3 inverseModel;

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

out VS_OUT
{
out vec3 Normal;
out vec2 BaseTexCoords;
out vec3 FragPos;
out vec2 TexCoordsDiffuse;
out vec2 TexCoordsNormal;
out vec2 TexCoordsParallax;
out vec2 TexCoordsReflex;
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
uniform mat4 lightSpaceMatrix[6];

void main()
{
    vec4 FragPos = model_ * vec4(position, 1.0);
    vs.Normal = normalize(inverseModel * normals);
    vs.eyeSpacePosition = view*FragPos;
    vs.mapping[0] = mapping[0];
    vs.mapping[1] = mapping[1];
    vs.mapping[2] = mapping[2];

    vs.FragPos = FragPos.xyz;

    if (vs.mapping[0]!=0 || vs.mapping[1]!=0)
    {
        vec3 T = normalize(vec3(model_ * vec4(tangent,   0.0)));
        vec3 B = normalize(vec3(model_ * vec4(bitangent, 0.0)));
        vec3 N = normalize(vec3(model_ * vec4(normals,    0.0)));
        T = normalize(T - dot(T, N) * N);
        vs.TBN = mat3(T, B, N);    
    }

        vs.TexCoordsDiffuse = vec2(
		    textureCoords.x *diffuseCoords[2] + diffuseCoords[0], 
		    textureCoords.y *diffuseCoords[3] + diffuseCoords[1]);

        if (mapping[0] != 0)
        {
            vs.TexCoordsNormal = vec2(
		    textureCoords.x *normalCoords[2] + normalCoords[0], 
		    textureCoords.y *normalCoords[3] + normalCoords[1]);
        }

        if (mapping[1] != 0)
        {
            vs.TexCoordsParallax = vec2(
		    textureCoords.x *heightCoords[2] + heightCoords[0], 
		    textureCoords.y *heightCoords[3] + heightCoords[1]);
            vs.maxParallax = vec2(heightCoords[0] + heightCoords[2], heightCoords[1] + heightCoords[3]);
            vs.off_ = vec2(heightCoords[0],heightCoords[1]);
            vs.sz_ = vec2(heightCoords[2],heightCoords[3]);
        }

        if (mapping[2] != 0)
        {
            vs.TexCoordsReflex = vec2(
		    textureCoords.x *reflexCoords[2] + reflexCoords[0], 
		    textureCoords.y *reflexCoords[3] + reflexCoords[1]);
        }

    vs.model = model_;
    vs.BaseTexCoords = textureCoords;
    gl_Position = proj * vs.eyeSpacePosition;
}