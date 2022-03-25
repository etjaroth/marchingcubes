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
    //vec4 viewDirection = viewMatInverse * get_camera_vector(vec2(textureSize(Texture, 0).xy), ivec2(gl_FragCoord.xy));
    //vec4 viewDirection = viewMatInverse * get_camera_vector(vec2(1024, 1024), ivec2(gl_FragCoord.xy));
	
    //const float theta = dot(normalize(viewDirection.xyz), calculateSunPos());
    //FragColor = vec4((theta > 0.9998 ? vec3(1.0) : texture(Texture, texturePos).xyz), 1.0); // w is depth
    FragColor = vec4(texture(Texture, texturePos).xyz, 1.0); // w is depth
    
    //float average = (FragColor.x + FragColor.y + FragColor.z) / 3.0;
    //if (isnan(FragColor.x)) { FragColor.xyz = vec3(0.0, 0.0, 1.0); }
    
    
    //if (isnan(FragColor.x)) { FragColor.xyz = texture(Texture, ivec2(-texturePos.x,  -texturePos.y)).xyz; }
    //if (length(FragColor.xyz) < 0.01) { FragColor.xyz = texture(Texture, ivec2(texturePos.x, 1.0 - texturePos.y)).xyz; }
}