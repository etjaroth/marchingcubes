#version 330 core
#define NUM_POINTLIGHTS 1
#define NUM_DIRECTIONALLIGHTS 1
#define NUM_SPOTLIGHTS 1
out vec4 FragColor;

//in vec4 Color;
//in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;

// Material
in vec3 Ambient;
in vec3 Diffuse;
in vec3 Specular;
in float Shininess;

//struct Material {
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    float shininess;
//};
//uniform Material material;

struct Light { // Arbitry light
    vec3 pos;
    vec3 dir;
    float angle; // cos value
    float brightness;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
//uniform Light light;


struct DirectionalLight {
    vec3 dir;
    float brightness;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight dirLights[NUM_DIRECTIONALLIGHTS];

struct PointLight {
    vec3 pos;
    float brightness;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[NUM_POINTLIGHTS];

struct Spotlight {
    vec3 pos;
    vec3 dir;
    float angle; // cos value
    float brightness;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Spotlight spotlights[NUM_SPOTLIGHTS];

uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

vec3 calculateLighting(vec3 direction, float strength, vec3 ambientLight, vec3 diffuseLight, vec3 specularLight);
vec3 calculateSpotlight(Spotlight light);
vec3 calculatePointLight(PointLight light);
vec3 calculateDirectionalLight(DirectionalLight light);

void main()
{
    vec3 lightDir;
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < NUM_POINTLIGHTS; i++)
  	    result += calculatePointLight(pointLights[i]);
    for(int i = 0; i < NUM_DIRECTIONALLIGHTS; i++)
  	    result += calculateDirectionalLight(dirLights[i]);
    for(int i = 0; i < NUM_SPOTLIGHTS; i++)
  	    result += calculateSpotlight(spotlights[i]);

    //FragColor = vec4(result, Color.w);
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(Diffuse, 1.0);
    //FragColor.w = 1.0;
}

vec3 calculateSpotlight(Spotlight light) {
    vec3 lightDir = normalize(vec3(light.pos) - FragPos);
    vec3 result;

    float theta = dot(lightDir, normalize(-vec3(light.dir))); // lightDir might need to be negative
    if (theta > light.angle) { // Same as directional light
        //lightDir = vec3(normalize(-light.dir));
        float epsilon = 0.2; // Blur size (as a cos value). Note that this subtracts from the size of the spotlight.
        float intensity = clamp((theta - light.angle) / epsilon, 0.0, 1.0);

        result = intensity * calculateLighting(lightDir, light.brightness, vec3(0.0), light.diffuse, light.specular);
        result += (light.brightness * light.ambient * Ambient);
    } else {
        result = (light.brightness * light.ambient * Ambient);
    }

    return result;
};
vec3 calculatePointLight(PointLight light) {
    vec3 result = vec3(1.0, 0.0, 0.0);

    vec3 lightDir = normalize(vec3(light.pos) - FragPos);
    float d = length(vec3(light.pos) - FragPos);
    float lightStrength = light.brightness/(light.constant + d*light.linear + d*light.quadratic*light.quadratic);
    result = calculateLighting(lightDir, lightStrength, light.ambient, light.diffuse, light.specular);

    return result;
};
vec3 calculateDirectionalLight(DirectionalLight light) {
    vec3 result = vec3(1.0, 0.0, 0.0);

    vec3 lightDir = vec3(normalize(-light.dir));
    float lightStrength = 1.0 * light.brightness;
    result = calculateLighting(lightDir, lightStrength, light.ambient, light.diffuse, light.specular);

    return result;
};

vec3 calculateLighting(vec3 direction, float strength, vec3 ambientLight, vec3 diffuseLight, vec3 specularLight) {
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.6);
    
    // Ambient Lighting
    vec3 ambient = ambientLight * Ambient;
  	
    // Diffuse Lighting
    vec3 norm = normalize(normal);
    //vec3 direction = normalize(vec3(light.pos) - FragPos);
    float diff = max(dot(norm, direction), 0.0);
    vec3 diffuse = diffuseLight * (diff * Diffuse);
    
    // Specular Lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = specularLight * (spec * Specular);
    
    // Combine
    ambient  *= strength; 
    diffuse  *= strength;
    specular *= strength;
    vec3 result = (ambient + diffuse + specular);// * Color.xyz;
    
    return result;
};