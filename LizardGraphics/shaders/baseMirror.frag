#version 330 core

out vec4 color;

in vec3 Normal;
in vec3 Position;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    color = vec4(texture(skybox, R).rgb, 1.0);
}