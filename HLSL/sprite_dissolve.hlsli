struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer SCROLL_CONSTANT_BUFFER : register(b2)
{
    float2 scroll_direction;
    float2 scroll_speed;
    float dissolve_value;
}
