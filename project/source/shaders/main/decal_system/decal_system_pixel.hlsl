#include <engine/math_constants.hlsli>
#include <engine/algorithms/octahedron.hlsli>
#include "../../camera_constant_buffer.hlsli"
#include "../../global_samplers_states.hlsli"
#include "../../plasma_constant_buffer.hlsli"

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4x4 decalToWorld : decalToWorld;
    float4x4 worldToDecal : worldToDecal;
    float4 color : INSTANCECOLOR;
    uint parentObjectID : INSTANCEPARENTOBJECTID;
};

struct PS_OUTPUT
{
    float3 albedo : SV_Target0;
    float4 normals : SV_Target1;
    float2 metallicRoughness : SV_Target2;
    float4 emission : SV_Target3;
};

#define TEXTURE_NORMAL_GEOM_NORMAL_TEXTURE_INDEX t2
#define DEPTH_TEXTURE_INDEX t4
#define OBJECID_TEXTURE_INDEX t5
#define DECAL_NORMAL_INDEX t6

Texture2D g_texNormGeomNormTexture : register(TEXTURE_NORMAL_GEOM_NORMAL_TEXTURE_INDEX);
Texture2D g_depthTexture : register(DEPTH_TEXTURE_INDEX);
Texture2D g_decalNormalTexture : register(DECAL_NORMAL_INDEX);
Texture2D<uint> g_objectIDTexture : register(OBJECID_TEXTURE_INDEX);

float3 VSPositionFromDepth(float2 vTexCoord, float depth);
float3 decalNormalFromNormalMap(float4 decalNormalTexel, float3 decalBasisX, float3 normal);

static const float CONST_DECAL_ROUGNESS = 0.78f;
static const float CONST_DECAL_METALLIC = 0.19f;

PS_OUTPUT
main
(VS_OUTPUT input)
{
    const float2 screenUV = input.position.xy / g_resolution.xy;
    
    const float depth = g_depthTexture.Sample(g_sampler, screenUV).r;
    const float4 texNormGeomNorm = g_texNormGeomNormTexture.Sample(g_sampler, screenUV);
    const uint objectID = g_objectIDTexture.Load(uint3(input.position.xy, 0u));
    
    const float3 n = unpackOctahedron(texNormGeomNorm.xy);
    const float3 gn = unpackOctahedron(texNormGeomNorm.zw);
    
    const float3 viewPosition = VSPositionFromDepth(screenUV, depth);
    const float3 worldPosition = mul(g_viewToWorld, float4(viewPosition, 1.f));
    const float3 decalPosition = mul(input.worldToDecal, float4(worldPosition, 1.f));
    
    if (abs(decalPosition.x) > 1.f || abs(decalPosition.y) > 1.f || abs(decalPosition.z) > 1.f)
        discard;
    
    const float2 decalUV = float2(
        decalPosition.x * 0.5f + 0.5f,
        decalPosition.y * -0.5f + 0.5f);
    
    if (objectID != input.parentObjectID)
        discard;
    
    //if (dot(gn, input.decalToWorld[2].xyz) < 0.0001f)
    //    discard;
    
    const float4 decalNormalTexel = g_decalNormalTexture.Sample(g_sampler, decalUV);
    
    if (decalNormalTexel.a == 0.0f)
    {
        discard;
    }
    
    const float3 decalNormal = decalNormalFromNormalMap(decalNormalTexel, input.decalToWorld[0].xyz, gn);

    PS_OUTPUT output;
    output.albedo = input.color.rgb;
    output.normals = float4(packOctahedron(decalNormal), texNormGeomNorm.zw);
    output.metallicRoughness = float2(CONST_DECAL_ROUGNESS, CONST_DECAL_METALLIC);
    output.emission = 0.f;
    return output;
}

float3 decalNormalFromNormalMap(float4 decalNormalTexel, float3 decalBasisX, float3 normal)
{
    const float3 normalFromTex = decalNormalTexel * 2.f - 1.f;
    const float3 T = normalize(decalBasisX - normal * dot(normal, decalBasisX));
    const float3 B = normalize(cross(normal, T));
    const float3x3 TBN = float3x3(T, B, normal);
    const float3 decalN = normalize(mul(normalFromTex.xyz, TBN));
    return decalN;
}

float3 VSPositionFromDepth(float2 vTexCoord, float depth)
{
    // Get the depth value for this pixel
    float z = depth;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(g_clipToView, vProjectedPos);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}