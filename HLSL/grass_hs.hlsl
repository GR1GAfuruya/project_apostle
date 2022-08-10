#include "grass.hlsli"
#include "constants.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float tess_factor[3] : SV_TessFactor;
    float inside_tess_factor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

//�p�b�`���ƂɃe�b�Z���[�V�����W���̊���
//���݂̃p�b�`�łǂ̒��x���_�����������邩�Ƃ����ʂ����蓖�Ă�
HS_CONSTANT_DATA_OUTPUT patch_constant_function(InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> input_patch, uint patch_id : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
#define ENABLE_LOD
#ifdef ENABLE_LOD
    //3�p�`�̏d�S�����߂�
    float3 midpoint = (input_patch[0].world_position.xyz + input_patch[1].world_position.xyz + input_patch[2].world_position.xyz) / 3.0;
    //�L�����̈ʒu����̋���
    const float distance = length(avatar_position.xyz - midpoint);
    // saturate :�l��[0, 1] �͈̔͂ɃN�����v���܂��B
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


//�R���g���[���|�C���g�̊���
//�p�b�`�̃R���g���[���|�C���g��ǂݎ��1�̏o�̓R���g���[���|�C���g�𐶐�����

[domain("tri")] //�h���C���̎w��B(tri/ quad/isoline)����I���B
[partitioning("integer")] //�������@�B(integer/fractional_eve,fractional_odd/pow2)����I���B
[outputtopology("triangle_cw")] //�o�͂��ꂽ���_���`������g�|���W�[�B(point/line/triangle_cw/triangle_ccw)����I���B

[outputcontrolpoints(3)] //�o�͂����R���g���[���|�C���g��(���͂ƈ���Ă��ǂ�)�B
[patchconstantfunc("patch_constant_function")] //Patch-Constant-Function���w�肷��B
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> input_patch,
	uint output_control_point_id : SV_OutputControlPointID,
	uint primitive_id : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output = input_patch[output_control_point_id];
    return output;
}

