#include "constants.hlsli"
SamplerState anisotropic_sampler_state : register(s2);

Texture2D color_map : register(t5);

struct VS_OUT
{
    float4 position : SV_POSITION; //vertexShaderは必ずこのセマンティックの値を出力しなければならない！！！！！   （値 = 頂点のスクリーン上の位置（ＮＤＣ）
    float4 color : COLOR;
    float2 texcoord : TEXCOORD; //UV座標
};
float2x2 rotz(float angle)
{
    float2x2 m;
    m[0][0] = cos(angle); m[0][1] = -sin(angle);
    m[1][0] = sin(angle); m[1][1] = cos(angle);
    return m;
}

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float2 mul_vec2_mat(float2 vec, float2x2 mat)
{
    float2 v;
    v.x = vec.x * mat[0][0] + vec.y * mat[0][1];
    v.y = vec.x * mat[1][0] + vec.y * mat[1][1];
    return v;
}

float fbm(float2 uv)
{
    
    float n = (color_map.Sample(anisotropic_sampler_state, uv).r - 0.5) * 0.5;
    n += (color_map.Sample(anisotropic_sampler_state, uv * 2.0).r - 0.5) * 0.5 * 0.5;
    n += (color_map.Sample(anisotropic_sampler_state, uv * 3.0).r - 0.5) * 0.5 * 0.5 * 0.5;
    
	return n + 0.5;
}


// -----------------------------------------------
float4 main(VS_OUT pin): SV_TARGET
{
    
    float2 uv = pin.texcoord.xy;
    float2 _uv = uv;

    uv -= float2(0.5, 1.5);
    uv.y /= pin.texcoord.x / pin.texcoord.y;
    uv.x /= pin.texcoord.x / pin.texcoord.y / 3;
    float2 centerUV = uv;
    
    // height variation from fbm
    const float variationH = fbm(float2(time * 0.3, time * 0.3)) * 1.1;
    
    // flame "speed"
    const float2 offset = float2(0.0, -time * 0.15);
    
    // flame turbulence
    float f = fbm(uv * 0.5 + offset); // rotation from fbm
    float l = max(0.1, length(uv)); // rotation amount normalized over distance
    uv += mul_vec2_mat(uv, rotz(((f - 0.5) / l) * smoothstep(-0.2, 0.4, _uv.y) * 0.45));
    
    // flame thickness
    float flame = 1.3 - length(uv.x) * 5.0;
    
    // bottom of flame 
    float blueflame = pow(flame * .9, 15.0);
    blueflame *= smoothstep(.2, -1.0, _uv.y);
    blueflame /= abs(uv.x * 2.0);
    blueflame = clamp(blueflame, 0.0, 1.0);
    
    // flame
    flame *= smoothstep(1., variationH * 0.5, _uv.y);
	flame = clamp(flame, 0.0, 1.0);
    flame = pow(flame, 3.);
    flame /= smoothstep(1.1, -0.1, _uv.y);    
	
    // colors
    float4 col = lerp(float4(1.0, 1., 0.0, 0.0), float4(1.0, 1.0, .6, 0.0), flame);
    col = lerp(float4(1.0, .0, 0.0, 0.0), col, smoothstep(0.0, 1.6, flame));
    pin.color = col;
    
	// a bit blueness on the bottom
    float4 bluecolor = lerp(float4(0.0, 0.0, 1.0, 0.0), pin.color, 0.95);
    pin.color = lerp(pin.color, bluecolor, blueflame);
    
    // clear bg outside of the flame
    pin.color *= flame;
    pin.color.a = flame;
    
    // bg halo
    float haloSize = 0.5;
    float centerL = 1.0 - (length(centerUV + float2(0.0, 0.1)) / haloSize);
    float4 halo = float4(.8, .3, .3, 0.0) * 1.0 * fbm(float2(time * 0.035, time * 0.035)) * centerL + 0.02;
    float4 finalCol = lerp(halo, pin.color, pin.color.a);
    pin.color = finalCol;

    // just a hint of noise
    pin.color *= lerp(rand(uv) + rand(uv * .45), 1.0, 0.9);
    pin.color = clamp(pin.color, 0.0, 1.0);
    
    return pin.color;

}