#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBuffer 
{
	mat4 model;
	vec4 color;
	int materialNum;
} buf_;

layout( push_constant ) uniform TransformData 
{	
	mat4 projView;
	vec3 viewPos;
    vec3 lightPos;
    vec3 ambient;
    vec3 diffuse;
	//int lightning;
} cnst;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 lightPos;
layout(location = 3) out vec3 ambient;
layout(location = 4) out vec3 diffuse;
layout(location = 5) out vec3 specular;
layout(location = 6) out vec4 Color;
layout(location = 7) out vec3 FragPos;
layout(location = 8) out vec3 viewPos;
layout(location = 9) out int lightning;

void main()
{
	FragPos = vec3(buf_.model * vec4(position, 1.0f));
	gl_Position = cnst.projView* vec4(FragPos, 1.0);
	TexCoord = texCoord;
	Color = vec4(1.0,1.0,1.0,1.0);
	//lightning = cnst.lightning;
	lightning = 1;

	if (lightning != 0)
	{
		Normal = normalize(mat3(transpose(inverse(buf_.model))) * normal);
	    lightPos = cnst.lightPos;
		ambient = cnst.ambient;
		diffuse = cnst.diffuse;
		specular = vec3(1.0f,1.0f,1.0f);
		viewPos = cnst.viewPos;
	}
}