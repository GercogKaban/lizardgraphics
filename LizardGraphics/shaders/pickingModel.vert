#version 430 core

layout (location = 0) in vec3 position;

uniform ivec2 ids;

uniform mat4 model_;
uniform mat4 view;
uniform mat4 proj;

flat out int objectId;
flat out int primitiveId;

void main()
{
    vec4 FragPos = model_ * vec4(vec3(position), 1.0);
    vec4 eyeSpacePosition = view*FragPos;
    objectId = ids[0];
    primitiveId = ids[1];
    gl_Position = proj * eyeSpacePosition;
}