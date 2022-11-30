#include "gpu_particles.hlsli"
#include "constants.hlsli"


RWStructuredBuffer<particle> particle_buffer : register(u0);
ConsumeStructuredBuffer<uint> particle_pool : register(u1);

[numthreads(16, 1, 1)]
void main()
{
    uint id = particle_pool.Consume();
    particle p = particle_buffer[id];
    p.is_active = true;

    p.position = emitter.pos;
    p.position.x += cos(id % 16);
    p.position.z += sin(id % 16);
    p.streak_factor = random(id) % 0.2;
   
    //カールノイズ
    p.velocity = CurlNoise(p);
    p.velocity.y = min(p.velocity.y, 20);
    p.velocity.y += 5.0;
    //生存時間m
    p.life_time = random(id);
    //生成してからの時間をリセット
    p.time = 0;
    //パーティクルの大きさ
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = particle_color;
    particle_buffer[id] = p;
}
