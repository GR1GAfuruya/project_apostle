#include "grass.hlsli"
#include "constants.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float tess_factor[3] : SV_TessFactor;
    float inside_tess_factor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

//パッチごとにテッセレーション係数の割当
//現在のパッチでどの程度頂点分割をかけるかという量を割り当てる
HS_CONSTANT_DATA_OUTPUT patch_constant_function(InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> input_patch, uint patch_id : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
#define ENABLE_LOD
#ifdef ENABLE_LOD
    //3角形の重心を求める
    float3 midpoint = (input_patch[0].world_position.xyz + input_patch[1].world_position.xyz + input_patch[2].world_position.xyz) / 3.0;
    //キャラの位置からの距離
    const float distance = length(avatar_position.xyz - midpoint);
    // saturate :値を[0, 1] の範囲にクランプします。
    const float subdivision_factor = saturate((tesselation_max_distance - distance) / tesselation_max_distance);
    const float subdivision = tesselation_max_subdivision * subdivision_factor + 1.0;
#else
    const float division = tesselation_max_division;
#endif
    output.tess_factor[0] = subdivision;
    output.tess_factor[1] = subdivision;
    output.tess_factor[2] = subdivision;
    output.inside_tess_factor = subdivision;
    return output;
}


//コントロールポイントの割当
//パッチのコントロールポイントを読み取り1つの出力コントロールポイントを生成する

[domain("tri")] //ドメインの指定。(tri/ quad/isoline)から選択。
[partitioning("integer")] //分割方法。(integer/fractional_eve,fractional_odd/pow2)から選択。
[outputtopology("triangle_cw")] //出力された頂点が形成するトポロジー。(point/line/triangle_cw/triangle_ccw)から選択。

[outputcontrolpoints(3)] //出力されるコントロールポイント数(入力と違っても良い)。
[patchconstantfunc("patch_constant_function")] //Patch-Constant-Functionを指定する。
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> input_patch,
	uint output_control_point_id : SV_OutputControlPointID,
	uint primitive_id : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output = input_patch[output_control_point_id];
    return output;
}

