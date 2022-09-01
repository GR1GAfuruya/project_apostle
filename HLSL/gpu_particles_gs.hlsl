#include "gpu_particles.hlsli"
#include "constants.hlsli"
StructuredBuffer<particle> particle_buffer : register(t9);

float3 rotate(float3 ang)
{
    float3 r = float3(cos(ang.y),cos(ang.x),0);
    return r;
}

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    const float3 billboard[] =
    {
        float3(-1.0f, -1.0f, 1.0f), // Bottom-left corner
		float3(+1.0f, -1.0f, 1.0f), // Bottom-right corner
		float3(-1.0f, +1.0f, 1.0f), // Top-left corner
		float3(+1.0f, +1.0f, 1.0f), // Top-right corner
    };
    const float2 texcoords[] =
    {
        float2(0.0f, 1.0f), // Bottom-left 
		float2(1.0f, 1.0f), // Bottom-right
		float2(0.0f, 0.0f), // Top-left
		float2(1.0f, 0.0f), // Top-right
    };
    
    particle p = particle_buffer[input[0].vertex_id];

    float3 view_space_velocity = mul(float4(p.velocity, 0.0f), view).xyz;
   // p.position = snoise(p.position.xyz * frac(time) * 5.0);
    float4 view_space_pos = mul(float4(p.position, 1.0), view);

	[unroll]
    for (uint vertex_index = 0; vertex_index < 4; ++vertex_index)
    {
        GS_OUT element;

        float3 corner_pos;
        
        corner_pos.x = (cos(-p.angle.z) * billboard[vertex_index].x + -sin(-p.angle.z) * billboard[vertex_index].y) * p.scale.x; //‰ñ“]•ûŒü’²®‚Ì‚½‚ßangle‚É-‚©‚¯‚Ä‚é
        corner_pos.y = (sin(-p.angle.z) * billboard[vertex_index].x + cos(-p.angle.z) * billboard[vertex_index].y) * p.scale.y;
        corner_pos.z = billboard[vertex_index].z;
        
        //float3 corner_pos = billboard[vertex_index] * particle_size.x;
        float3 streak = max(0, dot(normalize(-view_space_velocity), normalize(corner_pos))) * -view_space_velocity;
        corner_pos = corner_pos + p.streak_factor * streak;
       
        element.position = mul(float4(view_space_pos.xyz + corner_pos, 1), projection);
        element.color = p.color;
        element.texcoord = texcoords[vertex_index];
        output.Append(element);
    }
    output.RestartStrip();
}

