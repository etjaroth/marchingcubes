#pragma once
#include "Config.h"
#include "Texture.h"
#include <GLFW/glfw3.h>

class Binding {
	GLenum target;

	GLint prevBuffer;
	GLuint currentBuffer;
	GLenum target;

protected:
	Binding(GLenum target, GLuint currentBuffer);

public:
	virtual ~Binding();
};

class Texture2DBinding : public Binding {
public:
	Texture2DBinding(Texture2D& texture, GLuint unit);
};

class Texture3DBinding : public Binding {
public:
	Texture3DBinding(Texture3D& texture, GLuint unit);
};


