#include "aura.hlsli"
#include "constants.hlsli"
VS_OUT main(VS_IN vin)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (vin.bone_weights[i] * mul(vin.position, bone_transforms[vin.bone_indices[i]])).xyz;
        n += (vin.bone_weights[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices[i]])).xyz;
    }
    n = normalize(n);

    VS_OUT vout;
	//	–@ü•ûŒü‚ÉŠg‘å
    vout.position = mul(float4(p + (n * 3.8), 1.0f), mul(world, view_projection));
    vout.texcoord = vin.texcoord;
    vout.meta_position =float4(vout.position.xyz + (n * 3.8), 1.0f);
    return vout;
}
