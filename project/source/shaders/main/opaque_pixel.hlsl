#include <engine/algorithms/octahedron.hlsli>

#define SAMPLER_INDEX s0

#define ALBEDO_TEXTURE_INDEX t0
#define METALLIC_TEXTURE_INDEX t1
#define ROUGHNESS_TEXTURE_INDEX t2
#define NORMAL_TEXTURE_INDEX t3

Texture2D g_albedoMap : register(ALBEDO_TEXTURE_INDEX);
Texture2D g_metallicMap : register(METALLIC_TEXTURE_INDEX);
Texture2D g_roughnessMap : register(ROUGHNESS_TEXTURE_INDEX);
Texture2D g_normalMap : register(NORMAL_TEXTURE_INDEX);

SamplerState g_sampler : register(SAMPLER_INDEX);

struct VS_OUTPUT
{
    float3 vWorldPosition : WORLDPOSITON;
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBitangent : BITANGENT;
    float4 vColor : INSTANCECOLOR;
    uint objectID : INSTANCEOBJECTID;
};

struct PS_OUTPUT
{
    float3 albedo : SV_Target0;
    float2 metallicRoughness : SV_Target1;
    float4 normals : SV_Target2;
    float4 emission : SV_Target3;
    uint objectID : SV_Target4;
};

float3 normalFromNormalMap(Texture2D tex, float2 uv, float3 tangent, float3 bitangent, float3 normal);

PS_OUTPUT
main
(VS_OUTPUT input)
{
    const float3 albedo = g_albedoMap.Sample(g_sampler, input.vUV);
    const float metallic = g_metallicMap.Sample(g_sampler, input.vUV);
    const float roughness = g_roughnessMap.Sample(g_sampler, input.vUV);
    const float3 textureNormal = normalFromNormalMap(g_normalMap, input.vUV, input.vTangent, input.vBitangent, input.vNormal);
    const float3 geometryNormal = input.vNormal;
    const float2 packedTextureNormal = packOctahedron(textureNormal);
    const float2 packedGeometryNormal = packOctahedron(geometryNormal);
    
    PS_OUTPUT output;
    {
        output.albedo = albedo;
        output.metallicRoughness = float2(metallic, roughness);
        output.normals = float4(packedTextureNormal, packedGeometryNormal);
        output.objectID = input.objectID;
    }
    return output;

}

float3 normalFromNormalMap(Texture2D tex, float2 uv, float3 tangent, float3 bitangent, float3 normal)
{
    float3 normalTex = tex.Sample(g_sampler, uv).xyz;
    normalTex = normalTex * 2.0f - 1.0f;
    const float3x3 TBN = float3x3(tangent, bitangent, normal);
    float3 result = normalize(mul(normalTex, TBN));
    return result;
}