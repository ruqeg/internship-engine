struct GS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPosition : WORLDPOS;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBitangent : BITANGENT;
    float3 vColor : COLOR;
};