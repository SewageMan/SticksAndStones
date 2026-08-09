shader_type canvas_item;

uniform sampler2D overlay_tex_1: repeat_enable, filter_nearest;
uniform sampler2D overlay_tex_2: repeat_enable, filter_nearest;
//uniform float scale = 0.0009765625; // calculated by 1/texture size e.g. 1/144
varying vec2 world_position;

void vertex(){
	// calculate the world position for use in the fragment shader
	world_position = (MODEL_MATRIX * vec4(VERTEX, 0.0, 1.0)).xy;
}

void fragment() {
	// only apply overlay_tex on the fully red parts of the original tiles
	float mix_amount_red = floor(COLOR.r);
	float mix_amount_blue = floor(COLOR.b);
	float scale = 0.0004365625;
	// sample the overlay_tex using worldPos
	vec4 overlay_color_forRed = texture(overlay_tex_1, world_position * scale);
	vec4 overlay_color_forBlue = texture(overlay_tex_2, world_position * scale);
	// combine original color and overlay color together
	COLOR = mix(COLOR, overlay_color_forRed, mix_amount_red);
	COLOR = mix(COLOR, overlay_color_forBlue, mix_amount_blue);
}