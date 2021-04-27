#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;

// Material
in vec3 Ambient;
in vec3 Diffuse;
in vec3 Specular;
in float Shininess;

void main()
{
    FragColor = vec4(lightColor, 1.0);
}