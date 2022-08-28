#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
Texture2D originall_texture : register(t0);

// �T���v���[�X�e�[�g
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

static const float gamma = 2.3;
float3 simple_reinhard_tonemapping(float3 color, float exposure)
{
    //float exposure = 1.5;
    color *= exposure / (1. + color / exposure);
    color = pow(color, 1. / gamma);
    return color;
}

float4 tone_map(float4 sample_color)
{
    float4 color = sample_color;
    
    float3 fragment_color = color.rgb;
    float alpha = color.a;

#if 1
	// Apply tone mapping.
    const float exposure = brightness;
	//fragment_color = 1 - exp(-fragment_color * exposure);
	
	//fragment_color = filmic_tonemapping(fragment_color);
    //fragment_color = uncharted2_tonemapping(fragment_color, exposure);
    //fragment_color = white_preserving_lumabased_reinhard_tonemapping(fragment_color);
    fragment_color = simple_reinhard_tonemapping(fragment_color, exposure);
#endif
    
#if 0
	// Apply gamma correction.
	const float GAMMA = 2.2;
	fragment_color = pow(fragment_color, 1.0 / GAMMA);
#endif
    
    return float4(fragment_color, alpha);
}

//--------------------------------------------
//	RGB�F��Ԃ̐��l����HSV�F��Ԃ̐��l�ւ̕ϊ��֐�
//--------------------------------------------
//rgb:RGB�F��Ԃ̐��l
float3 RGB2HSV(float3 rgb)
{
    float3 hsv = 0;

    float Vmax = max(rgb.r, max(rgb.g, rgb.b)); // RGB�̎O�̒l�ōő�̂���
    float Vmin = min(rgb.r, min(rgb.g, rgb.b)); // RGB�̎O�̒l�ōŏ��̂���
    float delta = Vmax - Vmin; // �ő�l�ƍŏ��l�̍�

	// V�i���x�j ��ԋ����F��V�l�ɂ���
    hsv.z = Vmax;

	// S�i�ʓx�j �ő�l�ƍŏ��l�̍��𐳋K�����ċ��߂�
    hsv.y = (delta / Vmax) * step(0, Vmax);

	// H�i�F���j RGB�̂����ő�l�ƍŏ��l�̍����狁�߂�
    if (hsv.y > 0.0f)
    {
        if (rgb.r == Vmax)
        {
            hsv.x = 60 * ((rgb.g - rgb.b) / delta);
        }
        else if (rgb.g == Vmax)
        {
            hsv.x = 60 * ((rgb.b - rgb.r) / delta) + 120;
        }
        else
        {
            hsv.x = 60 * ((rgb.r - rgb.g) / delta) + 240;
        }

        if (hsv.x < 0)
        {
            hsv.x += 360;
        }
    }
    return hsv;
}

//--------------------------------------------
//	HSV�F��Ԃ̐��l����RGB�F��Ԃ̐��l�ւ̕ϊ��֐�
//--------------------------------------------
//hsv:HSV�F��Ԃ̐��l
float3 HSV2RGB(float3 hsv)
{
    float3 rgb = 0;
    if (hsv.y == 0)
    {
		// S�i�ʓx�j��0�Ɠ������Ȃ�Ζ��F�������͊D�F
        rgb.r = rgb.g = rgb.b = hsv.z;
    }
    else
    {
		// �F��H�i�F���j�̈ʒu��S�i�ʓx�j�AV�i���x�j����RGB�l���Z�o����
        float Vmax = hsv.z;
        float Vmin = Vmax - (hsv.y * Vmax);

        hsv.x %= 360; //	0�`360�ɕϊ�
        float Huei = (int) (hsv.x / 60);
        float Huef = hsv.x / 60 - Huei;
        float p = Vmax * (1.0f - hsv.y);
        float q = Vmax * (1.0f - hsv.y * Huef);
        float t = Vmax * (1.0f - hsv.y * (1 - Huef));
        if (Huei == 0)
        {
            rgb.r = Vmax;
            rgb.g = t;
            rgb.b = p;
        }
        else if (Huei == 1)
        {
            rgb.r = q;
            rgb.g = Vmax;
            rgb.b = p;
        }
        else if (Huei == 2)
        {
            rgb.r = p;
            rgb.g = Vmax;
            rgb.b = t;
        }
        else if (Huei == 3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = Vmax;
        }
        else if (Huei == 4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = Vmax;
        }
        else if (Huei == 5)
        {
            rgb.r = Vmax;
            rgb.g = p;
            rgb.b = q;
        }
    }
    return rgb;
}

float4 color_filter(float4 sample_color)
{
    float4 color = sample_color;
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

float4 main(VS_OUT pin) : SV_Target0
{
    float4 sampled_color = originall_texture.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 color = sampled_color;

    color = color_filter(color);
    return color;
}