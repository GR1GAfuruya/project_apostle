#include "sprite_particle.hlsli"
SamplerState sampler_border_black : register(s3);
Texture2D color_map : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(sampler_border_black, pin.texcoord) * pin.color;
    float alpha = color.a;
    clip(color.a - 0.1);
    return color;
}
