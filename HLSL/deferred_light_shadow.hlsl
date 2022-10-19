#include "fullscreen_quad.hlsli"
#include "constants.hlsli"

// �V���h�E�}�b�v�p�R���X�^���g�o�b�t�@
cbuffer CBShadowMap : register(b10)
{
	row_major float4x4  ShadowVP; // ���̈ʒu�J����*���f
}

// �V�[���e�N�X�`��
Texture2D sceneTexture : register(t0);
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);
Texture2D metal_smooth_map : register(t3);
Texture2D emissive_map : register(t4);

Texture2D shadow_map : register(t16);

// �V�[���T���v���X�e�[�g
SamplerState sceneSampler : register(s5);
SamplerState BorderSampler : register(s6);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �J���[�̎擾
	float4 tex = sceneTexture.Sample(sceneSampler, pin.texcoord);
	// �_��������
    float4 P = position_map.Sample(sceneSampler, pin.texcoord);
    float3 PLightPos = defferd_light_dir.xyz;
	float3 PLightDir = P.xyz - PLightPos;
	// ���C�g���
    float type = step(0.001, defferd_light_dir.w);
	// �����̐ݒ�
    float attenuation = type < 0.1 ? 1.0 : 1.0 - (length(PLightDir) / defferd_light_dir.w);
	attenuation = saturate(attenuation);
	// ���s��
    float3 L = lerp(defferd_light_dir.xyz, PLightDir, type);

	float3 N = tex.xyz * 2.0 - 1.0;
    float3 C = defferd_light_color.rgb;
	C *= attenuation; //����

	L = normalize(L);
	N = normalize(N);
	float d = -dot(N, L); //= -cos
	d = saturate(d); // 0�`1�ɃN���b�s���O
	tex.rgb = C * d; //���C�g����

	// Deferred Shadow
	// �����猩��XYZ W
	float4 shadowparam = mul(P, ShadowVP);
	float d2 = saturate(shadowparam.z / shadowparam.w);

	//ShadowMap
	float2 uv = shadowparam.xy;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;//-1<--->+1��0<--->1

    float2 dist = shadow_map.Sample(BorderSampler, uv).xy;
/*
	// �`�F�r�V�F�t�̕s�����𗘗p�������U�V���h�E�}�b�v
	float depth_sq = dist.x * dist.x;
	float variance = dist.y - depth_sq;
	float md = d2 - dist.x;
	float p = variance / (variance + (md * md));
	float shadow = p;// saturate(max(p, dist.x >= d2 - 0.001));
*/
	float shadow = dist.x > d2 - 0.003;
    tex.rgb *= shadow;
	tex.a = 1;

	return tex;
}

