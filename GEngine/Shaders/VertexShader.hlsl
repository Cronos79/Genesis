struct VSOut
{
    float4 color : Color;
    float4 pos : SV_POSITION;    
};

cbuffer CBuf
{
    matrix transform;
};

VSOut main(float3 pos : Position, float4 color : Color)
{
    VSOut output;
    output.pos = mul(float4(pos, 1.0f), transform);
    output.color = color;
    return output;
}