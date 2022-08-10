/*!
 * @brief �f�B�Y�j�[�x�[�X�̕����x�[�X�V�F�[�_
 */
#include "default_mesh.hlsli"
#include "constants.hlsli"


///////////////////////////////////////////////////
// �O���[�o���ϐ�
///////////////////////////////////////////////////

// �A���x�h�}�b�v
Texture2D<float4> albedo_map : register(t0);

// �@���}�b�v
Texture2D<float4> normal_map : register(t1);

// ���^���b�N�X���[�X�}�b�v�Br�Ƀ��^���b�N�Aa�ɃX���[�X
Texture2D<float4> metallic_map : register(t2);

Texture2D<float4> smooth_map : register(t3);

Texture2D<float4> ambient_oclusion_map : register(t4);

Texture2D<float4> emissives_map : register(t5);


// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);


struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 position : SV_TARGET3;
    float4 metallic_smooth : SV_TARGET4;
    float4 emissive : SV_TARGET5;
};
///////////////////////////////////////////////////
// �֐�
///////////////////////////////////////////////////

float3 get_normal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = normal_map.SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

    return newNormal;
}

/// <summary>
/// �s�N�Z���V�F�[�_�[
/// </summary>
PS_OUT main(VS_OUT pin) : SV_TARGET
{
    PS_OUT output;
    // �@�����v�Z
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    float3 normal = get_normal(N, T, B, pin.texcoord);
    // step-2 �e��}�b�v���T���v�����O����
    // �A���x�h�J���[�i�g�U���ˌ��j
    float4 albedo_color = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
   
    // �X�y�L�����J���[�̓A���x�h�J���[�Ɠ����ɂ���
    float3 specColor = albedo_color.rgb;

    // �����x
    float metallic = metallic_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
   
    // ���炩��
    float smooth = smooth_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).g;

    float4 ao_map = ambient_oclusion_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    float4 emissive_map = emissives_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // �����Ɍ������ĐL�т�x�N�g�����v�Z����
    float3 toEye = normalize(camera_position - pin.world_position);


    
    float3 V = camera_position.xyz - pin.world_position.xyz;
    float dist = length(V);

    //MRT�o��
    albedo_color = float4(albedo_color.xyz, 1);
    output.color = albedo_color * pin.color;
    output.depth = float4(dist, 0, 0, 1);
    output.normal = float4(normal, 1);
    output.position = float4(pin.world_position.xyz, 1);
    output.metallic_smooth = float4(0,0,0,1);
    output.metallic_smooth.r = metallic;
    output.metallic_smooth.g = smooth;
    output.emissive = emissive_map;
    return output;
}

