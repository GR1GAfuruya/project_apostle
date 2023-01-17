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
   
    //�J�[���m�C�Y
    p.velocity = emitter.velocity;
    //��������
    p.life_time = emitter.emit_life_time;
    //�������Ă���̎��Ԃ����Z�b�g
    p.time = 0;
    //�p�[�e�B�N���̑傫��
    p.scale.xy = particle_size.xy;
    p.scale.z = 1;
    p.angle = angle;
        
    p.color = emitter.particle_color;
    
    particle_buffer[id] = p;
}
