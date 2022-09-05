#include "object_mesh.hlsli"

[MaxVertexCount(108)]
void main(triangle GS_INPUT In[3], inout TriangleStream<PS_INPUT> triStream)
{
// 立方体の面法線
    const float3 sqFaceNorm[6] =
    {
        float3(0.0, 0.0, -1.0),
   float3(1.0, 0.0, 0.0),
   float3(0.0, 0.0, 1.0),
   float3(-1.0, 0.0, 0.0),
   float3(0.0, 1.0, 0.0),
   float3(0.0, -1.0, 0.0)
    };

// 立方体の頂点座標
    const float3 sqVtx[8] =
    {
        float3(-1.0, -1.0, -1.0),
   float3(1.0, -1.0, -1.0),
   float3(-1.0, 1.0, -1.0),
   float3(1.0, 1.0, -1.0),
   float3(-1.0, -1.0, 1.0),
   float3(1.0, -1.0, 1.0),
   float3(-1.0, 1.0, 1.0),
   float3(1.0, 1.0, 1.0)
    };

// 立方体のインデックス
    const int sqIdx[36] =
    {
        0, 2, 1,
   1, 2, 3,
   1, 3, 5,
   5, 3, 7,
   5, 7, 4,
   4, 7, 6,
   4, 6, 0,
   0, 6, 2,
   2, 6, 3,
   3, 6, 7,
   0, 1, 5,
   0, 5, 4
    };

    
	//各頂点に配置する立方体の大きさをなんとなく算出
    float L01 = length(In[1].pos.xyz - In[0].pos.xyz);
    float L12 = length(In[2].pos.xyz - In[1].pos.xyz);
    float L02 = length(In[0].pos.xyz - In[2].pos.xyz);
    float sq_scale[3] =
    {
		min(L01, L02),
		min(L01, L12),
		min(L12, L02)
    };

    //頂点の位置に立方体を配置しWVP変換
    //法線はWVで
    float4x4 wv = mul(world, view);
    float4x4 wvp = mul(wv, projection);
    int faceId = 0;
    for(int v = 0; v < 3; ++ v)//頂点番号のループ
    {
        for (int p = 0; p < 12; ++p)//立方体の各ポリゴン番号
        {
            faceId = p / 2;
            for (int i = 0; i < 3; ++i)//立方体のポリゴン1枚の頂点番号
            {
                PS_INPUT Out;
                int idx = sqIdx[p * 3 + i];
                float3 localP = sqVtx[idx] * sq_scale[v] * scale + In[v].pos.xyz;
                Out.pos = mul(float4(localP, 1.0), wvp);
                Out.normal = mul(float4(sqFaceNorm[faceId], 0.0), wv).xyz;
                //ストリームに追加
                triStream.Append(Out);
            }
            triStream.RestartStrip();//3つずつストリーム出力に区切りをつける
        }

    }

}