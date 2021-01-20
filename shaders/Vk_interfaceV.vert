#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform VertexUBuffer 
{
	mat4 model;
	vec4 color;
} buf_;

layout( push_constant ) uniform TransformData 
{	
    mat4 proj;
    mat4 view;
} cnst;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec4 Color;

void main()
{
	gl_Position = buf_.model* vec4(position,1.0);
	//gl_Position = cnst.proj* cnst.view* buf_.model* vec4(position,1.0);
	TexCoord = texCoord;
	Color = buf_.color;
}