#include "field_spark_particles.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<particle> spark_particle_buffer : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    particle p = spark_particle_buffer[id];

    if (length(p.position.xz - current_eye_position.xz) > outermost_radius)
    {
        p.position.xz = current_eye_position.xz - (p.position.xz - previous_eye_position.xz);
    }
	
    const float amplitude = 2.0;
   
    p.velocity = CurlNoise(p) * amplitude;
    
    p.velocity.x = min(p.velocity.x, 3);
    p.velocity.y = min(p.velocity.y, 3);
    p.velocity.z = min(p.velocity.z, 3);
    
    p.velocity.x = max(p.velocity.x, -3);
    p.velocity.y = max(p.velocity.y, -3);
    p.velocity.z = max(p.velocity.z, -3);
    
    
    p.position += p.velocity * delta_time;
    float alpha;
    alpha = smoothstep(0.0, 1.0, sin(dtid.x % 16 * time / 10));
    alpha = min(alpha, 0.7);
    p.color = float4(5.0, 0.6, 0.0, 1);
    p.streak_factor = 0.07;
    p.time += delta_time;

    if (p.position.y < current_eye_position.y - spark_area_height * 0.5)
    {
        p.position.y += spark_area_height;
    }
    else if (p.position.y > current_eye_position.y + spark_area_height * 0.5)
    {
        p.position.y -= spark_area_height;
    }

    spark_particle_buffer[id] = p;
}