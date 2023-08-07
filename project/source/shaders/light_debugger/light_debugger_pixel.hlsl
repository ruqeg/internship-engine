struct VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : INSTANCECOLOR;
};

struct PS_OUTPUT
{
    float4 color : SV_Target;
};

PS_OUTPUT
main
(VS_OUTPUT input)
{
    PS_OUTPUT output;
    {
        output.color = input.vColor;
    }
    return output;
}