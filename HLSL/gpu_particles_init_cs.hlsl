#include "gpu_particles.hlsli"
#include "constants.hlsli"


RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> dead_list : register(u1);

[numthreads(16, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GT_id : SV_GroupThreadID)
{
    uint no = DTid.x;
    uint group_num = GT_id.x;
    particle_buffer[no].is_active = false;
    particle_buffer[no].scale = 0;
    particle_buffer[no].streak_factor = 0;
    dead_list.Append(no); // 未使用リスト(AppendStructuredBuffer)の末尾に追加
   
}