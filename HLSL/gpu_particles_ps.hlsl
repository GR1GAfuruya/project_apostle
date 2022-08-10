#include "gpu_particles.hlsli"
#include "constants.hlsli"
SamplerState anisotropic_sampler_state : register(s2);

Texture2D color_map : register(t5);

#if 1
[earlydepthstencil]
float4 main(GS_OUT pin) : SV_TARGET
{
    //pin.color.a = 1;
    float4 sample_color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float3 color = sample_color.rgb * pin.color.rgb;
    float alpha = sample_color.a * pin.color.a;
   // color.a = /*sample_color.a **/ pin.color.a;
    //clip(color.a - 0.1);
    return float4(color, alpha);
}
#else
float snoise(float3 uv, float res)
{
    const float3 s = float3(1e0, 1e2, 1e3);
	
    uv *= res;
	
    float3 uv0 = floor(fmod(uv, res)) * s;
    float3 uv1 = floor(fmod(uv + float3(1.0, 1.0, 1.0), res)) * s;
	
    float3 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);

    float4 v = float4(uv0.x + uv0.y + uv0.z, uv1.x + uv0.y + uv0.z,
		      	  uv0.x + uv1.y + uv0.z, uv1.x + uv1.y + uv0.z);

    float4 r = frac(sin(v * 1e-1) * 1e3);
    float r0 = (lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
    r = frac(sin((v + uv1.z - uv0.z) * 1e-1) * 1e3);
    float r1 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
    return lerp(r0, r1, f.z) * 2. - 1.;
}

[earlydepthstencil]
float4 main(GS_OUT pin) : SV_TARGET
{
    return 1;
    float2 p = -0.5 + pin.texcoord.xy;
    //p.x *= resolution.x / resolution.y;
	
    float color = 3.0 - (3. * length(2. * p));
	
    float3 coord = float3(atan2(p.x, p.y) / 6.2832 + .5, length(p) * .4, .5);
	
    for (int i = 1; i <= 7; i++)
    {
        float power = pow(2.0, float(i));
        color += (1.5 / power) * snoise(coord + float3(0., -time * .05, time * .01), power * 16.);
    }
    float alpha = step(0.01, color);
    return float4(color, pow(max(color, 0.), 2.) * 0.4, pow(max(color, 0.), 3.) * 0.15, alpha);
}
#endif