#version 430 core
 
layout (vertices = 3) out;
layout (location = 0) in vec3 position[];
layout (location = 1) in vec3 normals[];

layout(location = 0) out vec3 oPosition[];
layout(location = 1) out vec3 oNormals[];
 
uniform float tessellationLevel;

in VS_OUT
{
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;
in vec2 TexCoordsNormal;
in vec2 TexCoordsParallax;  
in vec3 projCoords;
in vec4 eyeSpacePosition;
in mat3 TBN;
in mat4 model;
in vec3 viewPos_; 
in vec2 TexCoords_;
in vec2 off_;
in vec2 sz_;
in vec2 maxParallax;
in flat int normalMapping_;
in flat int parallaxMapping_;
} vs[];

out VS_OUT
{
out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;
out vec2 TexCoordsNormal;
out vec2 TexCoordsParallax;
out vec3 projCoords;
out vec4 eyeSpacePosition;
out mat3 TBN;
out mat4 model;
out vec3 viewPos_; 
out vec2 TexCoords_;
out vec2 off_;
out vec2 sz_;
out vec2 maxParallax;
out flat int normalMapping_;
out flat int parallaxMapping_;
} ts_out[];
 
struct PnPatch
{
 float b210;
 float b120;
 float b021;
 float b012;
 float b102;
 float b201;
 float b111;
 float n110;
 float n011;
 float n101;
};
 
out patch PnPatch oPnPatch[3];

float wij(int i, int j)
{
   return dot(position[j] - position[i], normals[i]);
}

float vij(int i, int j)
{
 vec3 Pj_minus_Pi = position[j] - position[i];
 vec3 Ni_plus_Nj  = normals[i] + normals[j];
 return 2.0*dot(Pj_minus_Pi, Ni_plus_Nj)/dot(Pj_minus_Pi, Pj_minus_Pi);
}

void main()
{
    oPosition[gl_InvocationID] = position[gl_InvocationID];
    oNormals[gl_InvocationID]   = normals[gl_InvocationID];

    ts_out[gl_InvocationID].Normal = vs[gl_InvocationID].Normal;
    ts_out[gl_InvocationID].TexCoords = vs[gl_InvocationID].TexCoords;
    ts_out[gl_InvocationID].FragPos = vs[gl_InvocationID].FragPos;
    ts_out[gl_InvocationID].TexCoordsNormal = vs[gl_InvocationID].TexCoordsNormal;
    ts_out[gl_InvocationID].TexCoordsParallax = vs[gl_InvocationID].TexCoordsParallax;
    ts_out[gl_InvocationID].projCoords = vs[gl_InvocationID].projCoords;
    ts_out[gl_InvocationID].eyeSpacePosition = vs[gl_InvocationID].eyeSpacePosition;
    ts_out[gl_InvocationID].TBN = vs[gl_InvocationID].TBN;
    ts_out[gl_InvocationID].model = vs[gl_InvocationID].model;
    ts_out[gl_InvocationID].viewPos_ = vs[gl_InvocationID].viewPos_;
    ts_out[gl_InvocationID].TexCoords_ = vs[gl_InvocationID].TexCoords_;
    ts_out[gl_InvocationID].off_ = vs[gl_InvocationID].off_;
    ts_out[gl_InvocationID].sz_ = vs[gl_InvocationID].sz_;
    ts_out[gl_InvocationID].maxParallax = vs[gl_InvocationID].maxParallax;
    ts_out[gl_InvocationID].normalMapping_ = vs[gl_InvocationID].normalMapping_;
    ts_out[gl_InvocationID].parallaxMapping_ = vs[gl_InvocationID].parallaxMapping_;

 float P0 = position[0][gl_InvocationID];
 float P1 = position[1][gl_InvocationID];
 float P2 = position[2][gl_InvocationID];
 float N0 = normals[0][gl_InvocationID];
 float N1 = normals[1][gl_InvocationID];
 float N2 = normals[2][gl_InvocationID];

 // compute control points
 oPnPatch[gl_InvocationID].b210 = (2.0*P0 + P1 - wij(0,1)*N0)/3.0;
 oPnPatch[gl_InvocationID].b120 = (2.0*P1 + P0 - wij(1,0)*N1)/3.0;
 oPnPatch[gl_InvocationID].b021 = (2.0*P1 + P2 - wij(1,2)*N1)/3.0;
 oPnPatch[gl_InvocationID].b012 = (2.0*P2 + P1 - wij(2,1)*N2)/3.0;
 oPnPatch[gl_InvocationID].b102 = (2.0*P2 + P0 - wij(2,0)*N2)/3.0;
 oPnPatch[gl_InvocationID].b201 = (2.0*P0 + P2 - wij(0,2)*N0)/3.0;
 float E = ( oPnPatch[gl_InvocationID].b210
           + oPnPatch[gl_InvocationID].b120
           + oPnPatch[gl_InvocationID].b021
           + oPnPatch[gl_InvocationID].b012
           + oPnPatch[gl_InvocationID].b102
           + oPnPatch[gl_InvocationID].b201 ) / 6.0;
 float V = (P0 + P1 + P2)/3.0;
 oPnPatch[gl_InvocationID].b111 = E + (E - V)*0.5;
 oPnPatch[gl_InvocationID].n110 = N0+N1-vij(0,1)*(P1-P0);
 oPnPatch[gl_InvocationID].n011 = N1+N2-vij(1,2)*(P2-P1);
 oPnPatch[gl_InvocationID].n101 = N2+N0-vij(2,0)*(P0-P2);
 
    // Calculate the tessellation levels
 gl_TessLevelOuter[gl_InvocationID] = tessellationLevel;
 gl_TessLevelInner[0] = tessellationLevel;
}