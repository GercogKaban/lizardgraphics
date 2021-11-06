#version 430 core

layout (location = 0) in vec3 position;
layout (location = 5) in mat4 model_;
layout (location = 17) in ivec2 ids;

flat out int objectId;
flat out int primitiveId;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 FragPos = model_ * vec4(vec3(position), 1.0);
    vec4 eyeSpacePosition = view*FragPos;
    objectId = ids[0];
    primitiveId = ids[1];
    gl_Position = proj * eyeSpacePosition;
}