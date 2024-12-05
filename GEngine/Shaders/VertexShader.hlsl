struct VSInput
{
    float4 inPos : POSITION;
    float4 inColor : COLOR;
};

struct VSOutput
{
    float4 outPos : SV_POSITION;
    float4 outColor : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.outPos = input.inPos;
    output.outColor = input.inColor;
    return output;
}