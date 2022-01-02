#version 430 core
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

uniform mat4 model_;
uniform mat4 lightSpaceMatrix;
uniform mat4 finalBonesTrans[MAX_BONES];

uniform bool playAnimation;

void main()
{
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

    gl_Position = lightSpaceMatrix * model_ * vec4(vec3(totalPosition), 1.0);
}