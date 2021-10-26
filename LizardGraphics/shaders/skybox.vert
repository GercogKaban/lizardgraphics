#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 proj;
uniform mat4 view;
uniform float skyboxDepth;

out vec3 TexCoord;
out vec4 eyeSpacePosition;

void main()
{
    eyeSpacePosition = view * vec4(position, skyboxDepth);
    vec4 pos =  proj * eyeSpacePosition;
    TexCoord = position;
    gl_Position = pos.xyww;
}