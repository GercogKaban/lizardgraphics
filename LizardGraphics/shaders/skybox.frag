#version 330 core

out vec4 color;
in vec3 TexCoord;
in vec4 eyeSpacePosition;

uniform samplerCube cubemap;

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

uniform Fog fog;

float getFogFactor(float fogCoordinate)
{
	float result = exp(-pow(fog.density * fogCoordinate, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

void main()
{
    float fogCoordinate = abs(eyeSpacePosition.z / eyeSpacePosition.w);
    if (fog.isEnabled)
    {
        if (fogCoordinate == 1.0)
        {
            color = vec4(fog.color,1.0);
            return;
        }
    }
    
    color = texture(cubemap, TexCoord);
    if(fog.isEnabled)
      color = mix(color, vec4(fog.color, 1.0), getFogFactor(fogCoordinate));
}