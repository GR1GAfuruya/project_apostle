static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};

cbuffer BONE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 bone_transforms[MAX_BONES]; //ボーン行列
};

cbuffer SHADOW_CONSTANT : register(b10)
{
    row_major float4x4 ShadowVP; // 光の位置カメラ*投映

};


struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 bone_weights : WEIGHTS;
    uint4 bone_indices : BONES;

};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 ShadowParam : TEXCOORD1;
};

