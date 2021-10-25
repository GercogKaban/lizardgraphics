#version 430 core
#define MAX_LIGHTS 128
#define MAX_BONES 2
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
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

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
out flat int normalMapping_;
out flat int parallaxMapping_;

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

uniform int parallaxMapping;
uniform int normalMapping;

uniform mat4 finalBonesTrans[MAX_BONES];

layout(std430, binding = 3) buffer layoutName
{
    int flag;
};

void main()
{
    flag = -100;
    normalMapping_ = normalMapping;
    parallaxMapping_ = parallaxMapping;

    if (normalMapping != 0 || parallaxMapping!= 0)
    {
        vec3 Tangent =   normalize(mat3(model_)*tangent);
        vec3 Bitangent = normalize(mat3(model_)*bitangent);
        Normal =         normalize(mat3(model_)*normals);
        Tangent =        normalize(Tangent - dot(Tangent, Normal) * Normal);
        TBN = transpose (mat3(Tangent, Bitangent, Normal));    
    }

    if (parallaxMapping != 0)
    {
        TexCoordsParallax = vec2(
		    textureCoords.x *textureSizeParallax.x + offsetParallax.x, 
		    textureCoords.y *textureSizeParallax.y + offsetParallax.y);

        maxParallax = vec2(offsetParallax.x + textureSizeParallax.x, offsetParallax.y + textureSizeParallax.y);
        off_ = offsetParallax;
        sz_ = textureSizeParallax;
    }

    if (normalMapping != 0)
    {
        TexCoordsNormal = vec2(
		    textureCoords.x *textureSizeNormal.x + offsetNormal.x, 
		    textureCoords.y*textureSizeNormal.y + offsetNormal.y);
    }
    else
        Normal = normalize(mat3(transpose(inverse(model_))) * normals); 

    //bool flag = false;
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(position,1.0f);
            if (flag == -100)
                flag = i;
            break;
        }
        totalPosition += finalBonesTrans[boneIds[i]] * vec4(position,1.0f) * weights[i];
    }

    vec4 temp = model_ * vec4(vec3(totalPosition), 1.0);
    eyeSpacePosition = view*temp;
    vec3 FragPos_ = vec3(temp);
    
    if (normalMapping != 0 || parallaxMapping!= 0)
    {
        FragPos  = TBN * FragPos_;
        viewPos_ = TBN * viewPos;
    }

    else 
    {
        FragPos = FragPos_;
        viewPos_ = viewPos;
    }

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos_, 1.0);
    projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w * 0.5 + 0.5;

    //if (flag == false)
    {
        TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);   
    }
    //else
    {
        //TexCoords = vec2(0.0f,0.0f);
    }
    model = model_;
    TexCoords_ = textureCoords;
    gl_Position = proj * eyeSpacePosition;
}