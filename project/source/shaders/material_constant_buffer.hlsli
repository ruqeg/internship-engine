cbuffer MaterialCBS : register(b3)
{
    float3 g_materialAmbient;
    float3 g_materialDiffuse;
    float3 g_materialSpecular;
    float3 g_materialShininess;
    float g_materialMetallic;
    float g_materialRoughness;
};