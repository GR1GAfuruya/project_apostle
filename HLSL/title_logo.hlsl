#include "sprite.hlsli"

#include "constants.hlsli"
#include "math.hlsli"

cbuffer CONSTANT_BUFFER : register(b9)
{
    float4 particle_color;
    float threshold;
};


SamplerState sampler_texture : register(s1);
Texture2D main_texture : register(t0);
Texture2D texture_mask : register(t20);
Texture2D texture_map : register(t21);
Texture2D distortion_texture : register(t22);

//テクスチャを歪ませる処理（返り値はテクスチャのUVに代入）
float2 distortion(float2 texcoord, float2 tile, float2 scroll_speed)
{
    //タイリングとUVスクロールの処理
    float2 distortion_uv = tilling(texcoord, tile) + (scroll_speed * time);
    //歪み
    float4 distortion_value = distortion_texture.Sample(sampler_texture, distortion_uv);
    return distortion_value.rg;

}
float4 main(VS_OUT pin) : SV_TARGET
{
    //メインカラー
    float2 scroll_speed = float2(0.0, 0.5);
    float2 distortion_tile = float2(1.5, 1.5);
    float2 tile = float2(1.0, 1.0);
    float2 uv = pin.texcoord + distortion(pin.texcoord, distortion_tile, scroll_speed);
    float4 main_color = texture_map.Sample(sampler_texture, tilling(uv, tile));
    main_color *= float4(1.0f, 0.8f, 5.5f, 1.0f);
    
    //黒い部分
    scroll_speed = float2(0.1, 0.5);
    distortion_tile = float2(2.0, 2.0);
    uv = pin.texcoord + distortion(pin.texcoord, distortion_tile, scroll_speed);
    float4 mask = texture_mask.Sample(sampler_texture, tilling(uv, tile));
    float4 result_color = main_color * mask;
    
    //Alphaマスク
    scroll_speed = float2(0.0, 0.0);
    distortion_tile = float2(1.0, 1.0);
    tile = float2(1.0,1.0);
    uv = pin.texcoord + distortion(pin.texcoord, distortion_tile, scroll_speed);
    float alpha = main_texture.Sample(sampler_texture, pin.texcoord).r;
    alpha *= mask.r;
    //ディゾルブ
    //float4 dissolve = texture_map.Sample(sampler_texture, pin.texcoord);
    //alpha = alpha * step(threshold, dissolve.r);
    //clip(alpha - 0.01);
    //カラー出力
    return float4(result_color.rgb, alpha);
}
