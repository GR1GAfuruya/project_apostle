#include "fullscreen_quad.hlsli"

#include "constants.hlsli"
Texture2D originall_texture : register(t0);

// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);



float3 RGB2HSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 HSV2RGB(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float4 color_filter(float4 c)
{
    float4 color = c;
	// RGB > HSV�ɕϊ�
    color.rgb = RGB2HSV(color.rgb);

	// �F������
    color.r += hueShift;

	// �ʓx����
    color.g *= saturation;

	// ���x����
    color.b *= brightness;

	// HSV > RGB�ɕϊ�
    color.rgb = HSV2RGB(color.rgb);

    return color;
}

//�R���g���X�g
float4 Contrast(float4 c)
{
    float4 color = c;
	//�V�O���C�h�Ȑ��ɂ��S���J�[�u
    color = 1 / (1 + exp(-contrast * (color - 0.5)));

    return color;
}

//�r�l�b�g
float4 Vignette(float4 c,float2 uv)
{
    float3 color = c.rgb;
    float2 center = float2(0.5, 0.5);
    float len = length(uv - center);
    color *= smoothstep(1., falloff, len * (amount + falloff));
    return float4(color, 1.0);

}


float4 main(VS_OUT pin) : SV_Target0
{
    float4 sampled_color = originall_texture.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = sampled_color;
    //�J���[�t�B���^�[
    color = color_filter(color);
    //�R���g���X�g
    color = Contrast(color);
    //�r�l�b�g
    color = Vignette(color,pin.texcoord);
    return color;
}