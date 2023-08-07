struct VS_OUTPUT
{
	float3 vWorldPosition : WORLDPOS;
	float2 vUV : TEXCOORD;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float3 vBitangent : BITANGENT;
	float3 vColor : COLOR;
};