#version 430 core
#define PI 3.14159265358979323846

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aMaterial; // x stores material index. x, z stores texture coords, w stores light level

// Coordinate Matricies
uniform mat4 model;
//uniform mat4 model_inverse;
uniform mat4 view;
uniform mat4 projection;

// Texture
uniform mat4 transform;

// Brightness
uniform float brightness;

// Output
flat out vec3 normal;
//out vec3 normal;
out vec3 FragPos;
out float lightingConstant;

// Material
out vec3 Ambient;
out vec3 Diffuse;
out vec3 Specular;
out float Shininess;

// Waves
uniform float wavetime;

vec3 materials_ambient[] = vec3[](
                      vec3(0.15, 0.15, 0.3), // water

                      vec3(0.1, 0.3, 0.15), // grass
                      vec3(0.28, 0.21, 0.02), // dirt
                      vec3(0.3), // stone

                      vec3(0.3, 0.3, 0.0), // top sand
                      vec3(0.0, 0.0, 0.0), // sand
                      vec3(0.0, 0.0, 0.0), // packed sand

                      vec3(0.0, 0.0, 0.0), // snow
                      vec3(0.0, 0.0, 0.0), // permafrost
                      vec3(0.0, 0.0, 0.0)); // ice

vec3 materials_diffuse[] = vec3[](
                      vec3(0.35, 0.8, 0.85), // water

                      vec3(0.3, 0.8, 0.0), // grass
                      vec3(0.5, 0.3, 0.1), // dirt
                      vec3(0.6, 0.6, 0.6), // stone

                      vec3(0.8, 0.8, 0.5), // top sand
                      vec3(0.0, 0.0, 0.0), // sand
                      vec3(0.0, 0.0, 0.0), // packed sand

                      vec3(0.0, 0.0, 0.0), // snow
                      vec3(0.0, 0.0, 0.0), // permafrost
                      vec3(0.0, 0.0, 0.0)); // ice

vec3 materials_specular[] = vec3[](
                      vec3(0.7, 0.8, 0.7), // water

                      vec3(0.2, 0.5, 0.2), // grass
                      vec3(0.28, 0.21, 0.02), // dirt
                      vec3(0.1, 0.1, 0.1), // stone

                      vec3(0.9, 0.9, 0.7), // top sand
                      vec3(1.0, 1.0, 1.0), // sand
                      vec3(1.0, 1.0, 1.0), // packed sand
                                 
                      vec3(1.0, 1.0, 1.0), // snow
                      vec3(1.0, 1.0, 1.0), // permafrost
                      vec3(1.0, 1.0, 1.0)); // ice

float materials_shininess[] = float[](
                      16.0, // water
                      8.0, // grass
                      8.0, // dirt
                      16.0, // stone
                      32.0, // top sand
                      32.0, // sand
                      32.0, // packed sand
                      32.0, // snow
                      32.0, // permafrost
                      32.0); // ice

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
    return waveamp * 0.5 * (sin(v.x / wavelength + wavespeed * wavetime) + cos(2.0 * v.y / wavelength + wavespeed * wavetime));
}

vec3 apply_wave(vec3 v) {
    v.y += wave(v.xz);
    return v;
}

const float isolevel = 0.0;
vec3 vec3interpolate(vec3 v1, float f1, vec3 v2, float f2) {
    // Code adapted from paulbourke.net/geometry/polygonise/
	if (abs(isolevel - f1) < 0.00001 || abs(f1 - f2) < 0.00001) {
		return v1;
	}
	else if (abs(isolevel - f2) < 0.00001) {
		return v2;
	}
	else if (abs(f1-f2) < 0.00001) {
      return v1;
	}

	float mu = (isolevel - f1)/(f2-f1);
	mu = max(0.0, min(1.0, mu));

	return v1 + mu * (v2 - v1);
}

void main()
{
    vec4 vertex_position = aPos;
    const int material = int(aMaterial.x);

    normal = aNormal.xyz;

    //////////////////////////////////////////////////////////////////////////

    // Waves
    if (material == 0) {
        vertex_position.y += wave(vertex_position.xz);

        // Recalculate normal
        const vec3 P0 = vertex_position.xyz;
        const vec3 P1 = P0 + vec3(1.0, 0.0, 0.0);
        const vec3 P2 = P0 + vec3(0.0, 0.0, 1.0);
        
        const vec3 wave0 = apply_wave(P0);
        const vec3 wave1 = apply_wave(P1);
        const vec3 wave2 = apply_wave(P2);

        const vec3 cross1 = wave1 - P0;
        const vec3 cross2 = wave2 - P0;
        
        normal = -normalize(cross(cross1, cross2));
    }
    
    //////////////////////////////////////////////////////////////////////////

    lightingConstant = brightness * aMaterial.w;
    Ambient = materials_ambient[material];
    Diffuse = materials_diffuse[material];
    Specular = materials_specular[material];
    Shininess = materials_shininess[material];

    //////////////////////////////////////////////////////////////////////////

    FragPos = (model * vertex_position).xyz;
    normal = normalize(normal);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}