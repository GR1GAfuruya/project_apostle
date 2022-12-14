#include "mesh_effect.hlsli"
#include "constants.hlsli"
SamplerState sampler_border_black : register(s3);
Texture2D texture_maps[4] : register(t20);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = texture_maps[0].Sample(sampler_border_black, pin.texcoord.yx).r;
    float4 tex = texture_maps[0].Sample(sampler_border_black, pin.texcoord.yx);
    clip(tex.rgb - 0.01);
   
   // float4 color = float4(10,0,0, alpha);
    float4 color = float4(particle_color.rgb * tex.rgb, alpha * particle_color.a);
    return color ;
}
