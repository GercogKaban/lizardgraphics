#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 proj;
uniform mat4 view;

out vec3 TexCoord;

void main()
{
    vec4 pos =  proj * view * vec4(position, 1.0f);
    TexCoord = position;
    gl_Position = pos.xyww;
}