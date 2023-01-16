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
   // p.position += CurlNoise(p);
    p.streak_factor = streak_factor;
   
    //カールノイズ
    p.velocity = emitter.velocity;
    //生存時間
    p.life_time = emitter.emit_life_time;
    //生成してからの時間をリセット
    p.time = 0;
    //パーティクルの大きさ
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = emitter.particle_color;
    
    particle_buffer[id] = p;
}
