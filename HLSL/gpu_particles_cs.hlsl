#include "gpu_particles.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    particle p = particle_buffer[id];

    const float amplitude = 0.3;
    p.velocity.x = snoise(p.position.xyz * frac(time) * 10.0) * amplitude;
    p.velocity.z = snoise(p.position.zyx * frac(time) * 10.0) * amplitude;
    p.is_active = true;
    if (p.is_active)
    {
        
    }
    p.time += delta_time;
    p.position += p.velocity * delta_time;
    //if (p.time > p.life_time)
    //{
    //    p.position = emitter.pos;
    //    p.time = 0;
    //}


    particle_buffer[id] = p;
}

//void emit()