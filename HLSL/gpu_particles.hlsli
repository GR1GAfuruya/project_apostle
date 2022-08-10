struct VS_OUT
{
    uint vertex_id : VERTEXID;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
struct particle
{
    float3 position;//�ʒu
    float3 velocity;//����
    float3 angle;//�p�x
    float3 scale;//�傫��
    float4 color;//�F
    float streak_factor;//���x�ɑ΂���X�v���C�g�̈������΂�
    float time; //��������Ă���̎���
    float life_time;//����
    bool is_active;//�A�N�e�B�u��Ԃ�
};

struct Emitter
{
    float3 pos;
    float emit_life; // �G�~�b�^�[�̎���
    float emit_life_time; // �G�~�b�^�[�̔N��
    float emit_time; //��������Ă���̎���
    float emit_rate; // 1�b�Ԃɉ����������邩
    int emit_count; // ���݂̔����J�E���g
};

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    Emitter emitter;

    float2 particle_size;
    uint particle_count;
    float3 angle;
};

#include "noise.hlsli"
//�֐�
// �p�[�����m�C�Y�ɂ��x�N�g����
// 3D�Ƃ���3�v�f���v�Z�B
// ���ꂼ��̃m�C�Y�͖��炩�ɈႤ�i�ɒ[�ɑ傫�ȃI�t�Z�b�g�j�����������l�Ƃ���
float3 Pnoise(float3 vec)
{
    float x = snoise(vec);

    float y = snoise(float3(
        vec.y + 31.416,
        vec.z - 47.853,
        vec.x + 12.793
    ));

    float z = snoise(float3(
        vec.z - 233.145,
        vec.x - 113.408,
        vec.y - 185.31
    ));

    return float3(x, y, z);
}

// �� = ramp(d(x)/d0)
// ��_constrainted(x) = ����(x) + (1 - ��)n(n�E��(x))
float3 SamplePotential(float3 pos, float time)
{
  //  float3 s = pos / _NoiseScales[0];
    float3 s = pos / 9;
    return Pnoise(s);
}
float3 CurlNoise(particle p)
{
    //const float e = 0.0009765625;
    const float e = 1e-4f;
    const float e2 = 2.0 * e;
    const float invE2 = 1.0 / e2;

    const float3 dx = float3(e, 0.0, 0.0);
    const float3 dy = float3(0.0, e, 0.0);
    const float3 dz = float3(0.0, 0.0, e);

    float3 pos = p.position;

    float3 p_x0 = SamplePotential(pos - dx, p.time);
    float3 p_x1 = SamplePotential(pos + dx, p.time);
    float3 p_y0 = SamplePotential(pos - dy, p.time);
    float3 p_y1 = SamplePotential(pos + dy, p.time);
    float3 p_z0 = SamplePotential(pos - dz, p.time);
    float3 p_z1 = SamplePotential(pos + dz, p.time);

    float x = (p_y1.z - p_y0.z) - (p_z1.y - p_z0.y);
    float y = (p_z1.x - p_z0.x) - (p_x1.z - p_x0.z);
    float z = (p_x1.y - p_x0.y) - (p_y1.x - p_y0.x);

    return float3(x, y, z) * invE2;
}
