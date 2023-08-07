cbuffer FullscrenTriangleNearPlaneConstantBuffer : register(b3)
{
	float4 nearLB_LT_RB[3];
}

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outFustumCornerVec : DIRECTION;
};

static const float4 fullScreenOutPosition[3] = { 
	float4(-1.0f, -1.0f, 0.0f, 1.0f),
	float4(-1.0f,  3.0f, 0.0f, 1.0f),
	float4( 3.0f, -1.0f, 0.0f, 1.0f) 
};

VS_OUTPUT main(uint vertexId : SV_VertexID)
{
	VS_OUTPUT output;
	output.outFustumCornerVec = nearLB_LT_RB[vertexId];
	output.outPosition = fullScreenOutPosition[vertexId];
	return output;
}
