#include "Light.h"
Light::Light() {
    model = glm::mat4(1.0f);
    pos = glm::vec3(0.0f);
    scaleFactor = glm::vec3(1.0f);
    rotation = glm::vec4(0.0f);
    
    dir = glm::vec3(0.0f, -1.0f, 0.0f);
    angle = 0.7f; // slightly below cos(pi/4)
    blurAngle = 0.1f; // looks nice
    brightness = 1.0f;

    ambient = glm::vec3(1.0f);
    diffuse = glm::vec3(1.0f);
    specular = glm::vec3(1.0f);

    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;
};

void Light::setPos(glm::vec3 point) { pos = point; };
void Light::changePos(glm::vec3 vec) { pos += vec; };
void Light::scale(glm::vec3 factor) { scaleFactor = factor; };
void Light::rotate(glm::vec3 axis, float theta) { rotation = glm::vec4(axis, theta); };
void Light::setDir(glm::vec3 newdir) { dir = newdir; };
void Light::setAngle(float theta) { angle = cos(theta); };
void Light::setSpotlightBlur(float theta) { blurAngle = cos(theta); };
void Light::setBrightness(float b) { brightness = b; };

void Light::setLighting(glm::vec3 amb, glm::vec3 dif, glm::vec3 spec) {
    ambient = amb;
    diffuse = dif;
    specular = spec;
};
void Light::setPointLightFade(float con, float lin, float quad) {
    constant = con;
    linear = lin;
    quadratic = quad;
};

void Light::useAsPointLight(Shader shader, int n) {
    shader.use();

    std::string root = "pointLights["; // Find name of target struct
    char i = ('0' + n);
    root += i;
    root += "].";

    shader.setVec3((root + "pos"), pos);
    shader.setFloat((root + "brightness"), brightness);

    shader.setVec3((root + "ambient"), ambient);
    shader.setVec3((root + "diffuse"), diffuse);
    shader.setVec3((root + "specular"), specular);

    shader.setFloat((root + "constant"), constant);
    shader.setFloat((root + "linear"), linear);
    shader.setFloat((root + "quadratic"), quadratic);

    shader.dontuse();
};
void Light::useAsDirectionalLight(Shader shader, int n) {
    shader.use();

    std::string root = "dirLights["; // Find name of target struct
    char i = ('0' + n);
    root += i;
    root += "].";

    shader.setVec3((root + "dir"), dir);
    shader.setFloat((root + "brightness"), brightness);

    shader.setVec3((root + "ambient"), ambient);
    shader.setVec3((root + "diffuse"), diffuse);
    shader.setVec3((root + "specular"), specular);
};
void Light::useAsSpotlight(Shader shader, int n) {
    shader.use();

    std::string root = "spotlights["; // Find name of target struct
    char i = ('0' + n);
    root += i;
    root += "].";

    shader.setVec3((root + "pos"), pos);
    shader.setVec3((root + "dir"), dir);
    shader.setFloat((root + "angle"), angle);
    shader.setFloat((root + "brightness"), brightness);

    shader.setVec3((root + "ambient"), ambient);
    shader.setVec3((root + "diffuse"), diffuse);
    shader.setVec3((root + "specular"), specular);
};

glm::vec3 Light::getPos() { return pos; };
glm::mat4 Light::getModel() {
    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, rotation.w, glm::vec3(rotation));
    model = glm::scale(model, scaleFactor);
    return model;
};