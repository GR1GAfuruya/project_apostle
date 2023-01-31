#include "mesh_effect.hlsli"
#include "constants.hlsli"
#include "math.hlsli"
SamplerState sampler_border_black : register(s3);
Texture2D main_texture : register(t20);
Texture2D dissolve_texture : register(t21);


float4 main(VS_OUT pin) : SV_TARGET
{
    float alpha = main_texture.Sample(sampler_border_black, pin.texcoord).r;
    float2 tile = (3, 1);
    float4 tex = main_texture.Sample(sampler_border_black, tilling(pin.texcoord, tile));
    clip(tex.rgb - 0.01);
   
    float3 main_color = tex.rgb * particle_color.rgb;
    
      //ƒfƒBƒ]ƒ‹ƒu
    float4 dissolve = dissolve_texture.Sample(sampler_border_black, pin.texcoord);
    alpha = alpha * step(threshold, dissolve.r) * particle_color.a;
    clip(alpha - 0.01);
    return float4(main_color, alpha);
}
