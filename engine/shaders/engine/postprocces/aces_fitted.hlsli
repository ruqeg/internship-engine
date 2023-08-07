///////////////////////////////////////////////
// ACES fitted
// from https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
///////////////////////////////////////////////

static const float3x3 g_ACESInputMat = float3x3(
		float3(0.59719f, 0.07600f, 0.02840f),
		float3(0.35458f, 0.90834f, 0.13383f),
		float3(0.04823f, 0.01566f, 0.83777f)
);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 g_ACESOutputMat = float3x3(
	float3(1.60475f, -0.10208, -0.00327f),
	float3(-0.53108f, 1.10813, -0.07276f),
	float3(-0.07367f, -0.00605, 1.07602f)
);

float3 RRTAndODTFit(float3 v)
{
    const float3 a = v * (v + 0.0245786f) - 0.000090537f;
    const float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color = mul(color, g_ACESInputMat);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(color, g_ACESOutputMat);
    
    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}