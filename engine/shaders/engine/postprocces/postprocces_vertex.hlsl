#include "postprocces_vertex_pixel_s.hlsli"

struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

static const float4 g_fullscreenPosition[3] =
{
    float4(-1.0f,-1.0f, 0.0f, 1.0f),
	float4(-1.0f, 3.0f, 0.0f, 1.0f),
	float4( 3.0f,-1.0f, 0.0f, 1.0f),
};

static const float2 g_fullscreenUV[3] =
{
    float2(0.0f, 1.0f),
	float2(0.0f,-1.0f),
	float2(2.0f, 1.0f),
};

VS_OUTPUT 
main
(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = g_fullscreenPosition[input.vertexId];
    output.outUV = g_fullscreenUV[input.vertexId];
    return output;
}
