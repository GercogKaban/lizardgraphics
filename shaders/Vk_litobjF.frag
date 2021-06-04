#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 lightPos;
layout(location = 3) in vec3 ambient;
layout(location = 4) in vec3 diffuse;
layout(location = 5) in vec3 specular;
layout(location = 6) in vec4 Color;
layout(location = 7) in vec3 FragPos;
layout(location = 8) in vec3 viewPos;
layout(location = 9) flat in int lightning;

layout(binding = 0) uniform UniformBuffer 
{
	mat4 model;
	vec4 color;
	int materialNum;
} buf_;


//layout(binding = 2) uniform MaterialNum 
//{
    //int num;
//} materialNum;

layout(binding = 1) uniform sampler2D texSampler;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

const Material materials[] =
{
  // No material
  {{1.0,1.0,1.0},{1.0,1.0,1.0},{0.0,0.0,0.0},0.0},
  // Emerald
  {{0.0215,0.1745,0.0215},{0.07568,0.61424,0.07568},{0.633,0.727811,0.633},0.6},
  // Jade
  {{0.135,0.2225,0.1575},{0.54,0.89,0.63},{0.316228,0.316228,0.316228 },0.1},
  // Obsidian
  {{0.05375,0.05,0.06625},{0.18275,0.17,0.22525},{0.332741,0.328634,0.346435},0.3},
  // Pearl
  {{0.25,0.20725,0.20725},{1,0.829,0.829},{0.296648,0.296648,0.296648}, 0.088},
  // Ruby
  {{0.1745,0.01175,0.01175},{0.61424,0.04136,0.04136},{0.727811,0.626959,0.626959},0.6},
  // Turquoise
  {{0.1,0.18725,0.1745},{0.396,0.74151,0.69102},{0.297254,0.30829,0.306678},0.1},
  // Brass
  {{0.329412,0.223529,0.027451},{0.780392,0.568627,0.113725},{0.992157,0.941176,0.807843},0.2179487},
  // Bronze
  {{0.2125,0.1275,0.054},{0.714,0.4284,0.18144 },{0.393548,0.271906,0.166721},0.2},
  // Chrome
  {{0.25,0.25,0.25},{0.4,0.4,0.4},{0.774597,0.774597,0.774597},0.6},
  // Copper
  {{0.19125,0.0735,0.0225},{0.7038,0.27048,0.0828},{0.256777,0.137622,0.086014},0.1},
  // Gold
  {{0.24725,0.1995,0.0745},{0.75164,0.60648,0.22648},{0.628281,0.555802,0.366065},0.4},
  // Silver
  {{0.19225,0.19225,0.19225},{0.50754,0.50754,0.50754},{0.508273,0.508273,0.508273},0.4},
  // Black plastic
  {{0.0215,0.1745,0.0215},{0.01,0.01,0.01},{0.5,0.5,0.5},0.25},
  // Cyan plastic
  {{0.0,0.1,0.06},{0.0,0.50980392,0.50980392},{0.50196078,0.50196078,0.50196078},0.25},
  // Green plastic
  {{0.0,0.0,0.0},{0.1,0.35,0.1},{0.45,0.55,0.45},0.25},
  // Red plastic
  {{0.0,0.0,0.0},{0.5,0.0,0.0},{0.7,0.6,0.6},0.25},
  // White plastic
  {{0.0,0.0,0.0},{0.55,0.55,0.55},{0.70,0.70,0.70},0.25},
  // Yellow plastic
  {{0.0,0.0,0.0},{0.5,0.5,0.0},{0.60,0.60,0.50},0.25},
  // Black rubber
  {{0.02,0.02,0.02},{0.01,0.01,0.01},{0.4,0.4,0.4},0.078125},
  // Cyan rubber
  {{0.0,0.05,0.05},{0.4,0.5,0.5},{0.04,0.7,0.7},0.078125},
  // Green rubber
  {{0.0,0.05,0.0},{0.4,0.5,0.4},{0.04,0.7,0.04},0.078125},
  // Red rubber
  {{0.05,0.0,0.0},{0.5,0.4,0.4},{0.7,0.04,0.04},0.078125},
  // White rubber
  {{0.05,0.05,0.05},{0.5,0.5,0.5},{0.7,0.7,0.7},0.078125},
  // Yellow rubber
  {{0.05,0.05,0.0},{0.5,0.5,0.4},{0.7,0.7,0.04},0.078125},
};

layout(location = 0) out vec4 outColor;

void main() 
{
    if (lightning != 0)
    {
        Material material = materials[buf_.materialNum];
        vec3 ambientVec = ambient * material.ambient;
  
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuseVec = diffuse * (diff * material.diffuse);
    
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specularVec = specular * (spec * material.specular);  
        
        vec4 result = vec4(ambientVec + diffuseVec + specularVec,1.0) * Color;
	    outColor = result*texture(texSampler, TexCoord);
    }

    else
        outColor = texture(texSampler, TexCoord)* Color;
}