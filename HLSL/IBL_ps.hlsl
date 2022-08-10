//物理ベースレンダリング
//著作権 (c) 2017-2018 Michal Siejak

//Physically Basedシェーディングモデル。
//Lambertian diffuse BRDF + Cook-Torrance microfact specular BRDF + アンビエントのためのIBL
// See: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

#include "constants.hlsli"
#include "default_mesh.hlsli"
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

static const uint NumLights = 3;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

Texture2D albedo_map: register(t0);
Texture2D normal_map : register(t1);
Texture2D metalness_map : register(t2);
Texture2D roughness_map : register(t3);
TextureCube specular_map : register(t4);
TextureCube irradiance_map : register(t5);
Texture2D specularBRDF_LUT : register(t6);

// サンプラーステート
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);

// GGX/Towbridge-Reitz 正規分布関数.
// Alpha = roughness^2 の Disney の再パラメトリック化を使用.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// 分離可能なSchlick-GGXの単一項。
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}


// スミス法による幾何学的減衰関数のSchlick-GGX近似.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// シュリックのフレネル係数近似。
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// 鏡面 IBL 環境マップのミップマップレベル数を返します。
uint querySpecularTextureLevels()
{
    uint width, height, levels;
    specular_map.GetDimensions(0, width, height, levels);
    return levels;
}

float3 get_normal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = normal_map.SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

    return newNormal;
}

float4 main(VS_OUT pin) : SV_Target
{
    // シェーディングモデルを取得するために入力テクスチャをサンプルする params.
    float3 albedo = albedo_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
    float metalness = metalness_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    float roughness = roughness_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;

    // 光の出射方向（世界空間のフラグメント位置から "eye "へのベクトル）
    float3 Lo = normalize(camera_position - pin.position);

     // 法線を計算
    float3 normal = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(normal, T));
    float3 B = normalize(cross(normal, T) * sigma);
    float3 N = get_normal(normal, T, B, pin.texcoord);

    // 表面法線と出射光方向との間の角度．
    float cosLo = max(0.0, dot(normal, Lo));
		
	// 鏡面反射ベクトル.
    float3 Lr = 2.0 * cosLo * N - Lo;

	// 垂直入射におけるフレネル反射率（金属の場合はアルベドカラーを使用）
    float3 F0 = lerp(Fdielectric, albedo, metalness);

    // 分析用照明の直接照明計算
    float3 directLighting = 0.0;
    //ライト計算
    {
        float3 Li = -light_direction.xyz;
        float3 Lradiance = 1; //平行光なので１
    // LiとLoの間の半ベクトル．
        float3 Lh = normalize(Li + Lo);

		// サーフェス法線と様々なライトベクトルとの間の角度を計算する
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));

		// 直接照明のフレネル項を計算する．
        float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// 鏡面BRDFの正規分布を計算する
        float D = ndfGGX(cosLh, roughness);
		// 鏡面BRDFの幾何学的減衰を計算する
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// 拡散散乱は、光が誘電体によって何度も屈折されることによって起こる。
		// 一方、金属はエネルギーを反射または吸収するため、拡散の寄与は常にゼロである。
		// エネルギー保存のため、フレネル係数とメタルネスに基づいて拡散BRDFの寄与をスケーリングする必要がある
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

		// ランバート拡散BRDF。
		// より便利にするために、照明と材料の単位を1/PIでスケーリングしていません。
		// 参照： https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
        float3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    // Ambient lighting (IBL).
    float3 ambientLighting;
	{
		// 法線方向の拡散放射照度を測定する
        float3 irradiance = irradiance_map.Sample(sampler_states[ANISOTROPIC], N).rgb;

		// アンビエント照明のフレネル項を計算する。
		// プリフィルタリングされたキューブマップを使用し、放射照度は多くの方向から来るので
		// 光の半ベクトル（上記のcosLh）の角度の代わりにcosLoを使用する。
		// 参照: https://seblagarde.wordpress.com/2011/08/17/hello-world/
        float3 F = fresnelSchlick(F0, cosLo);

		// 拡散寄与率を取得する（直接照明と同様）
        float3 kd = lerp(1.0 - F, 0.0, metalness);

		// 放射照度マップは、Lambertian BRDFを仮定した出射放射照度を含んでいるため、ここでも1/PIでスケールする必要はない。
        float3 diffuseIBL = kd * albedo * irradiance;

		// 正しいミップマップレベルで事前にフィルタリングされた鏡面反射環境をサンプリングします。
        uint specularTextureLevels = querySpecularTextureLevels();
        float3 specularIrradiance = specular_map.SampleLevel(sampler_states[ANISOTROPIC], Lr, roughness * specularTextureLevels).rgb;

		// クック・トランス鏡面BRDFのスプリットサム近似係数.  ********サンプラーステート変更すること！！！！
        float2 specularBRDF = specularBRDF_LUT.Sample(sampler_states[ANISOTROPIC], float2(cosLo, roughness)).rg;

		// 鏡面反射の IBL 寄与の合計。
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// アンビエント照明の寄与の合計
        ambientLighting = diffuseIBL + specularIBL;
    }
    // Final fragment color.
    return float4(directLighting + ambientLighting, 1.0);
}