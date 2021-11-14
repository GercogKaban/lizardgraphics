#version 430 core

struct Fog
{
    vec3 color;
    float density;
    bool isEnabled;
};

in vec2 TexCoords;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform Fog fog;

uniform vec3 viewPos;
uniform mat4 inverseViewProjection;
uniform mat4 viewProjection;

out vec4 color;

vec3 getPosition(vec2 UV, float depth)
{
	vec4 position = vec4(1.0f); 
 
	position.x = UV.x * 2.0f - 1.0f; 
	position.y = -(UV.y * 2.0f - 1.0f); 

	position.z = depth;  
	position = mul(position, inverseViewProjection); 
	position /= position.w;

	return position.xyz;
}

float getDepth(vec2 uv)
{
    return texture(depthMap,uv).r;
}

vec4 getColor(vec2 uv)
{
    return texture(colorMap,uv);
}

vec4 getNormal(vec2 uv)
{
    return texture(normalMap,uv);
}

vec3 getUV(vec3 position)
{
	 vec4 pVP = mul(vec4(position, 1.0f), viewProjection);
	 pVP.xy = vec2(0.5f, 0.5f) + vec2(0.5f, -0.5f) * pVP.xy / pVP.w;
	 return vec3(pVP.xy, pVP.z / pVP.w);
}

vec3 sslr(vec3 viewDir, vec3 reflectDir, vec3 texelPosition)
{
    float LFactor = 0.01f;
    //float LDelmiter = 1.0f;
    vec3 currentRay = vec3(0.0f);
    vec3 nuv = vec3(0.0f);
    float L = LFactor;
    //float error = 1.0f;

    for(int i = 0; i < 10; i++)
    {
        currentRay = texelPosition + reflectDir * L;

        nuv = getUV(currentRay);
        float n = getDepth(nuv.xy);
        if (n != 1.0f)
            break;
        vec3 newPosition = getPosition(nuv.xy, n);
        L+=LFactor;
        //L = length(texelPosition - newPosition);
        //L = clamp(L * LDelmiter,0.0,1.0);
        //error *= (1.0f - L);
    }
    return getColor(nuv.xy).rgb;
}

void main()
{
    vec3 texelNormal = getNormal(TexCoords).xyz;
    vec3 texelPosition = getPosition(TexCoords, getDepth(TexCoords));
    vec3 viewDir  = normalize(texelPosition - viewPos);
    vec3 reflectDir = normalize(reflect(viewDir, texelNormal));
    //reflectDir*= 0.0 + 2.8 * pow(1+dot(viewDir, texelNormal), 2);

    if (fog.color == getColor(TexCoords).xyz)
        color = getColor(TexCoords);
    else
        color = vec4(sslr(viewDir,reflectDir,texelPosition),1.0f);
}