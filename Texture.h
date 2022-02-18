#pragma once
#include "Config.h"
class AbstractTexture
{
	GLuint target;

public:
	GLuint getTarget() const;
};

class Texture2D : public AbstractTexture {
	glm::ivec2 dimensions;
public:
	explicit Texture2D(glm::ivec2 dimensions);
};

class Texture3D : public AbstractTexture {
	glm::ivec3 dimensions;
public:
	explicit Texture3D(glm::ivec3 dimensions);
};

