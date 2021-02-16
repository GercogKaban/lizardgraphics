#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform VertexUBuffer 
{
	mat4 model;
	vec4 color;
} buf_;

layout( push_constant ) uniform TransformData 
{	
	mat4 projView;
	vec3 lightPos;
	vec3 viewPos;
	vec3 lightColor;
	float ambient;
	float specular;
} cnst;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 lightPos;
layout(location = 3) out vec3 lightColor;
layout(location = 4) out float ambient;
layout(location = 5) out float specular;
layout(location = 6) out vec4 Color;
layout(location = 7) out vec3 FragPos;
layout(location = 8) out vec3 viewPos;

void main()
{
	FragPos = vec3(buf_.model * vec4(position, 1.0f));
	gl_Position = cnst.projView* vec4(FragPos, 1.0);
	TexCoord = texCoord;
	Normal = normalize(mat3(transpose(inverse(buf_.model))) * normal);
	lightPos = cnst.lightPos;
	lightColor = cnst.lightColor;
	ambient = cnst.ambient;
	specular = cnst.specular;
	viewPos = cnst.viewPos;
	Color = vec4(1.0,1.0,1.0,1.0);
}