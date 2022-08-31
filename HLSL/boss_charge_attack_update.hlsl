#include "gpu_particles.hlsli"
#include "boss_charge_attack.hlsli"
#include "constants.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> dead_list : register(u1);
[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particle p = particle_buffer[DTid.x];
    if (p.is_active)
    {
        //�R�A�ɋz�����܂�Ă��������̕\��
        float3 dir_vec = core_pos - p.position;
        float3 dir_norm_vec = normalize(dir_vec);
        
        float3 core_grav_velocity = dir_norm_vec * core_gravitation;
        p.velocity += core_grav_velocity;
        //�J�[���m�C�Y
        p.velocity += CurlNoise(p);
        //�ʒu�X�V
        p.position += p.velocity * delta_time;
        //��������
        p.life_time = max(0, p.life_time - delta_time);
        p.time += delta_time;
         //�������s�����疢�g�p���X�g�֖߂�
        if (length(dir_vec) < 5)
        {
            p.velocity = float3(0, 0, 0);
            p.is_active = false;
            p.scale = 0;
            p.streak_factor = 0;
            p.time = 0;
            dead_list.Append(DTid.x);
        }
    }   
    
    particle_buffer[DTid.x] = p;
}