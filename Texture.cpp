#include "Texture.h"


GLuint AbstractTexture::getTarget() const {
	return target;
}


Texture2D::Texture2D(glm::ivec2 dimensions)
	: dimensions{ dimensions } {

}


Texture3D::Texture3D(glm::ivec3 dimensions)
	: dimensions{ dimensions } {

}
