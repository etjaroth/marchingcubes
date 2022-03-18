#version 430 core

out vec4 FragColor;

in vec2 texturePos;
uniform sampler2D Texture;

void main() {
	FragColor = texture(Texture, texturePos);
	//FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}