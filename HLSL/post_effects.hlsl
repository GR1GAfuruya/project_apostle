#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
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

float4 tone_map(float4 sample_color)
{
    float4 color = sample_color;
    
    float3 fragment_color = color.rgb;
    float alpha = color.a;

#if 1
	// Apply tone mapping.
    const float exposure = brightness;
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

//--------------------------------------------
//	RGB色空間の数値からHSV色空間の数値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float3 RGB2HSV(float3 rgb)
{
    float3 hsv = 0;

    float Vmax = max(rgb.r, max(rgb.g, rgb.b)); // RGBの三つの値で最大のもの
    float Vmin = min(rgb.r, min(rgb.g, rgb.b)); // RGBの三つの値で最小のもの
    float delta = Vmax - Vmin; // 最大値と最小値の差

	// V（明度） 一番強い色をV値にする
    hsv.z = Vmax;

	// S（彩度） 最大値と最小値の差を正規化して求める
    hsv.y = (delta / Vmax) * step(0, Vmax);

	// H（色相） RGBのうち最大値と最小値の差から求める
    if (hsv.y > 0.0f)
    {
        if (rgb.r == Vmax)
        {
            hsv.x = 60 * ((rgb.g - rgb.b) / delta);
        }
        else if (rgb.g == Vmax)
        {
            hsv.x = 60 * ((rgb.b - rgb.r) / delta) + 120;
        }
        else
        {
            hsv.x = 60 * ((rgb.r - rgb.g) / delta) + 240;
        }

        if (hsv.x < 0)
        {
            hsv.x += 360;
        }
    }
    return hsv;
}

//--------------------------------------------
//	HSV色空間の数値からRGB色空間の数値への変換関数
//--------------------------------------------
//hsv:HSV色空間の数値
float3 HSV2RGB(float3 hsv)
{
    float3 rgb = 0;
    if (hsv.y == 0)
    {
		// S（彩度）が0と等しいならば無色もしくは灰色
        rgb.r = rgb.g = rgb.b = hsv.z;
    }
    else
    {
		// 色環のH（色相）の位置とS（彩度）、V（明度）からRGB値を算出する
        float Vmax = hsv.z;
        float Vmin = Vmax - (hsv.y * Vmax);

        hsv.x %= 360; //	0～360に変換
        float Huei = (int) (hsv.x / 60);
        float Huef = hsv.x / 60 - Huei;
        float p = Vmax * (1.0f - hsv.y);
        float q = Vmax * (1.0f - hsv.y * Huef);
        float t = Vmax * (1.0f - hsv.y * (1 - Huef));
        if (Huei == 0)
        {
            rgb.r = Vmax;
            rgb.g = t;
            rgb.b = p;
        }
        else if (Huei == 1)
        {
            rgb.r = q;
            rgb.g = Vmax;
            rgb.b = p;
        }
        else if (Huei == 2)
        {
            rgb.r = p;
            rgb.g = Vmax;
            rgb.b = t;
        }
        else if (Huei == 3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = Vmax;
        }
        else if (Huei == 4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = Vmax;
        }
        else if (Huei == 5)
        {
            rgb.r = Vmax;
            rgb.g = p;
            rgb.b = q;
        }
    }
    return rgb;
}

float4 color_filter(float4 sample_color)
{
    float4 color = sample_color;
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

float4 main(VS_OUT pin) : SV_Target0
{
    float4 sampled_color = originall_texture.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = sampled_color;

    color = color_filter(color);
    return color;
}