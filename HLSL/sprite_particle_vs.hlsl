#include "sprite_particle.hlsli"
#include "constants.hlsli"


VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = mul(position, view_projection);
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}