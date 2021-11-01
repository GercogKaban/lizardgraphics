#version 430 core

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

in VS_OUT
{
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragPos_;
in vec2 TexCoordsNormal;
in vec2 TexCoordsParallax;
in vec2 TexCoordsReflex;  
in vec3 projCoords;
in vec4 eyeSpacePosition;
in mat3 TBN;
in mat4 model;
in vec3 viewPos_; 
in vec2 TexCoords_;
in vec2 off_;
in vec2 sz_;
in vec2 maxParallax;
in flat ivec3 mapping;
} vs;

out vec4 color;

uniform sampler2D diffuseMap;
uniform Fog fog;

float getFogFactor(float fogCoordinate)
{
	float result = exp(-pow(fog.density * fogCoordinate, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

void main()
{    
    float fogCoordinate = abs(vs.eyeSpacePosition.z / vs.eyeSpacePosition.w);
    if(fog.isEnabled)
    {    
        if (fogCoordinate == 1.0)
        {
            color = vec4(fog.color, 1.0);
            return;
        }
    }

    color = texture(diffuseMap, vs.TexCoords);
    if(fog.isEnabled)
       color = mix(color, vec4(fog.color, 1.0), getFogFactor(fogCoordinate));
    if (color.a - 0.1 < 0) discard;
}
