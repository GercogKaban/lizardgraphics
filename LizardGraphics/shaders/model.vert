#version 430 core
#define MAX_LIGHTS 128
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

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
uniform mat4 lightSpaceMatrix[6];
uniform vec3 viewPos;
uniform mat3 inverseModel;

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
    vs.model = model_;
    vs.mapping[0] = normalMapping;
    vs.mapping[1] = parallaxMapping;
    vs.mapping[2] = reflexMapping;

    vec3 Tangent, Bitangent;

    if (parallaxMapping != 0)
    {
        vs.TexCoordsParallax = vec2(
		    textureCoords.x *textureSizeParallax.x + offsetParallax.x, 
		    textureCoords.y *textureSizeParallax.y + offsetParallax.y);

        vs.maxParallax = vec2(offsetParallax.x + textureSizeParallax.x, offsetParallax.y + textureSizeParallax.y);
        vs.off_ = offsetParallax;
        vs.sz_ = textureSizeParallax;
    }

    if (normalMapping != 0)
    {
        vs.TexCoordsNormal = vec2(
		    textureCoords.x *textureSizeNormal.x + offsetNormal.x, 
		    textureCoords.y*textureSizeNormal.y + offsetNormal.y);
    }

    if (reflexMapping != 0)
    {
            vs.TexCoordsReflex = vec2(
		    textureCoords.x *textureSizeReflex.x + offsetReflex.x, 
		    textureCoords.y*textureSizeReflex.y + offsetReflex.y);
    }

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
    vs.eyeSpacePosition = view*temp;
    vs.FragPos = vec3(temp);
    if (normalMapping != 0 || parallaxMapping!= 0)
    {
        vec3 Tangent =   normalize(mat3(model_)*tangent);
        vec3 Bitangent = normalize(mat3(model_)*bitangent);
        vec3 Normal =      normalize(mat3(model_)*normals);
        Tangent =        normalize(Tangent - dot(Tangent, Normal) * Normal);  
        vs.TBN = transpose (mat3(Tangent, Bitangent, Normal));  
    }
    if (normalMapping != 0 || parallaxMapping!= 0)
    {
        vs.FragPosTBN  = vs.TBN * vs.FragPos;
        vs.viewPosTBN = vs.TBN * viewPos;
    }

    vec4 FragPosLightSpace = lightSpaceMatrix[0] * vec4(vs.FragPos, 1.0);
    vs.projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w * 0.5 + 0.5;
    
    vs.TexCoordsDiffuse = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);   
    vs.model = model_;
    vs.BaseTexCoords = textureCoords;
    vs.Normal = normalize(inverseModel * normals); 
    gl_Position = proj * vs.eyeSpacePosition;
}