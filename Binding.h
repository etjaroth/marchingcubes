//#pragma once
//#include "Config.h"
//#include "Texture.h"
//#include <GLFW/glfw3.h>
//
//class Binding {
//	GLuint prevBuffer;
//	GLuint currentBuffer;
//	GLenum target;
//
//	class TextureUnitStateStoreage {
//		std::vector<GLuint> textureUnits;
//
//	
//	public:
//		TextureUnitStateStoreage();
//		GLuint getTextureUnit(unsigned int i) const;
//		void setTextureUnit(unsigned int i, GLuint unit);
//		void activateUnit(unsigned int i);
//
//	};
//	//static TextureUnitStateStoreage textureUnitStateStoreage;
//
//
//protected:
//	//Binding(GLenum target, GLuint currentBuffer);
//
//public:
//	virtual ~Binding();
//};
//
//
//
//class Texture2DBinding : public Binding {
//public:
//	Texture2DBinding(Texture2D& texture, GLuint unit);
//};
//
//
//
//class Texture3DBinding : public Binding {
//public:
//	Texture3DBinding(Texture3D& texture, GLuint unit);
//};
//
//
