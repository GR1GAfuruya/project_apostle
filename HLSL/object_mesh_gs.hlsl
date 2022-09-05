#include "object_mesh.hlsli"

[MaxVertexCount(108)]
void main(triangle GS_INPUT In[3], inout TriangleStream<PS_INPUT> triStream)
{
// �����̖̂ʖ@��
    const float3 sqFaceNorm[6] =
    {
        float3(0.0, 0.0, -1.0),
   float3(1.0, 0.0, 0.0),
   float3(0.0, 0.0, 1.0),
   float3(-1.0, 0.0, 0.0),
   float3(0.0, 1.0, 0.0),
   float3(0.0, -1.0, 0.0)
    };

// �����̂̒��_���W
    const float3 sqVtx[8] =
    {
        float3(-1.0, -1.0, -1.0),
   float3(1.0, -1.0, -1.0),
   float3(-1.0, 1.0, -1.0),
   float3(1.0, 1.0, -1.0),
   float3(-1.0, -1.0, 1.0),
   float3(1.0, -1.0, 1.0),
   float3(-1.0, 1.0, 1.0),
   float3(1.0, 1.0, 1.0)
    };

// �����̂̃C���f�b�N�X
    const int sqIdx[36] =
    {
        0, 2, 1,
   1, 2, 3,
   1, 3, 5,
   5, 3, 7,
   5, 7, 4,
   4, 7, 6,
   4, 6, 0,
   0, 6, 2,
   2, 6, 3,
   3, 6, 7,
   0, 1, 5,
   0, 5, 4
    };

    
	//�e���_�ɔz�u���闧���̂̑傫�����Ȃ�ƂȂ��Z�o
    float L01 = length(In[1].pos.xyz - In[0].pos.xyz);
    float L12 = length(In[2].pos.xyz - In[1].pos.xyz);
    float L02 = length(In[0].pos.xyz - In[2].pos.xyz);
    float sq_scale[3] =
    {
		min(L01, L02),
		min(L01, L12),
		min(L12, L02)
    };

    //���_�̈ʒu�ɗ����̂�z�u��WVP�ϊ�
    //�@����WV��
    float4x4 wv = mul(world, view);
    float4x4 wvp = mul(wv, projection);
    int faceId = 0;
    for(int v = 0; v < 3; ++ v)//���_�ԍ��̃��[�v
    {
        for (int p = 0; p < 12; ++p)//�����̂̊e�|���S���ԍ�
        {
            faceId = p / 2;
            for (int i = 0; i < 3; ++i)//�����̂̃|���S��1���̒��_�ԍ�
            {
                PS_INPUT Out;
                int idx = sqIdx[p * 3 + i];
                float3 localP = sqVtx[idx] * sq_scale[v] * scale + In[v].pos.xyz;
                Out.pos = mul(float4(localP, 1.0), wvp);
                Out.normal = mul(float4(sqFaceNorm[faceId], 0.0), wv).xyz;
                //�X�g���[���ɒǉ�
                triStream.Append(Out);
            }
            triStream.RestartStrip();//3���X�g���[���o�͂ɋ�؂������
        }

    }

}