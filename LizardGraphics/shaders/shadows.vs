#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in mat4 model_;
layout (location = 9) in vec2 offset;
layout (location = 10) in vec2 textureSize;
layout (location = 11) in vec2 offsetNormal;
layout (location = 12) in vec2 textureSizeNormal;
layout (location = 13) in vec2 offsetParallax;
layout (location = 14) in vec2 textureSizeParallax;
layout (location = 15) in ivec2 mapping;

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

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec4 temp = model_ * vec4(position, 1.0);
    eyeSpacePosition = view*temp;
    vec3 FragPos_ = vec3(temp);
    normalMapping_ = mapping[0];
    parallaxMapping_ = mapping[1];

    FragPos = FragPos_;
    viewPos_ = viewPos;

    if (normalMapping_!=0 || parallaxMapping_!=0)
    {
        vec3 Tangent =   normalize(mat3(model_)*tangent);
        vec3 Bitangent = normalize(mat3(model_)*bitangent);
        Normal =         normalize(mat3(model_)*normals);
        Tangent =        normalize(Tangent - dot(Tangent, Normal) * Normal);
        TBN = transpose(mat3(Tangent, Bitangent, Normal));    

        FragPos  = TBN * FragPos_;
        viewPos_ = TBN * viewPos;
    }

    if (parallaxMapping_)
    {
        TexCoordsParallax = vec2(
		    textureCoords.x *textureSizeParallax.x + offsetParallax.x, 
		    textureCoords.y *textureSizeParallax.y + offsetParallax.y);

        maxParallax = vec2(offsetParallax.x + textureSizeParallax.x, offsetParallax.y + textureSizeParallax.y);
        off_ = offsetParallax;
        sz_ = textureSizeParallax;
    }

    if (normalMapping_)
    {
        TexCoordsNormal = vec2(
		    textureCoords.x *textureSizeNormal.x + offsetNormal.x, 
		    textureCoords.y*textureSizeNormal.y + offsetNormal.y);
    }
    else
        Normal = normalize(mat3(transpose(inverse(model_))) * normals); 

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    vec3 projCoords_ = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords_ * 0.5 + 0.5;

    TexCoords = vec2(
		textureCoords.x *textureSize.x + offset.x , 
		textureCoords.y*textureSize.y + offset.y);
    model = model_;
    TexCoords_ = textureCoords;


    gl_Position = proj * eyeSpacePosition;
}