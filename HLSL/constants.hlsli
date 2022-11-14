
cbuffer MATERIAL_CONSTANTS : register(b2)
{
    float2 texcoord_offset;
    float2 texcoord_scale;
    float emissive_power;
};
cbuffer SCENE_CONSTANT : register(b3)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_projection;
    float4 light_color;
    float4 light_direction;
    float4 camera_position;
    float4 avatar_position;
    float4 avatar_direction;
    float2 resolution;
    float time;
    float delta_time;
};






cbuffer POST_EFFECTS_CONSTANT : register(b5)
{
    //LUMINA
    float4 l_min;
    float4 l_max;
    float4 lumina;
    //BLUR
    float2 c_sigma;
    float bloom_extraction_threshold;
    float blur_convolution_intensity;
   // float4 weight[2]; // 重み
    //RADIAL_BLUR
    float2 origin; //中心点
    float ray_power; //ブラーをかけるパワー :方向ベクトルにかけて光線の長さを決める
    float wipe_threshold;
      // 色相調整
    float hueShift;
    // 彩度調整
    float saturation;
    // 明度調整
    float brightness;
    //コントラスト
    float contrast;
    //ビネット
    float falloff;
    float amount;
}


cbuffer DEFFERD_LIGHT_CONSTANT : register(b7)
{
	//平行光　xyz＝方向 w = 0.0
	//点光源　xyz＝位置 w = 減衰距離
    float4 defferd_light_dir;
    float4 defferd_light_color;
}
