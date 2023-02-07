#include "fullscreen_quad.hlsli"
#include "math.hlsli"
#include "constants.hlsli"
Texture2D originall_texture : register(t0);
Texture2D dissolve_texture : register(t10);
Texture2D distortion_texture : register(t11);

// サンプラーステート
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

//テクスチャを歪ませる処理（返り値はテクスチャのUVに代入）
float2 distortion(float2 texcoord, float2 tile, float2 scroll_speed)
{
    //タイリングとUVスクロールの処理
    float2 distortion_uv = tilling(texcoord, tile) + (scroll_speed * time);
    //歪み
    float4 distortion_value = distortion_texture.Sample(sampler_states[LINEAR], distortion_uv);
    return distortion_value.rg;

}


float3 RGB2HSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 HSV2RGB(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float4 color_filter(float4 c)
{
    float4 color = c;
	// RGB > HSVに変換
    color.rgb = RGB2HSV(color.rgb);

	// 色相調整
    color.r += hueShift;

	// 彩度調整
    color.g *= saturation;

	// 明度調整
    color.b *= brightness;

	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);

    return color;
}

//コントラスト
float4 Contrast(float4 c)
{
    float4 color = c;
	//シグモイド曲線によるS字カーブ
    color = 1 / (1 + exp(-contrast * (color - 0.5)));

    return color;
}

//ビネット
float4 Vignette(float4 c,float2 uv)
{
    float3 color = c.rgb;
    float2 center = float2(0.5, 0.5);
    float len = length(uv - center);
    float vignette = smoothstep(1., falloff, len * (amount + falloff));
    color *= vignette;
    color += vignette_color * (1 - vignette);
    return float4(color, 1.0);

}


float weightedColour(float colour, int offset, float mid)
{
    
    return colour * 1.0 * (mid - abs(mid - float(offset))) / mid;
}

float4 radial_blur(float4 c, float2 uv)
{
    const float blurMax = 0.2;
    const float aberrationMax = 1.2;

    const int numIters = 51;


    float blur = -blurMax * radial_power;
    float aberration = 1.0;

    int channelspread = int(float(numIters) / aberration);
    float mid = float(channelspread + 1) / 2.0;

    float3 lobound = float3(numIters - channelspread, (numIters - channelspread) / 2, 0.0);
    float3 hibound = float3(numIters, (numIters + channelspread) / 2, channelspread);

 
    
    const float2 centre = float2(0.5, 0.5);
    uv -= centre;

    float4 color = float4(0.0, 0.0, 0.0, 0.0);

    float blurPerIter = blur / float(numIters - 1);
    for (int i = 0; i < numIters; i++)
    {
        float scale = 1.0 + (blurPerIter * float(i));
        float4 txColour = originall_texture.Sample(sampler_states[ANISOTROPIC], centre + (uv * scale));
       
        float3 kk = float3(i + 1, i + 1, i + 1) - lobound;
        
        if (i >= lobound.x && i < hibound.x)
        {
            color.x += weightedColour(txColour.x, kk.x, mid);
        }
        if (i >= lobound.y && i < hibound.y)
        {
            color.y += weightedColour(txColour.y, kk.y, mid);
        }
        if (i >= lobound.z && i < hibound.z)
        {
            color.z += weightedColour(txColour.z, kk.z, mid);
        }
    }
    
    
    return color / (0.5 * float(channelspread + 1));
}

float dissolve(float2 uv)
{
    float4 dissolve = dissolve_texture.Sample(sampler_states[LINEAR], uv);
    float alpha = step(scsene_threshold, dissolve.r);
    return alpha;
}

float4 main(VS_OUT pin) : SV_Target0
{
    float4 sampled_color = originall_texture.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = sampled_color;
    //ラジアルブラー
    color = radial_blur(color, pin.texcoord);
    //カラーフィルター
    color = color_filter(color);
    //コントラスト
    color = Contrast(color);
    //ビネット
    color = Vignette(color,pin.texcoord);
    
    //ディゾルブ
    float2 tile = float2(0.3, 0.3);
    float2 scroll_speed = float2(0, 0);
    color.a = dissolve(distortion(pin.texcoord, tile, scroll_speed));
    return color;
}