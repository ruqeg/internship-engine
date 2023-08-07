#include "vertex_output_struct.hlsli"
#include "geometry_output_struct.hlsli"
#include "../camera_constant_buffer.hlsli"

static const int NUM_TRIANGLES = 1;

[maxvertexcount(3 * NUM_TRIANGLES)]
void 
main
(
    triangle VS_OUTPUT input[3], 
    inout LineStream<GS_OUTPUT> output
)
{
    const float3 triangleNormal = normalize(input[0].vNormal + input[1].vNormal + input[2].vNormal);

    for (int i = 0; i < 3; ++i)
    {
        const float4 worldPosition = float4(input[i].vWorldPosition, 1.0f);
        const float4 viewPosition = mul(g_worldToView, worldPosition);
        const float4 clipPosition = mul(g_viewToClip, viewPosition);
        GS_OUTPUT outputVertex;
        {
            outputVertex.vPosition = clipPosition;
            outputVertex.vUV = input[i].vUV;
            outputVertex.vNormal = input[i].vNormal;
            outputVertex.vTangent = input[i].vTangent;
            outputVertex.vBitangent = input[i].vBitangent;
            outputVertex.vColor = input[i].vColor;
        }
        output.Append(outputVertex);
    }
    output.RestartStrip();
}
