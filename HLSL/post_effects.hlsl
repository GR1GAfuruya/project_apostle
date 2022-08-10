#include "fullscreen_quad.hlsli"
Texture2D originall_texture : register(t0);

// サンプラーステート
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

static const float gamma = 2.3;
float3 simple_reinhard_tonemapping(float3 color, float exposure)
{
    //float exposure = 1.5;
    color *= exposure / (1. + color / exposure);
    color = pow(color, 1. / gamma);
    return color;
}

float4 main(VS_OUT pin) : SV_Target0
{
    float4 sampled_color = originall_texture.Sample(sampler_states[POINT], pin.texcoord);
    
    float3 fragment_color = sampled_color.rgb;
    float alpha = sampled_color.a;

#if 1
	// Apply tone mapping.
	const float exposure = 1.2;
	//fragment_color = 1 - exp(-fragment_color * exposure);
	
	//fragment_color = filmic_tonemapping(fragment_color);
    //fragment_color = uncharted2_tonemapping(fragment_color, exposure);
    //fragment_color = white_preserving_lumabased_reinhard_tonemapping(fragment_color);
    fragment_color = simple_reinhard_tonemapping(fragment_color, exposure);
#endif
    
#if 0
	// Apply gamma correction.
	const float GAMMA = 2.2;
	fragment_color = pow(fragment_color, 1.0 / GAMMA);
#endif
    
    return float4(fragment_color, alpha);

}