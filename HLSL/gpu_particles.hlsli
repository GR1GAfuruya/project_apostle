struct VS_OUT
{
    uint vertex_id : VERTEXID;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
struct particle
{
    float3 position;//位置
    float3 velocity;//速力
    float3 angle;//角度
    float3 scale;//大きさ
    float4 color;//色
    float streak_factor;//速度に対するスプライトの引き延ばし
    float time; //生成されてからの時間
    float life_time;//寿命
    bool is_active;//アクティブ常態か
};

struct Emitter
{
    float3 pos;
    float emit_life; // エミッターの寿命
    float emit_life_time; // エミッターの年齢
    float emit_time; //生成されてからの時間
    float emit_rate; // 1秒間に何発発生するか
    int emit_count; // 現在の発生カウント
};

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    Emitter emitter;

    float2 particle_size;
    uint particle_count;
    float3 angle;
};

#include "noise.hlsli"
//関数
// パーリンノイズによるベクトル場
// 3Dとして3要素を計算。
// それぞれのノイズは明らかに違う（極端に大きなオフセット）を持たせた値とする
float3 Pnoise(float3 vec)
{
    float x = snoise(vec);

    float y = snoise(float3(
        vec.y + 31.416,
        vec.z - 47.853,
        vec.x + 12.793
    ));

    float z = snoise(float3(
        vec.z - 233.145,
        vec.x - 113.408,
        vec.y - 185.31
    ));

    return float3(x, y, z);
}

// α = ramp(d(x)/d0)
// ψ_constrainted(x) = αψ(x) + (1 - α)n(n・ψ(x))
float3 SamplePotential(float3 pos, float time)
{
  //  float3 s = pos / _NoiseScales[0];
    float3 s = pos / 9;
    return Pnoise(s);
}
float3 CurlNoise(particle p)
{
    //const float e = 0.0009765625;
    const float e = 1e-4f;
    const float e2 = 2.0 * e;
    const float invE2 = 1.0 / e2;

    const float3 dx = float3(e, 0.0, 0.0);
    const float3 dy = float3(0.0, e, 0.0);
    const float3 dz = float3(0.0, 0.0, e);

    float3 pos = p.position;

    float3 p_x0 = SamplePotential(pos - dx, p.time);
    float3 p_x1 = SamplePotential(pos + dx, p.time);
    float3 p_y0 = SamplePotential(pos - dy, p.time);
    float3 p_y1 = SamplePotential(pos + dy, p.time);
    float3 p_z0 = SamplePotential(pos - dz, p.time);
    float3 p_z1 = SamplePotential(pos + dz, p.time);

    float x = (p_y1.z - p_y0.z) - (p_z1.y - p_z0.y);
    float y = (p_z1.x - p_z0.x) - (p_x1.z - p_x0.z);
    float z = (p_x1.y - p_x0.y) - (p_y1.x - p_y0.x);

    return float3(x, y, z) * invE2;
}
