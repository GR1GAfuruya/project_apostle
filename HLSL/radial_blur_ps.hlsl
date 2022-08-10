#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2 
#define BORDER_ZERO 3
SamplerState sampler_states[4] : register(s0);
Texture2D texture_maps : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{

    float color[10];
    float2 dir = origin - pin.texcoord; //���S�_���猻�݂̍��W�܂ł̕����x�N�g�����o�� ���ꂪ�u���[���������΂������ɂȂ�
    //float len = length(dir); //�x�N�g���̋������o��
    dir = normalize(dir) * ray_power; //�����x�N�g���𐳋K��������A�����̋����������Č����̒����𒲐�

    float w = 9.0 / 11.0; //???
    //���������Ȃ���L�΂�
    color[0] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord) * 0.20; //�����ɋ߂��Ƃ���
    color[1] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.1f) * 0.18;
    color[2] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.2f) * 0.16;
    color[3] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.3f) * 0.14;
    color[4] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.4f) * 0.12;
    color[5] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.5f) * 0.10;
    color[6] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.6f) * 0.08;
    color[7] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.7f) * 0.06;
    color[8] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.8f) * 0.04;
    color[9] = texture_maps.Sample(sampler_states[BORDER_ZERO], pin.texcoord + dir * 0.9f) * 0.02; //�������牓���Ƃ���

    return  w * (color[0] + color[1] + color[2] + color[3] + color[4] + color[5] + color[6] + color[7] + color[8] + color[9]);

}