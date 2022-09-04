#include "mesh_effect.hlsli"
#include "constants.hlsli"
SamplerState sampler_border_black : register(s1);
Texture2D texture_maps : register(t20);
Texture2D dissolve_maps : register(t21);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = dissolve_maps.Sample(sampler_border_black, pin.texcoord.xy).r;
    float3 tex = dissolve_maps.Sample(sampler_border_black, pin.texcoord.xy).rgb;
    float yellow = dissolve_maps.Sample(sampler_border_black, pin.texcoord.yx).r + 1;
    float3 baseColor = particle_color.rgb * tex.rgb;
    clip(tex.rgb - 0.01);
   
    float4 color = float4(baseColor.r * yellow, baseColor.g * yellow, baseColor.b * yellow, alpha * particle_color.a);
    return color;
}