#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

out vec3 Normal;
out vec3 Position;

void main()
{
	Normal = mat3(transpose(inverse(model))) * normals;
	Position = vec3(model * vec4(position, 1.0));
    gl_Position =  proj * view * model * vec4(position, 1.0f);
}