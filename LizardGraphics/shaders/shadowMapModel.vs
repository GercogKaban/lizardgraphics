#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textureCoords;

uniform mat4 model_;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * model_ * vec4(position, 1.0);
}