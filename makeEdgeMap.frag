#version 430

in vec2 TexCoord;

layout (location = 0) out vec2 EdgeMap;	// r : from normal, g : from z-value

layout (binding = 0) uniform sampler2D GBuffer;	// rgb : normal, a : z-value

vec3 expand(vec3 v)
{
	return vec3(2*v.x - 1, 2*v.y - 1, 2*v.z - 1);
}

void main()
{
	/* X가 현재 위치
		A B C
		D X E
		F G H
	*/
	ivec2 pix = ivec2(gl_FragCoord.xy);
	vec4 A = texelFetchOffset(GBuffer, pix, 0, ivec2(-1, 1));
	vec4 C = texelFetchOffset(GBuffer, pix, 0, ivec2(1, 1));
	vec4 F = texelFetchOffset(GBuffer, pix, 0, ivec2(-1, -1));
	vec4 H = texelFetchOffset(GBuffer, pix, 0, ivec2(1, -1));

	vec2 result;

	// edge from normal
	result.r = (dot(expand(A.rgb), expand(H.rgb)) + dot(expand(C.rgb), expand(F.rgb))) / 2.0;
	//result.r = (dot(A.rgb, H.rgb) + dot(C.rgb, F.rgb)) / 2.0;

	// edge from z-value
	result.g = pow(1 - abs(A.a - H.a) / 200.0, 2) * pow(1 - abs(C.a - F.a) / 200.0, 2); 

	EdgeMap = result;
}