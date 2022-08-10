//éQçlhttp://marupeke296.com/DX10_No6_GeometryShader.html


struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct GS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 Ka;
    float4 Kd;
    float4 Ks;
    float scale;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float4 light_direction;
    float4 camera_position;
}
