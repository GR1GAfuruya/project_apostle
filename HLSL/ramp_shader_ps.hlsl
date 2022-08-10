#include "constants.hlsli"
#include "default_mesh.hlsli"

#include "shading_functions.hlsli"


Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D ramp_map : register(t7);
SamplerState ramp_sampler_state : register(s2);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = color_map.Sample(color_sampler_state, pin.texcoord);

    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(-light_direction.xyz);
    float3 N = normalize(pin.world_normal.xyz);
    float3 ambient = 0.0f;
    
    float3 directional_diffuse = CalcRampShading(ramp_map, ramp_sampler_state,
													N, L, light_color.rgb);
   
    float3 ks = { 1.0f, 1.0f, 1.0f };
    float3 directional_specular = calc_phong_specular(N, L, E, light_color.rgb, ks.rgb);

    float4 color = float4(ambient, diffuse_color.a);
    color.rgb += diffuse_color.rgb * directional_diffuse;
    color.rgb += directional_specular;
    return color ;
}
