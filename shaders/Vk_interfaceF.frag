#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec4 Color;

//layout(binding = 1) uniform FragmentUBuffer
//{
//    vec4 color_;
//} col;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = texture(texSampler, TexCoord)* Color;
}