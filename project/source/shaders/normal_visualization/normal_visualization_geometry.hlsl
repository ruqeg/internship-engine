#include "vertex_output_struct.hlsli"
#include "../camera_constant_buffer.hlsli"
#include "geometry_output_struct.hlsli"

static const int NUM_TRIANGLES = 1;
static const float LINE_LENGTH = 0.01;
static const float3 LINE_COLOR = float3(1.0f, 1.0f, 1.0f);

[maxvertexcount(2 * NUM_TRIANGLES)]
void 
main
(
    triangle VS_OUTPUT input[3], 
    inout LineStream<GS_OUTPUT> output
)
{
    const float3 triangleNormal = normalize(input[0].vNormal + input[1].vNormal + input[2].vNormal);
    const float3 triangleCentroid = (input[0].vWorldPosition + input[1].vWorldPosition + input[2].vWorldPosition) / 3.0f;
    
    GS_OUTPUT outputVerties[2];
    {
        outputVerties[0].vWorldPosition = triangleCentroid;
        outputVerties[1].vWorldPosition = triangleCentroid + triangleNormal * LINE_LENGTH;

        for (int i = 0; i < 2; ++i)
        {
            const float4 worldPosition = float4(outputVerties[i].vWorldPosition, 1.0f);
            const float4 viewPosition = mul(g_worldToView, worldPosition);
            const float4 clipPosition = mul(g_viewToClip, viewPosition);
            outputVerties[i].vPosition = clipPosition;
            outputVerties[i].vUV = input[i].vUV;
            outputVerties[i].vNormal = input[i].vNormal;
            outputVerties[i].vTangent = input[i].vTangent;
            outputVerties[i].vBitangent = input[i].vBitangent;
            outputVerties[i].vColor = LINE_COLOR;
            output.Append(outputVerties[i]);
        }
    }
    output.RestartStrip();
}