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
   
    p.streak_factor = random(id) % 0.2;
    p.position.x += cos(id % 16) * 20 % 20;
    p.position.z += sin(id % 16) * 20 % 20;
    //p.velocity.x = cos(id % 16) * 5;
    //p.velocity.z = sin(id % 16) * 5;
    //p.velocity = CalcVelocity(forceAngle, id);
    //カールノイズ
    //p.velocity = curlNoise(id % 16) * (time % 30);
    //p.velocity.y = min(p.velocity.y, 20);
    //p.velocity.y += 20.0;
    //竜巻
    //生存時間m
    p.life_time = 2;
    //生成してからの時間をリセット
    p.time = 0;
    //パーティクルの大きさ
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
    float rand = step(random(id / time), 0.5);
        
   // p.color = rand > 0.5 ? float4(0.5, 0.4, 0, 1) : float4(1, 0.3, 0.2, 1);
    p.color = float4(10, 2, 0.0, 0.7);
    particle_buffer[id] = p;
}
