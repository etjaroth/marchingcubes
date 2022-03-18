#include "Texture.h"
#include <algorithm>

GLenum AbstractTexture::getTarget() {
	return target;
}

AbstractTexture::AbstractTexture() {
	glGenTextures(1, &target);
}

AbstractTexture::~AbstractTexture() {
	glDeleteTextures(1, &target);
	target = 0;
}

AbstractTexture::AbstractTexture(AbstractTexture&& other) noexcept
	: target{ std::exchange(other.target, 0) },
	minmapSet{ other.minmapSet },
	textureClass{ other.textureClass }
{}

AbstractTexture& AbstractTexture::operator=(AbstractTexture&& other) noexcept {
	std::swap<GLuint>(target, other.target);
	std::swap(minmapSet, other.minmapSet);
	std::swap(textureClass, other.textureClass);
	return *this;
}

GLuint AbstractTexture::getTarget() const {
	return target;
}

void AbstractTexture::setFilters(GLenum filter) {
	setMinFilter(filter);
	setMaxFilter(filter);
}

void AbstractTexture::setMinFilter(GLenum filter) {
	glTexParameteri(textureClass, GL_TEXTURE_MIN_FILTER, filter);
}

void AbstractTexture::setMaxFilter(GLenum filter) {
	glTexParameteri(textureClass, GL_TEXTURE_MAG_FILTER, filter);
}

void AbstractTexture::use(GLuint i) {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(textureClass, target);
	glBindImageTexture(i, target, 0, ((textureClass == GL_TEXTURE_3D || minmapSet) ? GL_TRUE : GL_FALSE), 0, GL_READ_WRITE, GL_RGBA32F);
}

void AbstractTexture::dontuse(GLuint i) {
	glBindTexture(textureClass, 0);
}



Texture2D::Texture2D(glm::ivec2 dimensions, GLenum dataType)
	: AbstractTexture{},
	dimensions{ dimensions } {
	textureClass = GL_TEXTURE_2D;
	glBindTexture(textureClass, getTarget());
	glTexImage2D(textureClass, 0, GL_RGBA32F, dimensions.x, dimensions.y, 0, GL_RGBA, dataType, NULL);

}

Texture2D::Texture2D(glm::vec4* data, glm::ivec2 dimensions)
	: AbstractTexture{},
	dimensions{ dimensions } {
	textureClass = GL_TEXTURE_2D;
	glBindTexture(textureClass, getTarget());
	//glTexImage2D(textureClass, 0, GL_RGBA32F, dimensions.x, dimensions.y, 0, GL_RGBA, GL_FLOAT, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dimensions.x, dimensions.y, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture2D::setWrap(GLenum wrap) {
	setWrap(wrap, wrap);
}

void Texture2D::setWrap(GLenum s, GLenum t) {
	glTexParameteri(textureClass, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(textureClass, GL_TEXTURE_WRAP_T, t);
}

glm::ivec2 Texture2D::getSize() const {
	return dimensions;
}



Texture3D::Texture3D(glm::ivec3 dimensions, GLenum dataType)
	: AbstractTexture{},
	dimensions{ dimensions } {
	textureClass = GL_TEXTURE_3D;
	glBindTexture(textureClass, getTarget());
	glTexImage3D(textureClass, 0, GL_RGBA32F, dimensions.x, dimensions.y, dimensions.z, 0, GL_RGBA, dataType, NULL);
}

void Texture3D::setWrap(GLenum wrap) {
	setWrap(wrap, wrap, wrap);
}

void Texture3D::setWrap(GLenum s, GLenum t, GLenum r) {
	glTexParameteri(textureClass, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(textureClass, GL_TEXTURE_WRAP_T, t);
	glTexParameteri(textureClass, GL_TEXTURE_WRAP_R, r);
}

glm::ivec3 Texture3D::getSize() const {
	return dimensions;
}