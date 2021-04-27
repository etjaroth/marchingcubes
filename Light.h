#pragma once
#include "Config.h"
#include "Shader.h"
class Light
{	
    std::string lightType;
    glm::mat4 model;

    glm::vec3 pos;
    glm::vec3 scaleFactor;
    glm::vec4 rotation;

    glm::vec3 dir;
    float angle; // cos value
    float blurAngle;
    float brightness;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
public:
	void setPos(glm::vec3 point);
	void changePos(glm::vec3 vec);
    void scale(glm::vec3 factor);
    void rotate(glm::vec3 axis, float theta);
	void setDir(glm::vec3 newdir);
    void setAngle(float theta);
    void setSpotlightBlur(float theta);
    void setBrightness(float b);

    void setLighting(glm::vec3 amb, glm::vec3 dif, glm::vec3 spec);
    void setPointLightFade(float con, float lin, float quad);

    void useAsPointLight(Shader shader, int n);
    void useAsDirectionalLight(Shader shader, int n);
    void useAsSpotlight(Shader shader, int n);

    glm::vec3 getPos();
    glm::mat4 getModel();


	Light();
};