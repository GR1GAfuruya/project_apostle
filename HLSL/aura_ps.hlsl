
Texture2D color_map : register(t0);
SamplerState pointSamplerState : register(s0); //https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-sample
SamplerState linerSamplerState : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);
#if 0
float4 main(VS_OUT pin) : SV_TARGET    //戻り値のセマンティック   SVはシステムバリュー      //https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-sample
{
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}
#endif
struct VS_OUT
{
    float4 position : SV_POSITION; //vertexShaderは必ずこのセマンティックの値を出力しなければならない！！！！！   （値 = 頂点のスクリーン上の位置（ＮＤＣ）
    float4 color : COLOR;
    float2 texcoord : TEXCOORD; //UV座標
};

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(1, 1, 0, 1);
}
//#include "aura.hlsli"
//#include "constants.hlsli"
//#define POINT 0
//#define LINEAR 1
//#define ANISOTROPIC 2
//SamplerState sampler_states[3] : register(s0);
//Texture2D texture_maps : register(t5);


//float Hack(float2 p, in float s)
//{
//    float3 p2 = float3(p.xy, 17.3 * abs(sin(s)));
//    return frac(sin(dot(p2, float3(17.3, 61.7, 12.4))) * 173713.1);
//}


//float HackNoise(in float2 p, in float s)
//{
//    float2 i = floor(p);
//    float2 f = frac(p);
//    f *= f * (3.0 - 2.0 * f);
    
    
//    return lerp(lerp(Hack(i + float2(0., 0.), s), Hack(i + float2(1., 0.), s), f.x),
//               lerp(Hack(i + float2(0., 1.), s), Hack(i + float2(1., 1.), s), f.x),
//               f.y) * s;
//}


//float Thunder(float2 p, float3 spec)
//{
//    float v = -HackNoise(p * 2.0, 0.25);
//    v += HackNoise(p * 1.1, 0.5) - HackNoise(p * 1.1, 0.25);
//    v += HackNoise(p * 2.1, 0.25) - HackNoise(p * 2.1, 0.125);
//    v += HackNoise(p * 4.1, 0.125) * spec.x * 1. - HackNoise(p * 08.1, 0.0625);
//    v += HackNoise(p * 8.1, 0.0625) - HackNoise(p * 16., 0.03125) * spec.y * 1.;
//    v += HackNoise(p * 16.1, 0.03125);
//    return v * 1.2;

//}



//float4 main(VS_OUT pin) : SV_TARGET
//{

//    const float n = 1.1;
//    const float s = 1.3;
//    float WhoMadeWho = (time * s) + 1000.0;
//    float2 p = pin.texcoord.xy / reflection.yy;
//    float2 q = pin.texcoord.xy / reflection.yy;
    
//    int tx = int(p.x * 128.0);
    
//    float2 uv = (pin.texcoord.xy / resolution.xy) * 2.0 - 1.0;
//   // uv.y *= resolution.y / resolution.x;
    
    
//    float3 finalColor = float3(0.0, 0.0, 0.0);

//    for (float i = 1.0; i < n; i++)
//    {
//        float t = abs(4.0 / ((uv.x + Thunder(pin.texcoord + WhoMadeWho / i, float3(1,1,0))) * (i * 100.0)));
//        finalColor += t * float3(0, 0.2, 1.30);
//    }

//    return float4(finalColor, 1.0);
    
    
//}
//float f(float2 v)
//{
//    return 1.0 / (v.x * v.x + v.y * v.y + .0001);
//}

//float4 metaball(float2 st)
//{
//    float2 ires = { 1280, 720 };
//    st.x *= ires.x;
//    st.y *= ires.y;

//    float scale = 2300;
//    float d = 0;

  
//    d += f(st - mouse_pos.xy);
//    d += f(st - float2(640, 360));
    

//    d *= scale;
//    d = abs(d - 0.5);

//    float3 color = 1;
//   color = lerp(color, float3(0, 1, 0), smoothstep(d - 0.08, d - 0.08 + 0.002, 0));
//    color = lerp(color, float3(1, 0, 0), smoothstep(d - 0.02, d - 0.02 + 0.002, 0));
//    return float4(color, 1);
//}
//float4 main(VS_OUT pin) : SV_TARGET
//{
//    float4 color;
//	//color =  metaball(pin.texcoord);
//    float2 uv = pin.texcoord;
//    uv.y += time;
//    color = texture_maps.Sample(sampler_states[ANISOTROPIC], uv);
//    return float4(color.r,0,0, 1.0);
     
//}
