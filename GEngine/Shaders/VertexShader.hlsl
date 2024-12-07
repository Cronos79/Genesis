cbuffer CBuf : register(b0)
{
    float4x4 mat;
};

struct VSInput
{
    float4 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VSOutput
{
    float4 outPos : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.outPos = mul(input.inPos, mat);
    output.outTexCoord = input.inTexCoord;
    return output;
}