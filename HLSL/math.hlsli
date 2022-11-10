//ƒ^ƒCƒŠƒ“ƒO
float2 tex_coord(float2 uv,float2 tile)
{
    float2 tex_coord = uv;
    tex_coord.x *= tile.x;
    tex_coord.y *= tile.y;
    return tex_coord;

}

float4x4 matrix_rotation_quaternion(float4 quaternion)
{
    float m00 = (2.0 * quaternion.w * quaternion.w) + (2.0 * quaternion.x * quaternion.x) - 1;
    float m01 = (2.0 * quaternion.x * quaternion.y) + (2.0 * quaternion.z * quaternion.w);
    float m02 = (2.0 * quaternion.x * quaternion.z) - (2.0 * quaternion.y * quaternion.w);

    float m10 = (2.0 * quaternion.x * quaternion.y) - (2.0 * quaternion.z * quaternion.w);
    float m11 = (2.0 * quaternion.w * quaternion.w) + (2.0 * quaternion.y * quaternion.y) - 1;
    float m12 = (2.0 * quaternion.y * quaternion.z) + (2.0 * quaternion.x * quaternion.w);

    float m20 = (2.0 * quaternion.x * quaternion.z) + (2.0 * quaternion.y * quaternion.w);
    float m21 = (2.0 * quaternion.y * quaternion.z) - (2.0 * quaternion.x * quaternion.w);
    float m22 = (2.0 * quaternion.w * quaternion.w) + (2.0 * quaternion.z * quaternion.z) - 1;

    return float4x4(m00, m01, m02, 0,
                    m10, m11, m12, 0,
                    m20, m21, m22, 0,
                    0, 0, 0, 1);
}

float4x4 matrix_scalling(float3 scale)
{
    return float4x4(
       scale.x, 0, 0, 0,
       0, scale.y, 0, 0,
       0, 0, scale.z, 0,
       0, 0, 0, 1);
}

float4x4 matrix_transform(float3 position)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        position.x, position.y, position.z, 1);
}

float4x4 calc_world_transform(float3 scale, float4 quaternion, float3 position)
{
    return float4x4(mul(mul(matrix_scalling(scale), matrix_rotation_quaternion(quaternion)), matrix_transform(position)));
}


