#include "mesh_effect.hlsli"
#include "constants.hlsli"
#include "noise.hlsli"
Texture2D texture_maps[4] : register(t20);
SamplerState sampler_border_black : register(s0);
//start hash
//HLSLでのハッシュでのノイズがわからないので今は使わない
#if 0
uint3 k = uint3(0x456789abu, 0x6789ab45u, 0x89ab4567u);
uint3 u = uint3(1, 2, 3);
const uint UINT_MAX = 0xffffffffu;
uint uhash11(uint n)
{
    n ^= (n << u.x);
    n ^= (n >> u.x);
    n *= k.x;
    n ^= (n << u.x);
    return n * k.x;
}
uint2 uhash22(uint2 n)
{
    n ^= (n.yx << u.xy);
    n ^= (n.yx >> u.xy);
    n *= k.xy;
    n ^= (n.yx << u.xy);
    return n * k.xy;
}
uint3 uhash33(uint3 n)
{
    n ^= (n.yzx << u);
    n ^= (n.yzx >> u);
    n *= k;
    n ^= (n.yzx << u);
    return n * k;
}
float hash11(float p)
{   
//GLSLではuvec2 n = floatBitsToUint(p);
    uint n = asuint(p);
    return float(uhash11(n)) / float(UINT_MAX);
}
float hash21(float2 p)
{
    uint2 n = asuint(p);
    return float(uhash22(n).x) / float(UINT_MAX);
}
float hash31(float3 p)
{
    uint3 n = asuint(p);
    return float(uhash33(n).x) / float(UINT_MAX);
}
float2 hash22(float2 p)
{
    uint2 n = asuint(p);
    return float2(uhash22(n)) / float2(UINT_MAX, UINT_MAX);
}
float3 hash33(float3 p)
{
    uint3 n = asuint(p);
    return float3(uhash33(n)) / float3(UINT_MAX, UINT_MAX, UINT_MAX);
}

float fdist21(float2 p)
{
    float2 n = floor(p + 0.5);
    float dist = sqrt(2.0);
    for(float j = 0.0; j <= 2.0; j ++ ){
        float2 glid;
        glid.y = n.y + sign(fmod(j, 2.0) - 0.5) * ceil(j * 0.5);
        if (abs(glid.y - p.y) - 0.5 > dist){
            continue;
        }
        for(float i = -1.0; i <= 1.0; i ++ ){
            glid.x = n.x + i;
            float2 jitter = hash22(glid) - 0.5;
            dist = min(dist, length(glid + jitter - p));
        }
    }
    return dist;
}
float fdist31(float3 p)
{
    float3 n = floor(p + 0.5);
    float dist = sqrt(3.0);
    for(float k = 0.0; k <= 2.0; k ++ ){
        float3 glid;
            glid.z = n.z + sign(fmod(k, 2.0) - 0.5) * ceil(k * 0.5);
            if (abs(glid.z - p.z) - 0.5 > dist){
                continue;
            }
        for(float j = 0.0; j <= 2.0; j ++ ){
            glid.y = n.y + sign(fmod(j, 2.0) - 0.5) * ceil(j * 0.5);
            if (abs(glid.y - p.y) - 0.5 > dist){
                continue;
            }
            for(float i = -1.0; i <= 1.0; i ++ ){
                glid.x = n.x + i;
                float3 jitter = hash33(glid) - 0.5;
                dist = min(dist, length(glid + jitter - p));
            }
        }
    }
    return dist;
}
#else
//end hash
float fdist21(float2 p)
{
    float2 n = floor(p + 0.5);//最も近い格子点
    float dist = sqrt(2.0);//第一近傍距離の上限
    for (float j = 0.0; j <= 2.0; j++)
    {
        float2 glid;//近くの格子点
        glid.y = n.y + sign(fmod(j, 2.0) - 0.5) * ceil(j * 0.5);
        if (abs(glid.y - p.y) - 0.5 > dist)
        {
            continue;
        }
        for (float i = -1.0; i <= 1.0; i++)
        {
            glid.x = n.x + i;
            float2 jitter = random(glid) - 0.5;
            dist = min(dist, length(glid + jitter - p));
        }
    }
    return dist;
}


//3次元の場合
float fdist31(float3 p)
{
    float3 n = floor(p + 0.5);
    float dist = sqrt(3.0);
    for (float k = 0.0; k <= 2.0; k++)
    {
        float3 glid;
        glid.z = n.z + sign(fmod(k, 2.0) - 0.5) * ceil(k * 0.5);
        if (abs(glid.z - p.z) - 0.5 > dist)
        {
            continue;
        }
        for (float j = 0.0; j <= 2.0; j++)
        {
            glid.y = n.y + sign(fmod(j, 2.0) - 0.5) * ceil(j * 0.5);
            if (abs(glid.y - p.y) - 0.5 > dist)
            {
                continue;
            }
            for (float i = -1.0; i <= 1.0; i++)
            {
                glid.x = n.x + i;
                float3 jitter = noise(glid) - 0.5;
                dist = min(dist, length(glid + jitter - p));
            }
        }
    }
    return dist;
}

#endif
float4 main(VS_OUT pin) : SV_TARGET
{
    float2 pos = pin.texcoord.yx;
    pos *= 30.0;
    pos += time;
    float4 noise_color;
    float4 noise_color2;
    float Voronoi_diagram = fdist31(float3(pos, time));
    float Voronoi_diagram2 = fdist31(float3(pos, -time));
    //胞体ノイズ
    noise_color = float4(Voronoi_diagram, Voronoi_diagram, 1 -Voronoi_diagram, Voronoi_diagram);
    noise_color2 = float4(Voronoi_diagram2, Voronoi_diagram2, Voronoi_diagram2, Voronoi_diagram2);
    noise_color.rgb *= noise_color2.rgb;
    float alpha = particle_color.a;
    alpha = step(threshold, alpha);
    //カラー出力
    float4 color = float4(particle_color.rgb * noise_color.rgb, alpha);
    return color;
}
