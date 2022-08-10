#include "object_mesh.hlsli"
// 頂点シェーダ
GS_INPUT main(VS_INPUT In)
{
   // ローカル位置をそのままジオメトリシェーダへ
    GS_INPUT Out;
    Out.pos = float4(In.pos, 1.0);
    Out.normal = In.normal;
    return Out;
}