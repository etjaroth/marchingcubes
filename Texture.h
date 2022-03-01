#pragma once
#include "Config.h"
class AbstractTexture
{
	GLuint target;

public:
	AbstractTexture();
	virtual ~AbstractTexture();
	
	GLuint getTarget() const;

	virtual void setWrap(GLenum wrap) = 0;

	void setFilters(GLenum filter);
	void setMinFilter(GLenum filter);
	void setMaxFilter(GLenum filter);
};

class Texture2D : public AbstractTexture {
	glm::ivec2 dimensions;

public:
	explicit Texture2D(glm::ivec2 dimensions, GLenum dataType = GL_FLOAT);
	void setWrap(GLenum wrap) override;
	void setWrap(GLenum s, GLenum t);
};

class Texture3D : public AbstractTexture {
	glm::ivec3 dimensions;

public:
	explicit Texture3D(glm::ivec3 dimensions, GLenum dataType = GL_FLOAT);
	void setWrap(GLenum wrap) override;
	void setWrap(GLenum s, GLenum t, GLenum r );
};
