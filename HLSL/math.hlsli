//ƒ^ƒCƒŠƒ“ƒO
float2 tex_coord(float2 uv,float2 tile)
{
    float2 tex_coord = uv;
    tex_coord.x *= tile.x;
    tex_coord.y *= tile.y;
    return tex_coord;

}