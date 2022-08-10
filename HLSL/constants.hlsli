
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


cbuffer GRASS_CONSTANTS : register(b4)
{
    float grass_height_factor;
    float grass_width_factor;
    float grass_curvature;
    float grass_withered_factor;
    float grass_height_variance;
    float perlin_noise_distribution_factor;
    float tesselation_max_subdivision;
    float tesselation_max_distance;
    float4 grass_specular_color;
};




cbuffer POST_EFFECTS_CONSTANT : register(b5)
{
    //LUMINA
    float4 l_min;
    float4 l_max;
    float4 lumina;
    //BLUR
    float2 c_sigma;
    float bloom_extraction_threshold;
    float blur_convolution_intensity;
   // float4 weight[2]; // �d��
    //RADIAL_BLUR
    float2 origin; //���S�_
    float ray_power; //�u���[��������p���[ :�����x�N�g���ɂ����Č����̒��������߂�
    float wipe_threshold;
      // �F������
    float hueShift;
    // �ʓx����
    float saturation;
    // ���x����
    float brightness;
}

#define CELL_COUNT 5 

cbuffer DEFFERD_LIGHT_CONSTANT : register(b7)
{
	//���s���@xyz������ w = 0.0
	//�_�����@xyz���ʒu w = ��������
    float4 defferd_light_dir;
    float4 defferd_light_color;
}
