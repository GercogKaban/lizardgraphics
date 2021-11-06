#version 430 core
layout(triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (location = 0) in vec4 position_[];

out VS_OUT
{
out vec3 Normal;
out vec2 BaseTexCoords;
out vec3 FragPos;
out vec3 FragPosTBN;
out vec3 viewPosTBN;
out vec2 TexCoordsDiffuse;
out vec2 TexCoordsNormal;
out vec2 TexCoordsParallax;
out vec2 TexCoordsReflex;
out vec3 projCoords;
out vec4 eyeSpacePosition;
out mat3 TBN;
out mat4 model;
out vec2 off_;
out vec2 sz_;
out vec2 maxParallax;
out flat ivec3 mapping;
}  gOut;

in VS_OUT
{
in vec3 Normal;
in vec2 BaseTexCoords;
in vec3 FragPos;
in vec3 FragPosTBN;
in vec3 viewPosTBN;
in vec2 TexCoordsDiffuse;
in vec2 TexCoordsNormal;
in vec2 TexCoordsParallax;
in vec2 TexCoordsReflex;
in vec3 projCoords;
in vec4 eyeSpacePosition;
in mat3 TBN;
in mat4 model;
in vec2 off_;
in vec2 sz_;
in vec2 maxParallax;
in flat ivec3 mapping;
}  vsIn[];

uniform mat4 faceTransforms[6];

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; 
        for(int i = 0; i < gl_in.length(); ++i)
        {
            //vec4 newPos = position_[i];
            //newPos.y = -newPos.y;
            gOut.TexCoordsDiffuse = vsIn[i].TexCoordsDiffuse;
            gOut.eyeSpacePosition = vsIn[i].eyeSpacePosition;
            gl_Position = faceTransforms[face] * position_[i];
            EmitVertex();
        }    
        EndPrimitive();
    }
}  