shader_type canvas_item;
uniform float max_col_b = 0.22;
float line(float col_b){
	return col_b/max_col_b;
}
void fragment(){
	vec4 col = texture(TEXTURE,UV).rgba;
	if (col.r <= 5.0 && col.g <= 5.0 && col.b <= max_col_b) {
		col.a = line(col.b);
	}
	COLOR= col;
}