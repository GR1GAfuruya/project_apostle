#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState pointSamplerState : register(s0);
SamplerState linerSamplerState : register(s1);
SamplerState anisotropic_sampler_state : register(s2);
SamplerState clamp_sampler_state : register(s3);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(clamp_sampler_state, pin.texcoord);
    float alpha = color.a;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    return float4(color.rgb, alpha) * pin.color;
}
