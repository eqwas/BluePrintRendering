#version 430

in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D GBuffer;
layout (binding = 1) uniform sampler2D ColorLayer;
layout (binding = 2) uniform sampler2D EdgeMap;

void main()
{
	vec2 edgeMap = texture(EdgeMap, TexCoord).rg;
	float edgeIntensity = min(edgeMap.r, edgeMap.g);

	//gl_FragDepth = texture(GBuffer, TexCoord).a / 100.0;

	if(edgeIntensity < 0.99)
	{
		FragColor = vec4(0.0, 0.0, 0.0, edgeIntensity);
	}
	else
	{
		vec3 color = texture(ColorLayer, TexCoord).rgb;
		if(color.r < 0.01)
		{
			discard;
		}
		else
		{
			FragColor = vec4(color, 0.3);
		}
	}
}