#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in mat4 model;

//uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main() 
{ 
	FragPos = vec3(model * vec4(position, 1.0f)); 
	Normal = mat3(transpose(inverse(model))) * normals;   
	gl_Position =  proj * view * vec4(FragPos, 1.0f); 
	TexCoord = vec2(textureCoords.x, 1.0f - textureCoords.y);
}