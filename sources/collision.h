#pragma once

#include <DirectXMath.h>
#include "skeletal_mesh.h"


// �q�b�g����
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 };   // ���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 };   // �Փ˂����|���S���̖@���x�N�g��
    float distance = 0.0f;
    int material_index = -1;
};

class Collision
{
public:
    /*----< 2D >----*/
   //--�~���m�̓����蔻��--//
    static bool hit_check_circle(const DirectX::XMFLOAT2& pos1, float r1, const DirectX::XMFLOAT2& pos2, float r2);
    // ��`���m�̓����蔻��
    static bool hit_check_rect(const DirectX::XMFLOAT2& center_a, const DirectX::XMFLOAT2& radius_a,
        const DirectX::XMFLOAT2& center_b, const DirectX::XMFLOAT2& radius_b);
    /*----< 3D >----*/
    //--���Ƌ��̌�������--//
    static bool sphere_vs_sphere(
        const DirectX::XMFLOAT3& center_a, float radius_a,
        const DirectX::XMFLOAT3& center_b, float radius_b,
        DirectX::XMFLOAT3&  out_center_b);
    //--�~���Ɖ~���̌�������--//
    static bool cylinder_vs_cylinder(
        const DirectX::XMFLOAT3& position_a, float radius_a, float height_a,
        const DirectX::XMFLOAT3& position_b, float radius_b, float height_b,
        DirectX::XMFLOAT3& out_position_b);
    bool sphere_vs_cylinder(const DirectX::XMFLOAT3& sphere_position, float sphere_radius,
                            const DirectX::XMFLOAT3& cylinder_position, float cylinder_radius, float cylinder_height,
                            DirectX::XMFLOAT3& out_cylinder_position);

    //--���C�ƃ��f���̌�������--//
    static bool ray_vs_model(
        const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const SkeletalMesh* model,const SkeletalMesh::AnimeParam anime_para,
        const DirectX::XMFLOAT4X4 model_world_mat,
        HitResult& result);
};
