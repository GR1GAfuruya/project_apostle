#include "constants.hlsli"
#include "gaussian_blur.hlsli"
// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1h
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

Texture2D<float4> scene_texture : register(t0); // �V�[���e�N�X�`��
float4 main(VS_OUT pin) : SV_TARGET
{
	 // X,Y�u���[�p�̃s�N�Z���V�F�[�_�[������
    float4 Color;
    return 1;
    // ��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //Color = weight[0].x * scene_texture.Sample(sampler_states[POINT], pin.tex0.xy);
    //Color += weight[0].y * scene_texture.Sample(sampler_states[POINT], pin.tex1.xy);
    //Color += weight[0].z * scene_texture.Sample(sampler_states[POINT], pin.tex2.xy);
    //Color += weight[0].w * scene_texture.Sample(sampler_states[POINT], pin.tex3.xy);
    //Color += weight[1].x * scene_texture.Sample(sampler_states[POINT], pin.tex4.xy);
    //Color += weight[1].y * scene_texture.Sample(sampler_states[POINT], pin.tex5.xy);
    //Color += weight[1].z * scene_texture.Sample(sampler_states[POINT], pin.tex6.xy);
    //Color += weight[1].w * scene_texture.Sample(sampler_states[POINT], pin.tex7.xy);

    //// ��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //Color += weight[0].x * scene_texture.Sample(sampler_states[POINT], pin.tex0.zw);
    //Color += weight[0].y * scene_texture.Sample(sampler_states[POINT], pin.tex1.zw);
    //Color += weight[0].z * scene_texture.Sample(sampler_states[POINT], pin.tex2.zw);
    //Color += weight[0].w * scene_texture.Sample(sampler_states[POINT], pin.tex3.zw);
    //Color += weight[1].x * scene_texture.Sample(sampler_states[POINT], pin.tex4.zw);
    //Color += weight[1].y * scene_texture.Sample(sampler_states[POINT], pin.tex5.zw);
    //Color += weight[1].z * scene_texture.Sample(sampler_states[POINT], pin.tex6.zw);
    //Color += weight[1].w * scene_texture.Sample(sampler_states[POINT], pin.tex7.zw);

    //return float4(Color.xyz, 1.0f);
}