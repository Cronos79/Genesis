struct PSInput
{
    float4 inPos : SV_POSITION;
    float4 inColor : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(input.inColor);
}