//#include "Binding.h"
//#include <GLFW/glfw3.h>
//
////Binding::TextureUnitStateStoreage Binding::textureUnitStateStoreage{};
//
////Binding::Binding(GLenum target, GLuint currentBuffer)
////	: target{ target },
////	currentBuffer{ currentBuffer },
////	prevBuffer{textureUnitStateStoreage.getTextureUnit(currentBuffer)}
////{}
//
//Binding::TextureUnitStateStoreage::TextureUnitStateStoreage() {
//	int numTextures = 0;
//	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTextures);
//	textureUnits = std::vector<GLuint>(static_cast<size_t>(numTextures), static_cast<GLuint>(0));
//}
//
//GLuint Binding::TextureUnitStateStoreage::getTextureUnit(unsigned int i) const {
//	return textureUnits[i];
//}
//
//void Binding::TextureUnitStateStoreage::setTextureUnit(unsigned int i, GLuint unit) {
//	textureUnits[i] = unit;
//}
//
//void Binding::TextureUnitStateStoreage::activateUnit(unsigned int i) {
//	glActiveTexture(GL_TEXTURE0 + i);
//}
//
//Binding::~Binding() {
//	glBindBuffer(target, prevBuffer);
//	glActiveTexture(GL_TEXTURE0);
//}
//
//Texture2DBinding::Texture2DBinding(Texture2D& texture, GLuint unit)
//	: Binding{ GL_TEXTURE_2D, texture.getTarget() } {}
//
//Texture3DBinding::Texture3DBinding(Texture3D& texture, GLuint unit)
//	: Binding{ GL_TEXTURE_3D, texture.getTarget() } {}