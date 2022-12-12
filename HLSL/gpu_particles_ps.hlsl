#include "gpu_particles.hlsli"
#include "constants.hlsli"
SamplerState anisotropic_sampler_state : register(s2);

Texture2D color_map : register(t5);

[earlydepthstencil]
float4 main(GS_OUT pin) : SV_TARGET
{
    float4 sample_color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float3 color = sample_color.rgb * pin.color.rgb;
    float alpha = sample_color.a * pin.color.a;
    
    clip(alpha - 0.01);
    return float4(color, alpha);
}
