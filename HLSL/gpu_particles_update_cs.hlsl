#include "gpu_particles.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> dead_list : register(u1);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particle p = particle_buffer[DTid.x];
    if (p.is_active)
    {
        //p.velocity -= snoise(DTid.x * time);
        //p.velocity.y /= 2;
        //カールノイズ
     //   p.velocity.xz += curlNoise(p.velocity).xz;
       // p.angle += curlNoise(p.angle/2);
        //竜巻
        p.angle = angle;
        //p.velocity.z += cos(p.life_time) / 20;
       // p.velocity.y += -0.2 ;
        //p.streak_factor = random(p.streak_factor) % 0.2;
        p.position += p.velocity * delta_time;
        p.life_time = max(0, p.life_time - delta_time);
        p.time += delta_time;
       
        p.color.a = lerp(1, 0, 1.0 - (p.life_time / 1.0));
       // p.color.a = 0.5;
        //寿命が尽きたら未使用リストへ戻す
        if (p.life_time <= 0)
        {
            p.velocity = float3(0, 0, 0);
            p.is_active = false;
            p.scale = 0;
            p.time = 0;
            dead_list.Append(DTid.x);
        }
		particle_buffer[DTid.x] = p;
    }

}