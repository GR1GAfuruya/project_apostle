#include "sprite_dissolve.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
    float4 dst_color = color_map.Sample(color_sampler_state, pin.texcoord) * float4(0, 0, 0, 0);
    float4 mask = mask_texture.Sample(color_sampler_state, pin.texcoord) * float4(1, 1, 1, 1);
    return lerp(color, dst_color, step(mask.r, dissolve_value));


}