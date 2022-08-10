
static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 ka;  //�����@�A���r�G���g
    float4 kd;  //���s���@�f�B�q���[�Y
    float4 ks;  //���ʔ��� �X�y�L�����[
    float4 material_color;
    row_major float4x4 bone_transforms[MAX_BONES]; //�{�[���s��
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
};

cbuffer LIGHT_CONSTANT_BUFFER : register(b2)
{
    float4 ambient_color;
    float4 directional_light_direction;
    float4 directional_light_color;
};

#include "shading_functions.hlsli"
