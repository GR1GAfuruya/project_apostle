
#include "mesh_effect.hlsli"
#include "constants.hlsli"
SamplerState sampler_border_black : register(s1);
Texture2D texture_maps[4] : register(t20);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = texture_maps[1].Sample(sampler_border_black, pin.texcoord).r;
    float4 tex = texture_maps[1].Sample(sampler_border_black, pin.texcoord);
    alpha = step(threshold, alpha);
    float4 color = float4(particle_color.rgb * tex.rgb, alpha * particle_color.a);
    clip(tex.rgba - 0.01);
    return color;
}