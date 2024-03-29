#include "shadow_map.hlsli"

float4 main(VS_OUT pin) : SV_TARGET0
{
	// 光からの距離をレンダリング
    float depth = pin.ShadowParam.z / pin.ShadowParam.w;
    return float4(depth, 0, 0, 1);
}
