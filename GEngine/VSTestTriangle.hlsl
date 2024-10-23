#include "RSTestTriangle.hlsl"

[RootSignature(ROOTSIG)]
void main(
    // == IN ==
    in float2 pos : Position,
    // == OUT ==
    out float4 o_pos : SV_Position
)
{
    float2 px;
    px.x = pos.x; 
    px.y = pos.y;
    o_pos = float4(px, 0.0f, 1.0f);
}