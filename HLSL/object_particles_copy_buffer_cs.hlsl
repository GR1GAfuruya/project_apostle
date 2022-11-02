#include "object_particles.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<object_particle> object_particle_buffer : register(u0);
RWStructuredBuffer<particle> particle_buffer : register(u1);
RWStructuredBuffer<uint> completed_particle_buffer : register(u2);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    particle_buffer[dtid.x].position = object_particle_buffer[dtid.x].position;
    particle_buffer[dtid.x].color = object_particle_buffer[dtid.x].color;
	particle_buffer[dtid.x].velocity = 0;
	particle_buffer[dtid.x].state = 0;
	particle_buffer[dtid.x].age = 0;
}