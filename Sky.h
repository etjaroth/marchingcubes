#pragma once
#include "Config.h"
#include "FPSCamera.h"
#include "RenderBox.h"
#include "ComputeShader.h"

class Sky
{
	RenderBox box;
	Texture2D skyTexture;
	ComputeShader skyGenerator;
	glm::uvec2 resolution;

	GLuint tex = 0;

public:
	Sky(glm::uvec2 resolution=glm::uvec2(1024), std::string skyShaderName="SkyGenerator.comp");

	void generateSky(FPSCamera& camera, double time);
	void render(FPSCamera& camera, double time);
};

