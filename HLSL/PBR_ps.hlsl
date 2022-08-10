/*!
 * @brief ディズニーベースの物理ベースシェーダ
 */
#include "default_mesh.hlsli"
#include "constants.hlsli"
#include  "PBR_function.hlsli"

///////////////////////////////////////////////////
// グローバル変数
///////////////////////////////////////////////////

// アルベドマップ
Texture2D<float4> albedo_map : register(t0);

// 法線マップ
Texture2D<float4> normal_map : register(t1);

// メタリックスムースマップ。rにメタリック、aにスムース
Texture2D<float4> metallic_map : register(t2);

Texture2D<float4> smooth_map : register(t3);

Texture2D<float4> ambient_oclusion_map : register(t4);

Texture2D<float4> emissive_map : register(t5);


// サンプラーステート
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);


///////////////////////////////////////////////////
// 関数
///////////////////////////////////////////////////

float3 get_normal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = normal_map.SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

    return newNormal;
}
/// <summary>
/// ピクセルシェーダー
/// </summary>
float4 main(VS_OUT pin) : SV_TARGET
{
    // 法線を計算
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    float3 normal = get_normal(N, T, B, pin.texcoord);
    // step-2 各種マップをサンプリングする
    // アルベドカラー（拡散反射光）
    float4 albedo_color = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // スペキュラカラーはアルベドカラーと同じにする
    float3 specColor = albedo_color;

    // 金属度
    float metallic = metallic_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // 滑らかさ
    float smooth = smooth_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    float4 ao_map = ambient_oclusion_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    float4 emissive = emissive_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(camera_position - pin.world_position);


    // シンプルなディズニーベースの拡散反射を実装する
     // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
    float diffuseFromFresnel = calc_diffuse_from_fresnel(normal, -light_direction.xyz * 2.0f, toEye);
    float NdotL = saturate(dot(normal, -light_direction.xyz * 2.0f));
    float3 lambertDiffuse = float4(1, 1, 1, 1) * NdotL / PI;
    float3 diffuse = albedo_color.rgb * diffuseFromFresnel * lambertDiffuse;
    // Cook-Torranceモデルを利用した鏡面反射率を計算する
    float3 spec = cook_torrance_specular(-light_direction.xyz * 2.0f, toEye, normal, smooth) * float4(1, 1, 1, 1);
    spec *= lerp(float3(1, 1, 1), albedo_color.rgb, metallic);
   
    float3 lig = diffuse * (1.0f - smooth) + spec * 2;
	// 滑らかさが高ければ、拡散反射は弱くなる
	lig += diffuse * (1.0f - smooth) + spec;

    // 環境光による底上げ
   // lig += light_color * albedo_color;
   
    float4 finalColor = 1.0f;
    finalColor.xyz = lig;

    // emissive
    float3 emi = emissive.r * emissive.w * emissive.rgb;
    finalColor.xyz += emi;

    return finalColor;
}
