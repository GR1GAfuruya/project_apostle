#include "fullscreen_quad.hlsli"
#include "constants.hlsli"

//	テクスチャ
Texture2D albedo_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);
Texture2D metal_smooth_map : register(t3);
Texture2D emissive_map : register(t4);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = tex;
    float4 light = emissive_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
  //  light = 0;
    //メタリック
    float metallic = metal_smooth_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
 
   // metallic = 1 - metallic;
    float3 m1 = color.rgb * light.rgb;
    float3 m0 = color.rgb * 0.4;
    m0 += light.rgb * 0.25;
    //m0 = 0;
    //m1 = 0;
    //color.rgb = lerp(m0, m1, 0);
    color.rgb = lerp(m0, m1, metallic);

    //フレネル反射
    //視線
    float4 P = position_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 E = P.xyz - camera_position.xyz;
    E = normalize(E);
    //法線
    float3 N = normal_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float NoLight = 1.0 - length(N);
    NoLight = step(0.9, NoLight);
    N = N * 2.0 - 1.0;//0<-->1 -1<-->+1
    N = normalize(N);
	// 内積で視線と法線の向き合い具合
    float d = dot(-E, N);
    float fresnel = saturate(1.0 - d) * 0.6;
    fresnel = pow(fresnel, 6.0);
    color.rgb = lerp(color.rgb,
				light.rgb, fresnel);
    //color.rgb = lerp(color.rgb,
				//tex.rgb, NoLight);
    return color;


}