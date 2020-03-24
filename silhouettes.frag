#version 430

in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoord;

flat in int GIsEdge;

layout (binding = 0) uniform sampler2D RenderTex;
uniform bool SilhouetteMode;
uniform int Pass;

layout (location = 0) out vec4 FragColor;

void Pass1()
{
	if(SilhouetteMode == true)
	{
		if(GIsEdge == 0)
		{
			discard;
		}
		else
		{
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
	else
	{
		if(GIsEdge == 1)
		{
			discard;
		}

		vec3 v = normalize(-GPosition.xyz);
		float nDotV = max(dot(normalize(GNormal), v), 0.0);
		FragColor = vec4(nDotV, nDotV, nDotV, 1.0);
	}
}

void Pass2()
{
	FragColor = texture(RenderTex, GTexCoord);
}

void main() 
{
	if(Pass == 1) 
		Pass1();
	else 
		Pass2();
}