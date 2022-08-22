#include "slash.hlsli"

SamplerState sampler_border_black : register(s3);
Texture2D texture_map : register(t0);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = texture_map.Sample(sampler_border_black, pin.texcoord).a;
    float4 color = float4(0,3.5,10, alpha);
    return color;
}
