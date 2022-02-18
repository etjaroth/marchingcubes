#include "Binding.h"
#include <GLFW/glfw3.h>

Binding::Binding(GLenum target, GLuint currentBuffer)
	: target{ target },
	currentBuffer{ currentBuffer } {

}

Binding::~Binding() {
	glBindBuffer(target, prevBuffer);
}

Texture2DBinding::Binding(Texture2D& texture, GLuint unit)
	: Binding{ GL_TEXTURE_2D, texture.getTarget() } {}

Texture3DBinding::Binding(Texture3D& texture, GLint unit)
	: Binding{ GL_TEXTURE_3D, texture.getTarget() } {}