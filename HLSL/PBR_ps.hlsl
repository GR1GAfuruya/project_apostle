/*!
 * @brief �f�B�Y�j�[�x�[�X�̕����x�[�X�V�F�[�_
 */
#include "default_mesh.hlsli"
#include "constants.hlsli"
#include  "PBR_function.hlsli"

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

Texture2D<float4> emissive_map : register(t5);


// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);


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
float4 main(VS_OUT pin) : SV_TARGET
{
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
    float3 specColor = albedo_color;

    // �����x
    float metallic = metallic_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // ���炩��
    float smooth = smooth_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    float4 ao_map = ambient_oclusion_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    float4 emissive = emissive_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // �����Ɍ������ĐL�т�x�N�g�����v�Z����
    float3 toEye = normalize(camera_position - pin.world_position);


    // �V���v���ȃf�B�Y�j�[�x�[�X�̊g�U���˂���������
     // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
    float diffuseFromFresnel = calc_diffuse_from_fresnel(normal, -light_direction.xyz * 2.0f, toEye);
    float NdotL = saturate(dot(normal, -light_direction.xyz * 2.0f));
    float3 lambertDiffuse = float4(1, 1, 1, 1) * NdotL / PI;
    float3 diffuse = albedo_color.rgb * diffuseFromFresnel * lambertDiffuse;
    // Cook-Torrance���f���𗘗p�������ʔ��˗����v�Z����
    float3 spec = cook_torrance_specular(-light_direction.xyz * 2.0f, toEye, normal, smooth) * float4(1, 1, 1, 1);
    spec *= lerp(float3(1, 1, 1), albedo_color.rgb, metallic);
   
    float3 lig = diffuse * (1.0f - smooth) + spec * 2;
	// ���炩����������΁A�g�U���˂͎キ�Ȃ�
	lig += diffuse * (1.0f - smooth) + spec;

    // �����ɂ���グ
   // lig += light_color * albedo_color;
   
    float4 finalColor = 1.0f;
    finalColor.xyz = lig;

    // emissive
    float3 emi = emissive.r * emissive.w * emissive.rgb;
    finalColor.xyz += emi;

    return finalColor;
}
