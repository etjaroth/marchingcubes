#pragma once
#include "Config.h"
class AbstractTexture
{
	GLuint target;
	bool minmapSet = false;

public:
	GLenum textureClass = GL_INVALID_ENUM;
	GLenum getTarget();

	AbstractTexture(const AbstractTexture& other) = default;
	AbstractTexture& operator=(const AbstractTexture& other) = default;

public:
	AbstractTexture();
	virtual ~AbstractTexture();
	AbstractTexture(AbstractTexture&& other) noexcept;
	AbstractTexture& operator=(AbstractTexture&& other) noexcept;
	
	GLuint getTarget() const;

	virtual void setWrap(GLenum wrap) = 0;

	void setFilters(GLenum filter);
	void setMinFilter(GLenum filter);
	void setMaxFilter(GLenum filter);

	void use(GLuint i = 0);
	void dontuse(GLuint i = 0);
};

class Texture2D : public AbstractTexture {
	glm::ivec2 dimensions;

public:
	explicit Texture2D(glm::ivec2 dimensions, GLenum dataType = GL_FLOAT);
	Texture2D(glm::vec4 * data, glm::ivec2 dimensions);
	void setWrap(GLenum wrap) override;
	void setWrap(GLenum s, GLenum t);

	glm::ivec2 getSize() const;
};

class Texture3D : public AbstractTexture {
	glm::ivec3 dimensions;

public:
	explicit Texture3D(glm::ivec3 dimensions, GLenum dataType = GL_FLOAT);
	void setWrap(GLenum wrap) override;
	void setWrap(GLenum s, GLenum t, GLenum r);

	glm::ivec3 getSize() const;
};
