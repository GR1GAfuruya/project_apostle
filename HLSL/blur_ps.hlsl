#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    return 1;
//    uint mip_level = 0, width, height, number_of_levels;
//    texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels); //画像のサイズを参照

//    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
//    float alpha = color.a;
    
//    float3 blur_color = 0;
//    float gaussian_kernel_total = 0;

//    const int gaussian_half_kernel_size = 3; //kernel:核　　ブラーの核の大きさ？
//    const float gaussian_sigma = c_sigma.x; //ブラーの度合
//    [unroll] //ループの実行が停止するまで、ループをアンロール
//    for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
//    {
//    [unroll]
//        for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
//        {
//            float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) /
//            (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
//            blur_color += texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord +
//            float2(x * 1.0 / width, y * 1.0 / height)).rgb * gaussian_kernel;
//            gaussian_kernel_total += gaussian_kernel;
//        }
//    }
//    blur_color /= gaussian_kernel_total;
//    const float bloom_intensity = c_bloom_intensity.x;
//    color.rgb += blur_color * bloom_intensity;
//#if 1
//    // Tone mapping : HDR -> SDR
//    const float exposure = 1.2;
//    color.rgb = 1 - exp(-color.rgb * exposure);
//#endif

//#if 1
//    // Gamma process
//    const float GAMMA = 2.2;
//    color.rgb = pow(color.rgb, 1.0 / GAMMA);
//#endif

//    return float4(color.rgb, alpha);

}
