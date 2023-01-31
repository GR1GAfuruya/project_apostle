#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#include "PBR_function.hlsli"
#include "shading_functions.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
//	�e�N�X�`��
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);

// �s�N�Z���V�F�[�_�[
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex = normal_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord); // *input.Color;

	//0624 �_����
    float3 PLightPos = defferd_light_dir.xyz;
    float4 P = position_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 PLightDir = P.xyz - PLightPos;
	// ���C�g�^�C�v����(0:���s�� 1:�_����)
    float lighttype = step(0.01, defferd_light_dir.w);
	// ��������(���s��:w=0.0 �_����:w>0)
	//float3 dir = LightDir.w < 0.001 ? LightDir.xyz : PLightDir;
    float3 dir = lerp(
	  defferd_light_dir.xyz, PLightDir, lighttype);
	//����
    float attenuation = defferd_light_dir.w < 0.001 ?
		1.0 :
		1.0 - (length(PLightDir) / defferd_light_dir.w);
    attenuation = saturate(attenuation); //0.0<--->1.0
	// ���C�g�v�Z
    float3 L = dir; //����
    float3 C = defferd_light_color.rgb; //�J���[
    C *= attenuation; // ����
    float3 N = tex * 2.0 - 1.0; //�@��
    L = normalize(L);
    N = normalize(N);
	// ������ = -cos = -����
    float i = -dot(N, L);
    i = saturate(i); // 0.0�`1.0
    tex.rgb = C * i; // ���C�g�J���[����
    return tex;
}

