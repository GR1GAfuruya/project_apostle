#include "object_mesh.hlsli"
// ���_�V�F�[�_
GS_INPUT main(VS_INPUT In)
{
   // ���[�J���ʒu�����̂܂܃W�I���g���V�F�[�_��
    GS_INPUT Out;
    Out.pos = float4(In.pos, 1.0);
    Out.normal = In.normal;
    return Out;
}