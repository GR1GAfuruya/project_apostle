#include "constants.hlsli"
#include "shadow_map.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    float4 blended_position = { 0, 0, 0, 1 };
    float4 blended_normal = { 0, 0, 0, 0 };
    float4 blended_tangent = { 0, 0, 0, 0 };
    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        blended_position += vin.bone_weights[bone_index]
         * mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]); //ウェイトにポジションをかける
        blended_normal += vin.bone_weights[bone_index]
         * mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
        blended_tangent += vin.bone_weights[bone_index] *
    mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);

    }
    vin.position = float4(blended_position.xyz, 1.0f);
    vin.normal = float4(blended_normal.xyz, 0.0f);
    vin.tangent = float4(blended_tangent.xyz, 0.0f);
    
    VS_OUT vout;
	// 光カメラで座標変換
    vout.position = mul(vin.position, mul(world, ShadowVP));
   // vout.position = mul(vin.position, ShadowVP);
    vout.texcoord = vin.texcoord;
    vout.ShadowParam = vout.position;
 
 
    return vout;
}