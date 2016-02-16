#version 410

uniform sampler2D	uTex0;
uniform vec2		uSampleOffset;
uniform float		uAttenuation = 1.0;

in vec2 vTexCoord0;

out vec4 oFragColor;

void main()
{ 
	vec4 sum = vec4( 0.0 );	
	sum += texture( uTex0, vTexCoord0 + -10.0 * uSampleOffset ) * 0.009167927656011385;
	sum += texture( uTex0, vTexCoord0 +  -9.0 * uSampleOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  -8.0 * uSampleOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +  -7.0 * uSampleOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +  -6.0 * uSampleOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +  -5.0 * uSampleOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +  -4.0 * uSampleOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +  -3.0 * uSampleOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +  -2.0 * uSampleOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +  -1.0 * uSampleOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   0.0 * uSampleOffset ) * 0.086826196862124602;
	sum += texture( uTex0, vTexCoord0 +   1.0 * uSampleOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   2.0 * uSampleOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +   3.0 * uSampleOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +   4.0 * uSampleOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +   5.0 * uSampleOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +   6.0 * uSampleOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +   7.0 * uSampleOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +   8.0 * uSampleOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +   9.0 * uSampleOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  10.0 * uSampleOffset ) * 0.009167927656011385;

	oFragColor = sum * uAttenuation;
}
