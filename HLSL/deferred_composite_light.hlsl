#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
// �V���h�E�}�b�v�p�R���X�^���g�o�b�t�@
cbuffer CBShadowMap : register(b10)
{
    row_major float4x4 ShadowVP; // ���̈ʒu�J����*���f
}

//	�e�N�X�`��
Texture2D albedo_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);
Texture2D metal_smooth_map : register(t3);
Texture2D light_map : register(t5);
Texture2D shadow_map : register(t16);


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
SamplerState BorderSampler : register(s6);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = tex;
    float4 light = light_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 P = position_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    //�V���h�E�}�b�v
    float4 shadowparam = mul(P, ShadowVP);
    float d2 = saturate(shadowparam.z / shadowparam.w);

    float2 uv = shadowparam.xy;
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5; //-1<--->+1��0<--->1
    float2 dist = shadow_map.Sample(BorderSampler, uv).xy;
    float shadow = dist.x > d2 - 0.003;
    color.rgb *= min(shadow + 0.6, 1);
    color.a = 1;

    //���^���b�N
    float metallic = metal_smooth_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    float3 m1 = color.rgb * light.rgb;//���^���b�N��������
    float3 m0 = color.rgb * 0.4;//���^���b�N���Ȃ����
    m0 += light.rgb * 0.05;
    color.rgb = lerp(m0, m1, metallic);
    
    //�t���l������
    //����
    float3 E = P.xyz - camera_position.xyz;
    E = normalize(E);
    //�@��
    float3 N = normal_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float NoLight = 1.0 - length(N);
    NoLight = step(0.9, NoLight);
    N = N * 2.0 - 1.0; //0<-->1 -1<-->+1
    N = normalize(N);
	// ���ςŎ����Ɩ@���̌��������
    float d = dot(-E, N);
    float fresnel = saturate(1.0 - d) * 0.6;
    fresnel = pow(fresnel, 6.0);
    color.rgb = lerp(color.rgb,
				light.rgb, fresnel);
    
    return color;

}