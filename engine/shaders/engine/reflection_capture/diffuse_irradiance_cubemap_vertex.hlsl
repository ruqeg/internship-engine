cbuffer CameraCB : register(b0)
{
    float4x4 g_worldToClip;
};

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 direction : DIRECTION;
};
    
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 clipPosition = mul(g_worldToClip, float4(input.position, 1.f));
    output.position = clipPosition;
    output.direction = normalize(input.position);

    return output;
}