#include "gaussian_blur.hlsli"

Texture2D<float4> scene_texture : register(t0); // �V�[���e�N�X�`��
VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    // ���u���[�p�̒��_�V�F�[�_�[������
    VS_OUT vout;

    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    vout.pos = float4(position[vertexid], 0, 1);
    float2 tex = texcoords[vertexid];

    // �e�N�X�`���T�C�Y���擾
    float2 texSize;
    float level;
    scene_texture.GetDimensions(0, texSize.x, texSize.y, level);

    // ��e�N�Z������U���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex0.xy = float2(1.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex1.xy = float2(3.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex2.xy = float2(5.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex3.xy = float2(7.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex4.xy = float2(9.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex5.xy = float2(11.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex6.xy = float2(13.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    vout.tex7.xy = float2(15.0f / texSize.x, 0.0f);

    // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    vout.tex0.zw = vout.tex0.xy * -1.0f;
    vout.tex1.zw = vout.tex1.xy * -1.0f;
    vout.tex2.zw = vout.tex2.xy * -1.0f;
    vout.tex3.zw = vout.tex3.xy * -1.0f;
    vout.tex4.zw = vout.tex4.xy * -1.0f;
    vout.tex5.zw = vout.tex5.xy * -1.0f;
    vout.tex6.zw = vout.tex6.xy * -1.0f;
    vout.tex7.zw = vout.tex7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    vout.tex0 += float4(tex, tex);
    vout.tex1 += float4(tex, tex);
    vout.tex2 += float4(tex, tex);
    vout.tex3 += float4(tex, tex);
    vout.tex4 += float4(tex, tex);
    vout.tex5 += float4(tex, tex);
    vout.tex6 += float4(tex, tex);
    vout.tex7 += float4(tex, tex);

    return vout;
}
