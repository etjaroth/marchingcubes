#version 430 core

in vec2 texturePos;
out vec4 FragColor;


uniform sampler2D Texture;
uniform float time;
uniform mat4 viewMatInverse;

vec3 calculateSunPos() {
	return vec3(0.0, sin(-time), cos(-time));
}

vec4 get_camera_vector(vec2 resolution, vec2 coord) {
	vec2 uv = coord / resolution - vec2(0.5);
    uv.x *= resolution.x / resolution.y;
    return vec4(uv.x, uv.y, -1.0, 1.0);
}

void main() {
    FragColor = vec4(texture(Texture, texturePos).xyz, 1.0); // w is depth

    if (length(FragColor.xyz) < 0.0001) { FragColor.xyz = texture(Texture, ivec2(texturePos.x, 0.5 - texturePos.y)).xyz; }
}