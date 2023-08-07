struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 screenUV : SCREENUV;
};

static const float4 fullScreenOutPosition[3] =
{
    float4(-1.0f, -1.0f, 0.0f, 1.0f),
	float4(-1.0f, 3.0f, 0.0f, 1.0f),
	float4(3.0f, -1.0f, 0.0f, 1.0f)
};

static const float2 fullScreenUV[3] =
{
    float2(0.0f, 1.0f),
	float2(0.0f, -1.0f),
	float2(2.0f, 1.0f)
};

VS_OUTPUT main(uint vertexId : SV_VertexID)
{
    VS_OUTPUT output;
    output.position = fullScreenOutPosition[vertexId];
    output.screenUV = fullScreenUV[vertexId];
    return output;
}
