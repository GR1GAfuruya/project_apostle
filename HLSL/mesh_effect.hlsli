#include "default_mesh.hlsli"
cbuffer MESH_EFFECT_CONSTANT_BUFFER : register(b9)
{
   float4 particle_color;
   float2 scroll_direction;
   float scroll_speed;
   float threshold;
};
