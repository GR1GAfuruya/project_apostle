#include "gpu_particles.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> dead_list : register(u1);

[numthreads(16, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    particle p = particle_buffer[DTid.x];
    if (p.is_active)
    {

        //カールノイズ
        p.velocity.xz += snoiseVec3(p.velocity.xyz * DTid.x % 16).xz;
        float grav = p.velocity.y + random((DTid.x % 16 - 12) * 60.0f * delta_time);
        grav =  min(grav, 20); //-10より小さくなったら強制
        p.velocity.y = grav;
        if(p.time > 0.5)
        {
            p.velocity.xz += CurlNoise(p).xz ;
        }
        p.streak_factor = 0.2;
        p.position += p.velocity * delta_time;
        //生存時間
        p.life_time = max(0, p.life_time - delta_time);
        p.time += delta_time;
        //アルファ設定
        p.color.a = lerp(1, 0, 1.0 - (p.life_time / 1.0));
        
        //速度が上がりすぎないようクランプ
        p.velocity.x = clamp(p.velocity.x, -100, 100);
        p.velocity.y = clamp(p.velocity.y, -100, 100);
        p.velocity.z = clamp(p.velocity.z, -100, 100);
        //寿命が尽きたら未使用リストへ戻す
        if (p.life_time <= 0)
        {
            p.velocity = float3(0, 0, 0);
            p.is_active = false;
            p.scale = 0;
            p.streak_factor = 0;
            p.time = 0;
            dead_list.Append(DTid.x);

        }
        particle_buffer[DTid.x] = p;

    }

}