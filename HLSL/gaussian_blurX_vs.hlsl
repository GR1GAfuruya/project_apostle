#include "gaussian_blur.hlsli"

Texture2D<float4> scene_texture : register(t0); // シーンテクスチャ
VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    // 横ブラー用の頂点シェーダーを実装
    VS_OUT vout;

    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    vout.pos = float4(position[vertexid], 0, 1);
    float2 tex = texcoords[vertexid];

    // テクスチャサイズを取得
    float2 texSize;
    float level;
    scene_texture.GetDimensions(0, texSize.x, texSize.y, level);

    // 基準テクセルからU座標を+1テクセルずらすためのオフセットを計算する
    vout.tex0.xy = float2(1.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+3テクセルずらすためのオフセットを計算する
    vout.tex1.xy = float2(3.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+5テクセルずらすためのオフセットを計算する
    vout.tex2.xy = float2(5.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+7テクセルずらすためのオフセットを計算する
    vout.tex3.xy = float2(7.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+9テクセルずらすためのオフセットを計算する
    vout.tex4.xy = float2(9.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+11テクセルずらすためのオフセットを計算する
    vout.tex5.xy = float2(11.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+13テクセルずらすためのオフセットを計算する
    vout.tex6.xy = float2(13.0f / texSize.x, 0.0f);

    // 基準テクセルからU座標を+15テクセルずらすためのオフセットを計算する
    vout.tex7.xy = float2(15.0f / texSize.x, 0.0f);

    // オフセットに-1を掛けてマイナス方向のオフセットも計算する
    vout.tex0.zw = vout.tex0.xy * -1.0f;
    vout.tex1.zw = vout.tex1.xy * -1.0f;
    vout.tex2.zw = vout.tex2.xy * -1.0f;
    vout.tex3.zw = vout.tex3.xy * -1.0f;
    vout.tex4.zw = vout.tex4.xy * -1.0f;
    vout.tex5.zw = vout.tex5.xy * -1.0f;
    vout.tex6.zw = vout.tex6.xy * -1.0f;
    vout.tex7.zw = vout.tex7.xy * -1.0f;

    // オフセットに基準テクセルのUV座標を足し算して、
    // 実際にサンプリングするUV座標に変換する
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
