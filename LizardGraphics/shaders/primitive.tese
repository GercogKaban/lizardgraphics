#version 430 core
 
layout(triangles, equal_spacing, ccw) in;

layout(location = 0) in vec3 position[];
layout(location = 1) in vec3 normals[];

uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
 
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
//out vec3 Position;
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
} tes_out;

 
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

in PnPatch iPnPatch[];

#define b300    position[0]
#define b030    position[1]
#define b003    position[2]
#define n200    normals[0]
#define n020    normals[1]
#define n002    normals[2]
#define uvw     gl_TessCoord

void main()
{
    tes_out.FragPos = vs[0].FragPos;
    tes_out.TexCoordsNormal = vs[0].TexCoordsNormal;
    tes_out.TexCoordsParallax = vs[0].TexCoordsParallax;
    tes_out.projCoords = vs[0].projCoords;
    tes_out.eyeSpacePosition = vs[0].eyeSpacePosition;
    tes_out.TBN = vs[0].TBN;
    tes_out.model = vs[0].model;
    tes_out.viewPos_ = vs[0].viewPos_;
    tes_out.off_ = vs[0].off_;
    tes_out.sz_ = vs[0].sz_;
    tes_out.maxParallax = vs[0].maxParallax;
    tes_out.normalMapping_ = vs[0].normalMapping_;
    tes_out.parallaxMapping_ = vs[0].parallaxMapping_;


 vec3 uvwSquared = uvw*uvw;
 vec3 uvwCubed   = uvwSquared*uvw;

 // extract control points
 vec3 b210 = vec3(iPnPatch[0].b210, iPnPatch[1].b210, iPnPatch[2].b210);
 vec3 b120 = vec3(iPnPatch[0].b120, iPnPatch[1].b120, iPnPatch[2].b120);
 vec3 b021 = vec3(iPnPatch[0].b021, iPnPatch[1].b021, iPnPatch[2].b021);
 vec3 b012 = vec3(iPnPatch[0].b012, iPnPatch[1].b012, iPnPatch[2].b012);
 vec3 b102 = vec3(iPnPatch[0].b102, iPnPatch[1].b102, iPnPatch[2].b102);
 vec3 b201 = vec3(iPnPatch[0].b201, iPnPatch[1].b201, iPnPatch[2].b201);
 vec3 b111 = vec3(iPnPatch[0].b111, iPnPatch[1].b111, iPnPatch[2].b111);

 // extract control normals
 vec3 n110 = normalize(vec3(iPnPatch[0].n110,
                            iPnPatch[1].n110,
                            iPnPatch[2].n110));
 vec3 n011 = normalize(vec3(iPnPatch[0].n011,
                            iPnPatch[1].n011,
                            iPnPatch[2].n011));
 vec3 n101 = normalize(vec3(iPnPatch[0].n101,
                            iPnPatch[1].n101,
                            iPnPatch[2].n101));

 // compute texcoords
 tes_out.TexCoords  = gl_TessCoord[2]*vs[0].TexCoords
            + gl_TessCoord[0]*vs[1].TexCoords
            + gl_TessCoord[1]*vs[2].TexCoords;

 tes_out.TexCoordsNormal =  tes_out.TexCoords;
 tes_out.TexCoordsParallax = tes_out.TexCoords;

 // normal
 vec3 barNormal = gl_TessCoord[2]*normals[0]
                + gl_TessCoord[0]*normals[1]
                + gl_TessCoord[1]*normals[2];

 vec3 pnNormal  = n200*uvwSquared[2]
                + n020*uvwSquared[0]
                + n002*uvwSquared[1]
                + n110*uvw[2]*uvw[0]
                + n011*uvw[0]*uvw[1]
                + n101*uvw[2]*uvw[1];

float uTessAlpha = 0.001f;
tes_out.Normal = uTessAlpha*pnNormal + (1.0-uTessAlpha)*barNormal;

tes_out.TBN[2] = tes_out.Normal;

 // compute interpolated pos
 vec3 barPos = gl_TessCoord[2]*b300
             + gl_TessCoord[0]*b030
             + gl_TessCoord[1]*b003;

 // save some computations
 uvwSquared *= 3.0;

 // compute PN position
 vec3 pnPos  = b300*uvwCubed[2]
             + b030*uvwCubed[0]
             + b003*uvwCubed[1]
             + b210*uvwSquared[2]*uvw[0]
             + b120*uvwSquared[0]*uvw[2]
             + b201*uvwSquared[2]*uvw[1]
             + b021*uvwSquared[0]*uvw[1]
             + b102*uvwSquared[1]*uvw[2]
             + b012*uvwSquared[1]*uvw[0]
             + b111*6.0*uvw[0]*uvw[1]*uvw[2];

    vec3 finalPos = (1.0-uTessAlpha)*barPos + uTessAlpha*pnPos;
    tes_out.FragPos = finalPos;
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(tes_out.FragPos, 1.0);
    tes_out.projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w * 0.5 + 0.5;
    tes_out.eyeSpacePosition = view*vec4(finalPos,1.0f);
    gl_Position = proj * tes_out.eyeSpacePosition;
}