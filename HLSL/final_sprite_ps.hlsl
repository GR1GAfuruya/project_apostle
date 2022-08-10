#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D color_map : register(t0);
Texture2D depth_map : register(t1);
Texture2D normal_map : register(t2);
Texture2D position_map : register(t3);
Texture2D light_map : register(t4);


float4 main(VS_OUT pin) : SV_TARGET
{

    float4 color = color_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
   
    return color;
}