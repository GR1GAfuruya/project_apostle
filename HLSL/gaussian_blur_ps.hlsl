#include "constants.hlsli"
#include "gaussian_blur.hlsli"
// サンプラーステート
#define POINT 0
#define LINEAR 1h
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

Texture2D<float4> scene_texture : register(t0); // シーンテクスチャ
float4 main(VS_OUT pin) : SV_TARGET
{
	 // X,Yブラー用のピクセルシェーダーを実装
    float4 Color;
    return 1;
    // 基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    //Color = weight[0].x * scene_texture.Sample(sampler_states[POINT], pin.tex0.xy);
    //Color += weight[0].y * scene_texture.Sample(sampler_states[POINT], pin.tex1.xy);
    //Color += weight[0].z * scene_texture.Sample(sampler_states[POINT], pin.tex2.xy);
    //Color += weight[0].w * scene_texture.Sample(sampler_states[POINT], pin.tex3.xy);
    //Color += weight[1].x * scene_texture.Sample(sampler_states[POINT], pin.tex4.xy);
    //Color += weight[1].y * scene_texture.Sample(sampler_states[POINT], pin.tex5.xy);
    //Color += weight[1].z * scene_texture.Sample(sampler_states[POINT], pin.tex6.xy);
    //Color += weight[1].w * scene_texture.Sample(sampler_states[POINT], pin.tex7.xy);

    //// 基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    //Color += weight[0].x * scene_texture.Sample(sampler_states[POINT], pin.tex0.zw);
    //Color += weight[0].y * scene_texture.Sample(sampler_states[POINT], pin.tex1.zw);
    //Color += weight[0].z * scene_texture.Sample(sampler_states[POINT], pin.tex2.zw);
    //Color += weight[0].w * scene_texture.Sample(sampler_states[POINT], pin.tex3.zw);
    //Color += weight[1].x * scene_texture.Sample(sampler_states[POINT], pin.tex4.zw);
    //Color += weight[1].y * scene_texture.Sample(sampler_states[POINT], pin.tex5.zw);
    //Color += weight[1].z * scene_texture.Sample(sampler_states[POINT], pin.tex6.zw);
    //Color += weight[1].w * scene_texture.Sample(sampler_states[POINT], pin.tex7.zw);

    //return float4(Color.xyz, 1.0f);
}