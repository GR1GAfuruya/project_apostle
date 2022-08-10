#include "object_mesh.hlsli"
// ピクセルシェーダ
float4 main(PS_INPUT In) : SV_TARGET
{
    return float4(float3(0.8, 0.8, 0.8) * (-normalize(In.normal).z), 1.0);
}