#include "default_mesh.hlsli"
#include "constants.hlsli"
#include "shading_functions.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 position : SV_TARGET3;
};

PS_OUT main(VS_OUT pin)
{
    PS_OUT output;
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    
    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

    float3 V = camera_position.xyz - pin.world_position.xyz;
    float dist = length(V);

    //MRTèoóÕ
    color = float4(color.xyz, alpha);
    output.color = color * pin.color;
    output.depth = float4(dist, 0, 0, 1);
    output.normal = float4(N, 1);
    output.position = float4(pin.world_position.xyz, 1);
    return output;
}