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
   
    //�J�[���m�C�Y
    p.velocity = CurlNoise(p);
    p.velocity.y = min(p.velocity.y, 20);
    p.velocity.y += 5.0;
    //��������m
    p.life_time = random(id);
    //�������Ă���̎��Ԃ����Z�b�g
    p.time = 0;
    //�p�[�e�B�N���̑傫��
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = particle_color;
    particle_buffer[id] = p;
}
