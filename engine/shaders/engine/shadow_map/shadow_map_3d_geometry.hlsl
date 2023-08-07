static const int NUM_TRIANGLES = 6;

cbuffer ShadowMap3DConstantBuffer : register(b10)
{
    float4x4 g_worldToLightViewsPerFace[6];
    float4x4 g_lightViewToClip;
    float g_texelWorldSize;
    float3 _pad1;
    float3 g_lightPosition;
    float _pad2;
};

struct GS_OUTPUT
{
    uint outTargetIndex : SV_RenderTargetArrayIndex;
    float4 outPosition : SV_Position;
};

struct VS_OUTPUT
{
    float3 vWorldPosition : WORLDPOSITION;
    float3 vWorldNormal : NORMAL;
};

[maxvertexcount(3 * NUM_TRIANGLES)]
void 
main
(
    triangle VS_OUTPUT input[3], 
    inout TriangleStream<GS_OUTPUT> output
)
{
    for (int face = 0; face < 6; ++face)
    {
        GS_OUTPUT outputVertex;
        outputVertex.outTargetIndex = face;
        
		[unroll]
        for (int i = 0; i < 3; ++i)
        {
            const float3 lighDir = normalize(g_lightPosition - input[i].vWorldPosition);
            const float3 offset = (g_texelWorldSize / sqrt(2.f)) * (input[i].vWorldNormal - 0.9f * lighDir * dot(input[i].vWorldNormal, lighDir));
            const float4 worldPosition = float4(input[i].vWorldPosition - offset, 1.f);
            const float4 viewPosition = mul(g_worldToLightViewsPerFace[face], worldPosition);
            const float4 clipPosition = mul(g_lightViewToClip, viewPosition);
            outputVertex.outPosition = clipPosition;
            output.Append(outputVertex);
        }
        output.RestartStrip();
    }
}