
cbuffer MATERIAL_CONSTANTS : register(b2)
{
    float2 texcoord_offset;
    float2 texcoord_scale;
    float emissive_power;
};
cbuffer SCENE_CONSTANT : register(b3)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_projection;
    float4 light_color;
    float4 light_direction;
    float4 camera_position;
    float4 avatar_position;
    float4 avatar_direction;
    float2 resolution;
    float time;
    float delta_time;
};






cbuffer POST_EFFECTS_CONSTANT : register(b5)
{
    //BLOOM
    float2 c_sigma;
    float bloom_extraction_threshold;
    float blur_convolution_intensity;
    //���W�A���u���[
    float radial_power;
    //�V�[���J��
    float2 scsene_threshold;
    float pad;
    // �F������
    float hueShift;
    // �ʓx����
    float saturation;
    // ���x����
    float brightness;
    //�R���g���X�g
    float contrast;
    //�r�l�b�g
    float falloff;
    float amount;
    float3 vignette_color;

}


cbuffer DEFFERD_LIGHT_CONSTANT : register(b7)
{
	//���s���@xyz������ w = 0.0
	//�_�����@xyz���ʒu w = ��������
    float4 defferd_light_dir;
    float4 defferd_light_color;
}
