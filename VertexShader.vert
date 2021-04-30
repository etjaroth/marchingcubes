#version 430 core
#define PI 3.14159265358979323846

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

// Waves
uniform float wavetime;

vec3 materials[] = vec3[](
                      vec3(0.514,0.843,0.933), // water

                      vec3(0.0, 1.0, 0.0), // grass
                      vec3(0.478, 0.286, 0.043), // dirt
                      vec3(0.305, 0.403, 0.494), // stone

                      vec3(0.878, 0.843, 0.693), // top sand
                      vec3(0.725, 0.654, 0.254), // sand
                      vec3(0.462, 0.403, 0.074), // packed sand

                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(0.576, 0.768, 0.784), // permafrost
                      vec3(0.713, 0.964, 0.964)); // ice

const vec3 wave_triangle_verticies[6][2] = {
    {vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)}, 
    {vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 1.0)}, 
    {vec3(0.0, 0.0, -1.0), vec3(1.0, 0.0, 1.0)}, 

    {vec3(1.0, 0.0, -1.0), vec3(1.0, 0.0, 0.0)}, 
    {vec3(-1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)}, 
    {vec3(-1.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0)}};

//const vec3 wave_triangle_verticies[6][2] = {
//    {vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)}, 
//    {vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)}, 
//    {vec3(0.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0)}, 
//
//    {vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 0.0)}, 
//    {vec3(1.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)}, 
//    {vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0)}};

// Water suface index chart
//      x ->
// -z  ________________________
//  |  |\  3                5 |
//  v  |  \                   |
//     | 2  \                 |
//     |      \               |
//     |        \             |
//     |          \           |
//     |            \         |
//     |              \       |
//     |                \  4  |
//     |                  \   |
//     | 0               1  \ |
//     ''''''''''''''''''''''''



const float wavelength = 2.0;
const float waveamp = 0.5;

float wave(vec2 v) {
    return waveamp * 0.5 * (sin(v.x / wavelength + wavetime) + cos(v.y / wavelength + wavetime));
}

vec3 apply_wave(vec3 v) {
    v.y += wave(v.xz);
    return v;
}

void main()
{
    vec4 vertex_position = aPos;
    int material = int(aMaterial.x);

    normal = aNormal.xyz;

    //////////////////////////////////////////////////////////////////////////

    vec3 material_colour = materials[material];
    Shininess = 8.0;

    // Waves
    if (material == 0) {
        vertex_position.y += wave(vertex_position.xz);

        Shininess = 32.0;

        // Recalculate normal

        // Find other verticies in triangle
        vec3 orgin = vertex_position.xyz;
        int index = abs(int(normal.y)) - 1; // -1 is because normal.y is 1 without encoding
        vec3 v1 = wave_triangle_verticies[index][0];
        v1.y += wave(v1.xz + orgin.xz);
        vec3 v2 = wave_triangle_verticies[index][1];
        v2.y += wave(v2.xz + orgin.xz);

        
		vec3 cross1 = v1;
		vec3 cross2 = v2;
		normal = -cross(cross1, cross2);
        material_colour = materials[0];

    }
    
    //////////////////////////////////////////////////////////////////////////

    Ambient = 0.5 * material_colour;
    Diffuse = material_colour;
    Specular = vec3(1.0);
    

    //////////////////////////////////////////////////////////////////////////

    normal = normalize(normal);
    FragPos = (model * vertex_position).xyz;
    

    gl_Position = projection * view * vec4(FragPos, 1.0); // matrix multiplication is read from right to left
}