#include "Texture.h"

AbstractTexture::AbstractTexture() {
	glGenTextures(1, &target);
}

AbstractTexture::~AbstractTexture() {
	glDeleteTextures(1, &target);
}

GLuint AbstractTexture::getTarget() const {
	return target;
}

void AbstractTexture::setFilters(GLenum filter) {
	setMinFilter(filter);
	setMaxFilter(filter);
}

void AbstractTexture::setMinFilter(GLenum filter) {
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
}

void AbstractTexture::setMaxFilter(GLenum filter) {
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
}




Texture2D::Texture2D(glm::ivec2 dimensions, GLenum dataType)
	: AbstractTexture{},
	dimensions{ dimensions } {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dimensions.x, dimensions.y, 0, GL_RGBA, dataType, NULL);
}

void Texture2D::setWrap(GLenum wrap) {
	setWrap(wrap, wrap);
}

void Texture2D::setWrap(GLenum s, GLenum t) {
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, t);
}



Texture3D::Texture3D(glm::ivec3 dimensions, GLenum dataType)
	: AbstractTexture{},
	dimensions{ dimensions } {
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dimensions.x, dimensions.y, dimensions.z, 0, GL_RGBA, dataType, NULL);
}

void Texture3D::setWrap(GLenum wrap) {
	setWrap(wrap, wrap, wrap);
}

void Texture3D::setWrap(GLenum s, GLenum t, GLenum r) {
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, t);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, r);
}