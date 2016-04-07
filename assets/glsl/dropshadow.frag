#version 410

uniform sampler2D	uTex0;
uniform vec2		uSampleOffset;
uniform vec2		uShadowOffset;

//uniform int	uPass;

in vec2 vTexCoord0;

out vec4 oFragColor;

void main()
{ 
	vec2 offset = uSampleOffset;
	//offset *= 0.5;
	vec2 dropShadowOffset = uShadowOffset;

#if 0
	// debug overriding shadow offset
	vec2 texSize = vec2( textureSize( uTex0, 0 ) );
	dropShadowOffset.x = 10.0 / texSize.x;
	dropShadowOffset.y = 10.0 / texSize.y;
#endif

	vec4 sum = vec4( 0.0 );	
	sum += texture( uTex0, vTexCoord0 + -10.0 * offset + dropShadowOffset ) * 0.009167927656011385;
	sum += texture( uTex0, vTexCoord0 +  -9.0 * offset + dropShadowOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  -8.0 * offset + dropShadowOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +  -7.0 * offset + dropShadowOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +  -6.0 * offset + dropShadowOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +  -5.0 * offset + dropShadowOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +  -4.0 * offset + dropShadowOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +  -3.0 * offset + dropShadowOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +  -2.0 * offset + dropShadowOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +  -1.0 * offset + dropShadowOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   0.0 * offset + dropShadowOffset ) * 0.086826196862124602;
	sum += texture( uTex0, vTexCoord0 +   1.0 * offset + dropShadowOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   2.0 * offset + dropShadowOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +   3.0 * offset + dropShadowOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +   4.0 * offset + dropShadowOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +   5.0 * offset + dropShadowOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +   6.0 * offset + dropShadowOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +   7.0 * offset + dropShadowOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +   8.0 * offset + dropShadowOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +   9.0 * offset + dropShadowOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  10.0 * offset + dropShadowOffset ) * 0.009167927656011385;

	// Convert to grayscale using NTSC conversion weights
    float gray = dot( sum.rgb, vec3( 0.299, 0.587, 0.114 ) );

	oFragColor = vec4( gray, gray, gray, sum.a );
	
	// if( uPass == 0 ) {
	// 	oFragColor.r = 0.5;
	// 	oFragColor.a = 1;		
	// }
}
