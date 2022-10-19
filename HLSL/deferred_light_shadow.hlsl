#include "fullscreen_quad.hlsli"
#include "constants.hlsli"

// シャドウマップ用コンスタントバッファ
cbuffer CBShadowMap : register(b10)
{
	row_major float4x4  ShadowVP; // 光の位置カメラ*投映
}

// シーンテクスチャ
Texture2D sceneTexture : register(t0);
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);
Texture2D metal_smooth_map : register(t3);
Texture2D emissive_map : register(t4);

Texture2D shadow_map : register(t16);

// シーンサンプラステート
SamplerState sceneSampler : register(s5);
SamplerState BorderSampler : register(s6);

float4 main(VS_OUT pin) : SV_TARGET
{
	// カラーの取得
	float4 tex = sceneTexture.Sample(sceneSampler, pin.texcoord);
	// 点光源方向
    float4 P = position_map.Sample(sceneSampler, pin.texcoord);
    float3 PLightPos = defferd_light_dir.xyz;
	float3 PLightDir = P.xyz - PLightPos;
	// ライト種別
    float type = step(0.001, defferd_light_dir.w);
	// 減衰の設定
    float attenuation = type < 0.1 ? 1.0 : 1.0 - (length(PLightDir) / defferd_light_dir.w);
	attenuation = saturate(attenuation);
	// 平行光
    float3 L = lerp(defferd_light_dir.xyz, PLightDir, type);

	float3 N = tex.xyz * 2.0 - 1.0;
    float3 C = defferd_light_color.rgb;
	C *= attenuation; //減衰

	L = normalize(L);
	N = normalize(N);
	float d = -dot(N, L); //= -cos
	d = saturate(d); // 0～1にクリッピング
	tex.rgb = C * d; //ライト決定

	// Deferred Shadow
	// 光から見たXYZ W
	float4 shadowparam = mul(P, ShadowVP);
	float d2 = saturate(shadowparam.z / shadowparam.w);

	//ShadowMap
	float2 uv = shadowparam.xy;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;//-1<--->+1を0<--->1

    float2 dist = shadow_map.Sample(BorderSampler, uv).xy;
/*
	// チェビシェフの不等式を利用した分散シャドウマップ
	float depth_sq = dist.x * dist.x;
	float variance = dist.y - depth_sq;
	float md = d2 - dist.x;
	float p = variance / (variance + (md * md));
	float shadow = p;// saturate(max(p, dist.x >= d2 - 0.001));
*/
	float shadow = dist.x > d2 - 0.003;
    tex.rgb *= shadow;
	tex.a = 1;

	return tex;
}

