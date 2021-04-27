#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aMaterial; // x stores material index. Other three store texture coords?

// Coordinate Matricies
uniform mat4 model;
//uniform mat4 model_inverse;
uniform mat4 view;
uniform mat4 projection;

// Texture
uniform mat4 transform;

// Output
flat out vec3 normal;
out vec3 FragPos;

// Material
out vec3 Ambient;
out vec3 Diffuse;
out vec3 Specular;
out float Shininess;


vec3 materials[] = vec3[](
                      vec3(1.0, 0.0, 0.0), // error

                      vec3(0.0, 1.0, 0.0), // grass
                      vec3(0.478, 0.286, 0.043), // dirt
                      vec3(0.305, 0.403, 0.494), // stone

                      vec3(0.878, 0.843, 0.693), // top sand
                      vec3(0.725, 0.654, 0.254), // sand
                      vec3(0.462, 0.403, 0.074), // packed sand

                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(0.576, 0.768, 0.784), // permafrost
                      vec3(0.713, 0.964, 0.964)); // ice



void main()
{
    FragPos = (model * aPos).xyz;

    normal = aNormal.xyz;

    int material = int(aMaterial.x);


    // Material
    Ambient = 0.5 * materials[material];
    Diffuse = materials[material];
    Specular = vec3(1.0);
    Shininess = 32.0;



    

    gl_Position = projection * view * vec4(FragPos, 1.0); // matrix multiplication is read from right to left
}