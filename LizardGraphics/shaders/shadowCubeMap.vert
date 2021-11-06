#version 430 core
layout (location = 0) in vec3 position;
layout (location = 5) in mat4 model_;

void main()
{
    gl_Position = model_ * vec4(position, 1.0);
}  