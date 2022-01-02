#version 430 core
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 albedo;

in VS_OUT
{
in vec3 Normal;
in vec2 BaseTexCoords;
in vec3 FragPos;
in vec2 TexCoordsDiffuse;
in vec2 TexCoordsNormal;
in vec2 TexCoordsParallax;
in vec2 TexCoordsReflex;
in vec4 eyeSpacePosition;
in mat3 TBN;
in mat4 model;
in vec2 off_;
in vec2 sz_;
in vec2 maxParallax;
in flat ivec3 mapping;
} vs;

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D reflexMap;

uniform samplerCube environment;

uniform vec3 viewPos;
uniform bool drawingReflex;
uniform mat4 view;
uniform Fog fog;

float getFogFactor(float fogCoordinate)
{
	float result = exp(-pow(fog.density * fogCoordinate, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

void main()
{
    position = vs.FragPos;
    vec3 Normal = vs.Normal;
//    float fogCoordinate = abs(vs.eyeSpacePosition.z / vs.eyeSpacePosition.w);
//
//    if(fog.isEnabled)
//    {    
//        if (fogCoordinate == 1.0)
//        {
//            albedo = fog.color;
//            return;
//        }
//    }

    if (vs.mapping[0])
    {
        Normal = texture(normalMap, vs.TexCoordsNormal).rgb;
        Normal = normalize(Normal * 2.0 - 1.0);   
        Normal = normalize(vs.TBN * Normal);  
    }  
    normal = Normal;

    albedo = texture(diffuseMap, vs.TexCoordsDiffuse).rgb;
    //albedo = albedo4.rgb;
    if (!drawingReflex && vs.mapping[2] != 0)
    {
        vec4 reflex = texture(reflexMap, vs.TexCoordsReflex);
        float refCoef = (reflex.r + reflex.g + reflex.b)/3.0;
        vec3 I = normalize(position - viewPos);
        vec3 R = reflect(I, vs.Normal);
        albedo.rgb = mix(albedo.rgb,texture(environment, R).rgb,refCoef);
    }

    //if(fog.isEnabled)
       //albedo = mix(albedo, fog.color, getFogFactor(fogCoordinate));
}