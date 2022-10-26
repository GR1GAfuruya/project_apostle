struct InstancedVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 InstRotation : I_ROTATION; // Orientation quaternion (per instance)
    float3 InstPosition : I_POSITION; // Orientation quaternion (per instance)
    float3 InstScale : I_SCALE; // Position and scale (per instance)
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

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 global_transform;
};

float3 RotateVectorByQuaternion(float4 Q, float3 V)
{
    return V + 2.0f * cross(Q.xyz, cross(Q.xyz, V) + Q.w * V);
}