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

    p.position = emitter.pos + (id % 32) /** random(10.0)*/;
    
    p.velocity = CurlNoise(p);
    
     //生成してからの時間をリセット
    p.time = 0;
    //パーティクルの大きさ
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = particle_color;
    
    particle_buffer[id] = p;
}