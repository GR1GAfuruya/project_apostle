#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
//	�e�N�X�`��
Texture2D albedo_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D position_map : register(t2);
Texture2D metal_smooth_map : register(t3);
Texture2D emissive_map : register(t4);
Texture2D env_map : register(t15);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 normal = normal_map.Sample(sampler_states[LINEAR], pin.texcoord);
    float3 emissive = emissive_map.Sample(sampler_states[LINEAR], pin.texcoord).rgb;
	//���������@����Y
    float3 N = normalize(normal.xyz * 2.0 - 1.0);

	//�����̔���
    float4 P = position_map.Sample(sampler_states[LINEAR], pin.texcoord);
    float3 E = P.xyz - camera_position.xyz;
    float3 R = reflect(normalize(E), N);
	// float3 R = reflect(N, normalize(E));
    float3 Dir = R;
    Dir.y = 0; //��������
    Dir = normalize(Dir);
    float2 uv;
    uv.x = Dir.x;
	//�p�x�̂Ђ��݂𖳂��� //�������x�Ƒ��k acos���d��
	//-1<--->+1 ��0<--->1.0
    uv.x = acos(uv.x) / 3.141593;
	//0.0<--->1.0 ��-1<--->+1
    uv.x = uv.x * 2.0 - 1.0;

    uv.y = R.y;
	
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
	
	//��������
    uv.x *= 0.5;
	//��k����
    if (R.z > 0)
    {
        uv.x = 1 - uv.x;
    }
	
	//Roughness�\��
    float Roughness = 1 - metal_smooth_map.Sample(sampler_states[LINEAR], pin.texcoord).g;
  
	//R		0	0.1	   0.25     0.4	  1.0
	//N		0   0.45   0.72     0.81  1.0      //���K�����
	//Lv	0	5	   8	    9 	  11
	//1-R	1	0.9	   0.75		0.6   0.0
	//1-	1   0.55   0.28     0.19  0.0
    float level = pow(Roughness, 5.0);
    level =  level;
    level *= 11;
	//�~�b�v�}�b�v�𗘗p���Ăڂ���
    float3 light = env_map.SampleLevel(sampler_states[LINEAR], uv, level);
    //�G�~�b�V�u�}�b�v�K�p
    light += emissive;
    return float4(light.rgb, 1.0);
}