#include "constants.hlsli"
#include "mesh_effect.hlsli"
VS_OUT main(VS_IN vin)
{
    float sigma = vin.tangent.w;
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));
 
    vout.world_position = mul(vin.position, world);
    vout.world_normal = normalize(mul(vin.normal, world));
    vout.world_tangent = normalize(mul(vin.tangent, world));
    vout.world_tangent.w = sigma;
    vout.texcoord = vin.texcoord + scroll_direction * scroll_speed * time;
    return vout;
}
