#version 430

in vec3 Position;	// eye space
in vec3 Normal;		// eye space

layout (location = 0) out vec4 GBuffer;
layout (location = 1) out vec3 ColorLayer;

layout (binding = 0) uniform sampler2D PreviousGBuffer;

uniform int Depth;

vec3 GetColor()
{
	return vec3(0.5, 0.75, 1.00); 
}

void main()
{
	if(Depth == 0)
	{
		GBuffer = vec4(normalize(Normal), -Position.z);
		ColorLayer = GetColor();
	}
	else
	{
		ivec2 pix = ivec2( gl_FragCoord.xy );
		vec4 gbuffer = texelFetchOffset(PreviousGBuffer, pix, 0, ivec2(0, 0));

		if(gbuffer.a < 0.01)
		{
			discard;
		}
		else if(-Position.z <= gbuffer.a)
		{
			discard;
		}
		else
		{
			GBuffer = vec4(normalize(Normal), -Position.z);
			ColorLayer = GetColor();
		}
	}
}