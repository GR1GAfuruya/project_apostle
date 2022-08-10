//--------------------------------------------
//	ランバート拡散反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 calc_lambert(float3 N, float3 L, float3 C, float3 K = 1.0)
{
    float power = saturate(dot(N, -L));
    return C * power * K ;
}

//--------------------------------------------
//	フォンの鏡面反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// E:視線ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 calc_phong_specular(float3 N, float3 L, float3 E, float3 C, float3 K = 1.0)
{
    float3 R = reflect(L, N);
    float power = max(dot(-E, R), 0);
    power = pow(power, 128);
    return C * power ;
}

//--------------------------------------------
//	ハーフランバート拡散反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 clac_half_lambert(float3 N, float3 L, float3 C, float3 K = 1.0)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    return C * D * K;
}


//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// C :ライト色
// RimPower : リムライトの強さ(初期値はテキトーなので自分で設定するが吉)
float3 CalcRimLight(float3 N, float3 E, float3 L, float3 C, float RimPower = 4.0f)
{
    float rim = 1.0f - saturate(dot(N, -E));
    return C * pow(rim, RimPower) * saturate(dot(L, -E));
}

//--------------------------------------------
// ランプシェーディング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)

float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C,float3 K = 1.0)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return C * Ramp * K;
}


//--------------------------------------------
// 球体環境マッピング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// color: 現在のピクセル色
// N:法線(正規化済み)
// C:入射光(色・強さ)
// value:適応率
float3 CalcSphereEnvironment(Texture2D tex, SamplerState samp, in float3 color, float3 N, float3 E, float value)
{
    float3 R = reflect(E, N); //eyeと法線の反射ベクトル
    float2 texcoord = R.xy * 0.5f + 0.5f;
    return lerp(color.rgb, tex.Sample(samp, texcoord).rgb, value);
}
