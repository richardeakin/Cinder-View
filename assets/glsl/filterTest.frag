#version 410

uniform sampler2D	uTex0;
uniform float		ciElapsedSeconds;

in vec2 vTexCoord0;

out vec4 oFragColor;

// For all settings: 1.0 = 100% 0.5=50% 1.5 = 150%
// https://mouaif.wordpress.com/2009/01/05/photoshop-math-with-glsl-shaders/
vec3 ContrastSaturationBrightness( vec3 color, float brt, float sat, float con )
{
	// Increase or decrease theese values to adjust r, g and b color channels seperately
	const float AvgLumR = 0.5;
	const float AvgLumG = 0.5;
	const float AvgLumB = 0.5;
	
	const vec3 LumCoeff = vec3( 0.2125, 0.7154, 0.0721 );
	
	vec3 AvgLumin = vec3( AvgLumR, AvgLumG, AvgLumB );
	vec3 brtColor = color * brt;
	vec3 intensity = vec3( dot( brtColor, LumCoeff ) );
	vec3 satColor = mix( intensity, brtColor, sat );
	vec3 conColor = mix( AvgLumin, satColor, con );
	return conColor;
}

void main()
{ 
	vec4 col = texture( uTex0, vTexCoord0 );

    float t = ciElapsedSeconds;
    float brightness = sin( t + 9.19 ) * 0.5 + 1.5;
    float saturation = sin( t + 5.33 ) * 0.5 + 1.5;
    float contrast = sin( t + 13.11 ) * 0.5 + 1.5;
	col.rgb = ContrastSaturationBrightness( col.rgb, brightness, saturation, contrast );

	oFragColor = col;
}
