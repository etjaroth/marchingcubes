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

vec3 materials_ambient[] = vec3[](
                      vec3(0.514,0.843,0.933), // water

                      vec3(0.0, 0.75, 0.0), // grass
                      vec3(0.478, 0.286, 0.043), // dirt
                      vec3(0.305, 0.403, 0.494), // stone

                      vec3(0.878, 0.843, 0.693), // top sand
                      vec3(0.725, 0.654, 0.254), // sand
                      vec3(0.462, 0.403, 0.074), // packed sand

                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(0.576, 0.768, 0.784), // permafrost
                      vec3(0.713, 0.964, 0.964)); // ice

vec3 materials_diffuse[] = vec3[](
                      vec3(0.514,0.843,0.933), // water

                      vec3(0.0, 0.75, 0.0), // grass
                      vec3(0.478, 0.286, 0.043), // dirt
                      vec3(0.305, 0.403, 0.494), // stone

                      vec3(0.878, 0.843, 0.693), // top sand
                      vec3(0.725, 0.654, 0.254), // sand
                      vec3(0.462, 0.403, 0.074), // packed sand

                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(0.576, 0.768, 0.784), // permafrost
                      vec3(0.713, 0.964, 0.964)); // ice

vec3 materials_specular[] = vec3[](
                      vec3(0.514,0.843,0.933), // water

                      vec3(0.0, 0.75, 0.0), // grass
                      vec3(0.478, 0.286, 0.043), // dirt
                      vec3(0.305, 0.403, 0.494), // stone

                      vec3(0.878, 0.843, 0.693), // top sand
                      vec3(0.725, 0.654, 0.254), // sand
                      vec3(0.462, 0.403, 0.074), // packed sand

                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(0.576, 0.768, 0.784), // permafrost
                      vec3(0.713, 0.964, 0.964)); // ice

float materials_shininess[] = float[](
                      16.0, // water

                      4.0, // grass
                      4.0, // dirt
                      4.0, // stone

                      4.0, // top sand
                      4.0, // sand
                      4.0, // packed sand

                      4.0, // snow
                      4.0, // permafrost
                      4.0); // ice

// Vectors are ordered so that their cross product has a positive y
const vec3 wave_triangle_verticies[6][2] = {
    {vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0)}, 
    {vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 1.0)}, 
    {vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, -1.0)}, 

    {vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, -1.0)}, 
    {vec3(-1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)}, 
    {vec3(0.0, 0.0, -1.0), vec3(-1.0, 0.0, 0.0)}};

const vec3 index_colors[6] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(0,1,1), vec3(1,0,1), vec3(1,1,0)};

const float wavelength = 2.0;
const float waveamp = 0.5;
const float wavespeed = 1.0;

float wave(vec2 v) {
    return waveamp * 0.5 * (sin(v.x / wavelength + wavespeed * wavetime) + cos(v.y / wavelength + wavespeed * wavetime));
}

vec2 wave_derivitive(vec2 v) {
    // wave(v) = f(x, y) = waveamp * 0.5 * (sin(v.x / wavelength + wavespeed * wavetime) + cos(v.y / wavelength + wavespeed * wavetime))
    // df/dx = (0.5 * waveamp * cos((x / wavelength) + wavetime * wavespeed)) / wavelength
    // df/dy = - (0.5 * waveamp * cos((y / wavelength) + wavetime * wavespeed)) / wavelength

    const float c = wavetime * wavespeed;
    return 0.5 * waveamp * vec2(cos((v.x / wavelength) + c), -cos((v.y + wavelength) + c)) / wavelength;
}

vec3 find_wave_normal(vec2 pos) {
    
    vec3 normal;


    return normal;
}

vec3 apply_wave(vec3 v) {
    v.y += wave(v.xz);
    return v;
}

const float isolevel = 0.0;
vec3 vec3interpolate(vec3 v1, float f1, vec3 v2, float f2) {
	// Code adapted from paulbourke.net/geometry/polygonise/
	if (abs(isolevel - f1) < 0.00001 || abs(f1 - f2) < 0.00001)
		return v1;
	if (abs(isolevel - f2) < 0.00001)
		return v2;

	return v1 + vec3((isolevel - f1) / (f2 - f1)) * (v2 - v1);
}

void main()
{
    vec4 vertex_position = aPos;
    int material = int(aMaterial.x);

    normal = aNormal.xyz;

    //////////////////////////////////////////////////////////////////////////

    // Waves
    if (material == 0) {
        Shininess = 32.0;
        
        
        vertex_position.y += wave(vertex_position.xz);
        //vec3 color = normal.xyz;

        // Recalculate normal
        const vec3 pos = vertex_position.xyz;
        const vec2 der = wave_derivitive(vertex_position.xz);
        vec3 tangent_x = vec3(1.0, der.x, 0.0);
        vec3 tangent_z = vec3(0.0, der.y, 1.0);
        normal = -normalize(cross(tangent_x, tangent_z));
    }
    
    //////////////////////////////////////////////////////////////////////////

    Ambient = 0.5 * materials_ambient[material];
    Diffuse = materials_diffuse[material];
    Specular = vec3(0.0);
    Shininess = materials_shininess[material];

    //////////////////////////////////////////////////////////////////////////

    FragPos = (model * vertex_position).xyz;
    normal = normalize(normal);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}