#include "instancing_mesh.hlsli"
#include "constants.hlsli"
#include "math.hlsli"

VS_OUT main(InstancedVertex In)
{
    float sigma = In.tangent.w;
    VS_OUT Out;
    
#if 1
    
    float4x4 world, C, transform, scalling, quaternion;                     
    
    //RHS-Y-UP
    C = float4x4(
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 );

   
    world = mul(global_transform, mul(C, calc_world_transform(In.InstScale, In.InstRotation, In.InstPosition)));
    
    Out.position = mul(float4(In.position, 1), mul(world, view_projection));
    Out.world_position = mul(float4(In.position, 1), world);
    Out.world_normal = normalize(mul(float4(In.normal, 0), world));
    Out.world_tangent = normalize(mul(In.tangent, world));
    Out.texcoord = In.texcoord;
    Out.color = float4(1, 1, 1, 1);

       
#else

    // Scale.

    // Rotate vertex position and normal based on instance quaternion...
   // position = RotateVectorByQuaternion(In.InstRotation, position);
    float3 normal = RotateVectorByQuaternion(In.InstRotation, In.normal);

    // Move to world space.
    
    //RHS-Y-UP
    float4x4 C =
    {
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1

    };

    float3 scale = In.InstScale;
    float4x4 S =
    {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    };
    
    float4 q = In.InstRotation;
    
    float3x3 r =
    {
        1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
        2.0f * q.x * q.y + 2.0f * q.w * q.z,
        2.0f * q.x * q.z - 2.0f * q.w * q.y,
        
        2.0f * q.x * q.y - 2.0f * q.w * q.z,
        1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
        2.0f * q.y * q.z + 2.0f * q.w * q.x,
        
        2.0f * q.x * q.z + 2.0f * q.w * q.y,
        2.0f * q.y * q.z - 2.0f * q.w * q.x,
        1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y,
    };
    
    float4x4 R =
    {
        r._11, r._12, r._13, 0,
        r._21, r._22, r._23, 0,
        r._31, r._32, r._33, 0,
        0, 0, 0, 1
    };
    //ポジション行列算出
    float3 t = In.InstPosition;
    
    float4x4 T =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t.x, t.y, t.z, 1
    };
    //トランスフォーム
    //float4x4 transform = mul(mul(mul(C, S), R), T);
    float4x4 transform = mul(mul(S, R), T);
    float4x4 W = mul(global_transform, transform);
    
    // ...and clip.
    Out.position = mul(float4(In.position, 1), mul(W, view_projection));

    // World space transform
    Out.world_position = mul(float4(In.position, 1), W);
    
    // Finally, output  normal and color
    Out.world_normal = float4(normal, 0);
    Out.texcoord = In.texcoord;
    Out.color = float4(1, 1, 1, 1);
    Out.world_tangent = float4(In.tangent.xyz, 0.0f);
    Out.world_tangent.w = float4(In.tangent.xyz, 0.0f);
#endif
    return Out;
}
