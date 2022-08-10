#include "fullscreen_quad.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
  
    return texture_map[0].Sample(sampler_states[POINT], pin.texcoord);
}