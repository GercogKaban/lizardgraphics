#version 430 core
#extension GL_EXT_gpu_shader4 : enable

flat in int objectId;
flat in int primitiveId;

out uvec3 color;

void main()
{
    color = uvec3(objectId, primitiveId, gl_PrimitiveID);
}