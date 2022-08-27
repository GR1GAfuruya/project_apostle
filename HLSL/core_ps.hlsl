#include "aura.hlsli"
#include "constants.hlsli"
SamplerState sampler_border_black : register(s1);
Texture2D texture_maps : register(t20);
Texture2D dissolve_maps : register(t21);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = dissolve_maps.Sample(sampler_border_black, pin.texcoord.x).r;
    float4 tex = dissolve_maps.Sample(sampler_border_black, pin.texcoord.x);
    clip(tex.rgb - 0.01);
   
    float4 color = float4(particle_color.rgb * tex.rgb, alpha * particle_color.a);
    return color;
}