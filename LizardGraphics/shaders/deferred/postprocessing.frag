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

uniform mat4 view;
uniform mat4 proj;
uniform mat4 invView;
uniform mat4 invProj;
//uniform mat4 invProjView;
//uniform mat4 projView;

out vec4 color;

vec3 ssToViewSpace(vec2 uv, float depth)
{
	vec4 position = vec4(1.0f); 
 
	position.x = uv.x * 2.0f - 1.0f; 
	position.y = -(uv.y * 2.0f - 1.0f); 
     
	position.z = depth;//* 2.0f - 1.0f; 
 
	position = invProj * position;
	return position.xyz / position.w;
}

//vec3 ssToWorldSpace(vec2 uv, float depth)
//{
//	vec4 position = vec4(1.0f); 
// 
//	position.x = uv.x * 2.0f - 1.0f; 
//	position.y = -(uv.y * 2.0f - 1.0f); 
//
//	position.z = depth; 
// 
//	position = invProjView * position; 
// 
//	position /= position.w;
//
//	return position.xyz;
//}

vec2 viewSpaceToSs(vec3 position)
{
	 vec4 pVP = proj * vec4(position,1.0f);
     pVP.xy /= pVP.w;
     pVP.xy = pVP.xy * 0.5 + 0.5;
	 return pVP.xy;
}

//vec3 worldSpaceToViewSpace(vec3 position)
//{
//	 vec4 pVP = view * vec4(position,1.0f);
//     return pVP.xyz;
//}
//
//vec2 worldSpaceToSs(vec3 position)
//{
//	 vec4 pVP = proj * inverse(invView) * vec4(position,1.0f);
//	 pVP.xy = vec2(0.5f) + vec2(0.5f) * pVP.xy / pVP.w;
//	 return pVP.xy;
//}
//

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

vec3 ssr(vec3 startCoord, vec3 reflectDir)
{
    const float stepSize = 0.01f;
    const vec3 step = reflectDir * stepSize;
    const float maxSteps = 100;
 
    vec3 currentRay = startCoord;
    for(int i = 0; i < maxSteps; i++)
    {
        currentRay+= step;
        vec2 projectedCoords = viewSpaceToSs(currentRay);

        float depth = getDepth(projectedCoords);

        if(depth > 1.0)
            continue;

        float dDepth = currentRay.z - depth;

        if((currentRay.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
                return getColor(projectedCoords).xyz;
        }

//        if(getDepth(projectedCoords) > 0.0f) 
//            return getColor(projectedCoords).xyz;
    }   
    return getColor(TexCoords).xyz;
}

void main()
{
    vec4 texelNormal = getNormal(TexCoords);
    vec3 texelNormalVS = ((view * texelNormal).xyz);
    vec3 texelPositionVS = ssToViewSpace(TexCoords, getDepth(TexCoords));

    //vec3 viewDirVS  = texelPositionVS;

    vec3 reflectDirVS = normalize(reflect(normalize(texelPositionVS), normalize(texelNormalVS)));

    if (texelNormal.g == 1.0f)
        color = vec4(ssr(texelPositionVS,reflectDirVS),1.0f);
    else
        color = getColor(TexCoords);
}