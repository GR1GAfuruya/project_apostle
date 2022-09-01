#include "gpu_particles.hlsli"
#include "boss_charge_attack.hlsli"
#include "constants.hlsli"


RWStructuredBuffer<particle> particle_buffer : register(u0);
ConsumeStructuredBuffer<uint> particle_pool : register(u1);


[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = particle_pool.Consume();
    particle p = particle_buffer[id];
    p.is_active = true;

    p.position.x = emitter.pos.x + sin(id % 36)*3 ;
    p.position.y = emitter.pos.y;
    p.position.z = emitter.pos.z + cos(id % 36)*3 ;
    
    p.velocity = sin(id % 16);
    p.velocity = CurlNoise(p);
    p.life_time = 5;
     //生成してからの時間をリセット
    p.time = 0;
    //パーティクルの大きさ
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = particle_color;
    p.color.a = 0;
    
    particle_buffer[id] = p;
}