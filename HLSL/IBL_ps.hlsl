//�����x�[�X�����_�����O
//���쌠 (c) 2017-2018 Michal Siejak

//Physically Based�V�F�[�f�B���O���f���B
//Lambertian diffuse BRDF + Cook-Torrance microfact specular BRDF + �A���r�G���g�̂��߂�IBL
// See: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

#include "constants.hlsli"
#include "default_mesh.hlsli"
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

static const uint NumLights = 3;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

Texture2D albedo_map: register(t0);
Texture2D normal_map : register(t1);
Texture2D metalness_map : register(t2);
Texture2D roughness_map : register(t3);
TextureCube specular_map : register(t4);
TextureCube irradiance_map : register(t5);
Texture2D specularBRDF_LUT : register(t6);

// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

// GGX/Towbridge-Reitz ���K���z�֐�.
// Alpha = roughness^2 �� Disney �̍ăp�����g���b�N�����g�p.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// �����\��Schlick-GGX�̒P�ꍀ�B
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}


// �X�~�X�@�ɂ��􉽊w�I�����֐���Schlick-GGX�ߎ�.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// �V�����b�N�̃t���l���W���ߎ��B
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ���� IBL ���}�b�v�̃~�b�v�}�b�v���x������Ԃ��܂��B
uint querySpecularTextureLevels()
{
    uint width, height, levels;
    specular_map.GetDimensions(0, width, height, levels);
    return levels;
}

float3 get_normal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = normal_map.SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

    return newNormal;
}

float4 main(VS_OUT pin) : SV_Target
{
    // �V�F�[�f�B���O���f�����擾���邽�߂ɓ��̓e�N�X�`�����T���v������ params.
    float3 albedo = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
    float metalness = metalness_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    float roughness = roughness_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;

    // ���̏o�˕����i���E��Ԃ̃t���O�����g�ʒu���� "eye "�ւ̃x�N�g���j
    float3 Lo = normalize(camera_position - pin.position);

     // �@�����v�Z
    float3 normal = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(normal, T));
    float3 B = normalize(cross(normal, T) * sigma);
    float3 N = get_normal(normal, T, B, pin.texcoord);

    // �\�ʖ@���Əo�ˌ������Ƃ̊Ԃ̊p�x�D
    float cosLo = max(0.0, dot(normal, Lo));
		
	// ���ʔ��˃x�N�g��.
    float3 Lr = 2.0 * cosLo * N - Lo;

	// �������˂ɂ�����t���l�����˗��i�����̏ꍇ�̓A���x�h�J���[���g�p�j
    float3 F0 = lerp(Fdielectric, albedo, metalness);

    // ���͗p�Ɩ��̒��ڏƖ��v�Z
    float3 directLighting = 0.0;
    //���C�g�v�Z
    {
        float3 Li = -light_direction.xyz;
        float3 Lradiance = 1; //���s���Ȃ̂łP
    // Li��Lo�̊Ԃ̔��x�N�g���D
        float3 Lh = normalize(Li + Lo);

		// �T�[�t�F�X�@���Ɨl�X�ȃ��C�g�x�N�g���Ƃ̊Ԃ̊p�x���v�Z����
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));

		// ���ڏƖ��̃t���l�������v�Z����D
        float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// ����BRDF�̐��K���z���v�Z����
        float D = ndfGGX(cosLh, roughness);
		// ����BRDF�̊􉽊w�I�������v�Z����
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// �g�U�U���́A�����U�d�̂ɂ���ĉ��x�����܂���邱�Ƃɂ���ċN����B
		// ����A�����̓G�l���M�[�𔽎˂܂��͋z�����邽�߁A�g�U�̊�^�͏�Ƀ[���ł���B
		// �G�l���M�[�ۑ��̂��߁A�t���l���W���ƃ��^���l�X�Ɋ�Â��Ċg�UBRDF�̊�^���X�P�[�����O����K�v������
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

		// �����o�[�g�g�UBRDF�B
		// ���֗��ɂ��邽�߂ɁA�Ɩ��ƍޗ��̒P�ʂ�1/PI�ŃX�P�[�����O���Ă��܂���B
		// �Q�ƁF https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
        float3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    // Ambient lighting (IBL).
    float3 ambientLighting;
	{
		// �@�������̊g�U���ˏƓx�𑪒肷��
        float3 irradiance = irradiance_map.Sample(sampler_states[ANISOTROPIC], N).rgb;

		// �A���r�G���g�Ɩ��̃t���l�������v�Z����B
		// �v���t�B���^�����O���ꂽ�L���[�u�}�b�v���g�p���A���ˏƓx�͑����̕������痈��̂�
		// ���̔��x�N�g���i��L��cosLh�j�̊p�x�̑����cosLo���g�p����B
		// �Q��: https://seblagarde.wordpress.com/2011/08/17/hello-world/
        float3 F = fresnelSchlick(F0, cosLo);

		// �g�U��^�����擾����i���ڏƖ��Ɠ��l�j
        float3 kd = lerp(1.0 - F, 0.0, metalness);

		// ���ˏƓx�}�b�v�́ALambertian BRDF�����肵���o�˕��ˏƓx���܂�ł��邽�߁A�����ł�1/PI�ŃX�P�[������K�v�͂Ȃ��B
        float3 diffuseIBL = kd * albedo * irradiance;

		// �������~�b�v�}�b�v���x���Ŏ��O�Ƀt�B���^�����O���ꂽ���ʔ��ˊ����T���v�����O���܂��B
        uint specularTextureLevels = querySpecularTextureLevels();
        float3 specularIrradiance = specular_map.SampleLevel(sampler_states[ANISOTROPIC], Lr, roughness * specularTextureLevels).rgb;

		// �N�b�N�E�g�����X����BRDF�̃X�v���b�g�T���ߎ��W��.  ********�T���v���[�X�e�[�g�ύX���邱�ƁI�I�I�I
        float2 specularBRDF = specularBRDF_LUT.Sample(sampler_states[ANISOTROPIC], float2(cosLo, roughness)).rg;

		// ���ʔ��˂� IBL ��^�̍��v�B
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// �A���r�G���g�Ɩ��̊�^�̍��v
        ambientLighting = diffuseIBL + specularIBL;
    }
    // Final fragment color.
    return float4(directLighting + ambientLighting, 1.0);
}