#include "geometry_output_struct.hlsli"

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT
main
(GS_OUTPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    {
        output.color = float4(input.vColor, 1.0f);
    }
    return output;
}