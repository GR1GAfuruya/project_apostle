#include "constants.hlsli"
#include "sprite_particle.hlsli"

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
    

    
    float4 view_space_pos = mul(float4(input[0].position.xyz, 1.0), view_projection);

	[unroll]
    for (uint vertex_index = 0; vertex_index < 4; ++vertex_index)
    {
        GS_OUT element;

        
        float3 corner_pos = billboard[vertex_index] * 10;
     
        element.position = mul(float4(view_space_pos.xyz + corner_pos, 1), projection);
        element.color = input[0].color;
        element.texcoord = texcoords[vertex_index];
        output.Append(element);
    }
    output.RestartStrip();
}