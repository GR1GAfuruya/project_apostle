#include "instancing_mesh.hlsli"
#include "constants.hlsli"
#include "math.hlsli"

VS_OUT main(InstancedVertex In)
{
    VS_OUT Out;
    
    float4x4 world, C, transform;                     
    
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

       
    return Out;
}
