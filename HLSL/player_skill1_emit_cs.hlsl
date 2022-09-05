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

    p.position.x = emitter.pos.x + random(time + id * 2.978);
    p.position.y = emitter.pos.y;
    p.position.z = emitter.pos.z + random(time + id * 2.978) ;
    const float Runout = 5.0;
    //p.position.x += (frac(random(id % 16)) - 0.5) * Runout;
    //p.position.z += (frac(random(id % 16)) - 0.5) * Runout;
    p.streak_factor = random(id) % 0.2;
   
  //初速度
    p.velocity = emitter.velocity;
    p.velocity += sin(id % 16);
    p.velocity = CurlNoise(p);
  
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
