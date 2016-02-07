#version 410

uniform sampler2D	uTex0;

in vec2 vTexCoord0;

out vec4 oFragColor;

void main()
{ 
	vec3 col = texture( uTex0, vTexCoord0 ).rgb;

	oFragColor = vec4( col, 1.0 );
}
