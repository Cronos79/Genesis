cbuffer alphaBuffer : register(b0)
{
    float alpha;
};

struct PSInput
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord).xyz;
    return float4(pixelColor, alpha);
}