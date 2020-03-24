#version 430

layout( triangles ) in;
layout( triangle_strip, max_vertices = 7 ) out;

in vec3 Position[];
in vec3 Normal[];
in vec2 TexCoord[];

out vec3 GPosition;
out vec3 GNormal;
out vec2 GTexCoord;

flat out int GIsEdge;

uniform mat4 ProjectionMatrix;
uniform float EdgeWidth;
uniform float PctExtend;

vec3 calculateCenter(const float nDotV[3], const int positiveIndex,  const int negativeIndex)
{
	float diff = nDotV[positiveIndex] - nDotV[negativeIndex];
	vec3 pToN = Position[negativeIndex] - Position[positiveIndex];
	vec3 center = Position[positiveIndex] + pToN * nDotV[positiveIndex] / diff;
	return center;
}

void emitEdgeQuad()
{
	vec3 v[3];
	for(int i = 0; i < 3; i++)
	{
		v[i] = normalize(-Position[i].xyz);
	}

	float nDotV[3];
	bool isNegative[3];
	for(int i = 0; i < 3; i++)
	{
		nDotV[i] = dot(v[i], Normal[i]);
		if(nDotV[i] < 0)
		{
			isNegative[i] = true;
		}
		else
		{
			isNegative[i] = false;
		}
	}

	if(	(isNegative[0] == true  && isNegative[1] == true  && isNegative[2] == true) ||
		(isNegative[0] == false && isNegative[1] == false && isNegative[2] == false) )
	{
		return;
	}

	vec3 edgePoint1;
	vec3 edgePoint2;
	if(isNegative[0] == false && isNegative[1] == true && isNegative[2] == true)
	{
		edgePoint1 = calculateCenter(nDotV, 0, 1);
		edgePoint2 = calculateCenter(nDotV, 0, 2);
	}
	else if(isNegative[0] == true && isNegative[1] == false && isNegative[2] == true)
	{
		edgePoint1 = calculateCenter(nDotV, 1, 0);
		edgePoint2 = calculateCenter(nDotV, 1, 2);
	}
	else if(isNegative[0] == true && isNegative[1] == true && isNegative[2] == false)
	{
		edgePoint1 = calculateCenter(nDotV, 2, 0);
		edgePoint2 = calculateCenter(nDotV, 2, 1);
	}
	else if(isNegative[0] == true && isNegative[1] == false && isNegative[2] == false)
	{
		edgePoint1 = calculateCenter(nDotV, 1, 0);
		edgePoint2 = calculateCenter(nDotV, 2, 0);
	}
	else if(isNegative[0] == false && isNegative[1] == true && isNegative[2] == false)
	{
		edgePoint1 = calculateCenter(nDotV, 0, 1);
		edgePoint2 = calculateCenter(nDotV, 2, 1);
	}
	else if(isNegative[0] == false && isNegative[1] == false && isNegative[2] == true)
	{
		edgePoint1 = calculateCenter(nDotV, 0, 2);
		edgePoint2 = calculateCenter(nDotV, 1, 2);
	}

	vec2 ext = PctExtend * (edgePoint2.xy - edgePoint1.xy);
	vec2 vec = normalize(edgePoint2.xy - edgePoint1.xy);
	vec2 n = vec2(-vec.y, vec.x) * EdgeWidth;

	GIsEdge = 1;   // This is part of the sil. edge

	gl_Position = ProjectionMatrix * vec4( edgePoint1.xy + n - ext, edgePoint1.z, 1.0 ); 
	EmitVertex();
	
	gl_Position = ProjectionMatrix * vec4( edgePoint1.xy - n - ext, edgePoint1.z, 1.0 ); 
	EmitVertex();
	
	gl_Position = ProjectionMatrix * vec4( edgePoint2.xy + n + ext, edgePoint2.z, 1.0 ); 
	EmitVertex();
	
	gl_Position = ProjectionMatrix * vec4( edgePoint2.xy - n + ext, edgePoint2.z, 1.0 ); 
	EmitVertex();

	EndPrimitive();
}

void emitOriginalTriangle()
{
	GIsEdge = 0;   // This triangle is not part of an edge.
	
    GNormal = Normal[0];
    GPosition = Position[0];
	GTexCoord = TexCoord[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    GNormal = Normal[1];
    GPosition = Position[1];
	GTexCoord = TexCoord[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    GNormal = Normal[2];
    GPosition = Position[2];
	GTexCoord = TexCoord[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

	EndPrimitive();
}

void main()
{
	emitEdgeQuad();
	emitOriginalTriangle();
}