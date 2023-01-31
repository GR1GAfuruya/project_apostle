#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#include "PBR_function.hlsli"
#include "shading_functions.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
//	テクスチャ
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);

// ピクセルシェーダー
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex = normal_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord); // *input.Color;

	//0624 点光源
    float3 PLightPos = defferd_light_dir.xyz;
    float4 P = position_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 PLightDir = P.xyz - PLightPos;
	// ライトタイプ判定(0:平行光 1:点光源)
    float lighttype = step(0.01, defferd_light_dir.w);
	// 方向決定(平行光:w=0.0 点光源:w>0)
	//float3 dir = LightDir.w < 0.001 ? LightDir.xyz : PLightDir;
    float3 dir = lerp(
	  defferd_light_dir.xyz, PLightDir, lighttype);
	//減衰
    float attenuation = defferd_light_dir.w < 0.001 ?
		1.0 :
		1.0 - (length(PLightDir) / defferd_light_dir.w);
    attenuation = saturate(attenuation); //0.0<--->1.0
	// ライト計算
    float3 L = dir; //方向
    float3 C = defferd_light_color.rgb; //カラー
    C *= attenuation; // 減衰
    float3 N = tex * 2.0 - 1.0; //法線
    L = normalize(L);
    N = normalize(N);
	// 当たり具合 = -cos = -内積
    float i = -dot(N, L);
    i = saturate(i); // 0.0～1.0
    tex.rgb = C * i; // ライトカラー決定
    return tex;
}

