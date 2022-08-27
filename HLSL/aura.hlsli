static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};

cbuffer BONE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 bone_transforms[MAX_BONES]; //É{Å[ÉìçsóÒ
};

cbuffer AURA_CONSTANT_BUFFER : register(b9)
{
   float4 particle_color;
   float2 scroll_direction;
   float scroll_speed;
   float thirethold;
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
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

