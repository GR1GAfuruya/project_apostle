#include "collision.h"
#include "user.h"
#include "camera.h"

bool Collision::hit_check_circle(const DirectX::XMFLOAT2& pos1, float r1, const DirectX::XMFLOAT2& pos2, float r2)
{
    float x = pos1.x - pos2.x;
    float y = pos1.y - pos2.y;
    float r = r1 + r2;

    return (x * x) + (y * y) <= (r * r);
}

bool Collision::hit_check_rect(const DirectX::XMFLOAT2& center_a, const DirectX::XMFLOAT2& radius_a,
    const DirectX::XMFLOAT2& center_b, const DirectX::XMFLOAT2& radius_b)
{
   

    const DirectX::XMFLOAT2 dis = { center_a.x - center_b.x, center_a.y - center_b.y };  //���S�̍������߂�
    const DirectX::XMFLOAT2 center_dif = { radius_a.x + radius_b.x, radius_a.y + radius_b.y };

    return abs(dis.x) <= center_dif.x && abs(dis.y) <= center_dif.y;
}

bool Collision::sphere_vs_sphere(const DirectX::XMFLOAT3& center_a, float radius_a,
    const DirectX::XMFLOAT3& center_b, float radius_b,
    DirectX::XMFLOAT3& out_center_b)
{
    
    using namespace DirectX;
    // B �� A �̒P�ʃx�N�g�����Z�o
    XMVECTOR position_a_vec = XMLoadFloat3(&center_a);
    XMVECTOR position_b_vec = XMLoadFloat3(&center_b);
    XMVECTOR vec = XMVectorSubtract(position_b_vec, position_a_vec);
    XMVECTOR length_sq_vec = XMVector3LengthSq(vec);   // XMVector3LengthSq()�͓�悳�ꂽ���̂��o��(sq��squad�̈�)
    float length_sq;
    XMStoreFloat(&length_sq, length_sq_vec);

    // ��������
    float range = radius_a + radius_b;
    if (length_sq >= range * range) return false;

    // A��B�������o��
    XMVECTOR norm_sq_vec = XMVector3Normalize(vec);
    XMVECTOR out_position_add = XMVectorScale(norm_sq_vec, range);
    XMVECTOR out_position_b_vec = XMVectorAdd(position_a_vec, out_position_add);

    XMStoreFloat3(&out_center_b, out_position_b_vec);

    return true;
}

bool Collision::cylinder_vs_cylinder(const DirectX::XMFLOAT3& position_a, float radius_a, float height_a,
    const DirectX::XMFLOAT3& position_b, float radius_b, float height_b,
    DirectX::XMFLOAT3& out_position_b)
{
   
    using namespace DirectX;
    // A�̑�����B�̓�����Ȃ瓖�����ĂȂ�
    if (position_a.y > position_b.y + height_b) { return false; }
    // A�̓���B�̑�����艺�Ȃ瓖�����ĂȂ�
    if (position_a.y + height_a < position_b.y) { return false; }
    // XZ���ʂł͈̔̓`�F�b�N
    float x = position_a.x - position_b.x;
    float z = position_a.z - position_b.z;
    float range = radius_a + radius_b;
    if ((x * x) + (z * z) >= (range * range)) return false;
    // A��B�������o��
    XMFLOAT3 positionA = { position_a.x, 0, position_a.z };
    XMFLOAT3 positionB = { position_b.x, 0, position_b.z };
    // XMVECTOR�ɕϊ�
    XMVECTOR position_a_vec = XMLoadFloat3(&positionA);
    XMVECTOR position_b_vec = XMLoadFloat3(&positionB);
    // �|�W�V����A�ƃ|�W�V����B�̍��̃x�N�g�������߂�
    XMVECTOR vec = XMVectorSubtract(position_b_vec, position_a_vec);
    // ���K��
    XMVECTOR norm_sq_vec = XMVector3Normalize(vec);
    XMVECTOR out_position_add = XMVectorScale(norm_sq_vec, range);
    XMVECTOR out_position_b_vec = XMVectorAdd(position_a_vec, out_position_add);

    XMFLOAT3 out_positionB;
    XMStoreFloat3(&out_positionB, out_position_b_vec);

    out_position_b.x = out_positionB.x;
    out_position_b.y = position_b.y;
    out_position_b.z = out_positionB.z;

    return true;
}

bool Collision::sphere_vs_cylinder(const DirectX::XMFLOAT3& sphere_position, float sphere_radius,
    const DirectX::XMFLOAT3& cylinder_position, float cylinder_radius, float cylinder_height,
    DirectX::XMFLOAT3& out_cylinder_position)
{
   
    using namespace DirectX;
    // A�̋��̉���B�̓�����Ȃ瓖�����ĂȂ�
    if (sphere_position.y - sphere_radius > cylinder_position.y + cylinder_height) { return false; }
    //  A�̋��̏オB�̑�����艺�Ȃ瓖�����ĂȂ�
    if (sphere_position.y + sphere_radius < cylinder_position.y) { return false; }

    // XZ���ʂł͈̔̓`�F�b�N
    float x = sphere_position.x - cylinder_position.x;
    float z = sphere_position.z - cylinder_position.z;
    float range = sphere_radius + cylinder_radius;
    if ((x * x) + (z * z) >= (range * range)) return false;
    // A��B�������o��
    XMFLOAT3 S_position = { sphere_position.x, 0, sphere_position.z };
    XMFLOAT3 C_position = { cylinder_position.x, 0, cylinder_position.z };
    // XMVECTOR�ɕϊ�
    XMVECTOR s_position_vec = XMLoadFloat3(&S_position);
    XMVECTOR c_position_vec = XMLoadFloat3(&C_position);
    // �|�W�V����S�ƃ|�W�V����C�̍��̃x�N�g�������߂�
    XMVECTOR vec = XMVectorSubtract(c_position_vec, s_position_vec);
    // ���K��
    XMVECTOR norm_sq_vec = XMVector3Normalize(vec);
    XMVECTOR out_position_add = XMVectorScale(norm_sq_vec, range);
    XMVECTOR out_c_position_vec = XMVectorAdd(s_position_vec, out_position_add);

    XMFLOAT3 out_c_position;
    XMStoreFloat3(&out_c_position, out_c_position_vec);

    out_cylinder_position.x = out_c_position.x;
    out_cylinder_position.y = cylinder_position.y;
    out_cylinder_position.z = out_c_position.z;

    return true;
}

bool Collision::ray_vs_model(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const SkeletalMesh* model, const SkeletalMesh::AnimeParam anime_para,
    const DirectX::XMFLOAT4X4 model_world_mat, HitResult& result)
{
    using namespace DirectX;
    XMVECTOR world_start_vec = XMLoadFloat3(&start);
    XMVECTOR world_end_vec = XMLoadFloat3(&end);
    XMVECTOR world_ray_vec = world_end_vec - world_start_vec;
    XMVECTOR world_ray_length_vec = XMVector3Length(world_ray_vec);
    // ���[���h��Ԃ̃��C�̒���(������Ȃ������ꍇ�̒���)
    XMStoreFloat(&result.distance, world_ray_length_vec);

    bool hit = false;
    for (const ModelResource::mesh& mesh : model->model_resource->get_meshes())
    {
        // ���b�V���m�[�h�擾
        // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        XMMATRIX world_trans_mat;
        if (&anime_para.animation_keyframe && (&anime_para.animation_keyframe)->nodes.size() > 0) // �A�j���[�V��������
        {
            const animation::keyframe::node& mesh_node{ (&anime_para.animation_keyframe)->nodes.at(mesh.node_index) };
            world_trans_mat = XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&model_world_mat);
        }
        else // �A�j���[�V�����Ȃ�
        {
            world_trans_mat = XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&model_world_mat);
        }
        // ���[���h�s��̋t�s��
        XMMATRIX inverse_world_trans_mat = XMMatrixInverse(nullptr, world_trans_mat);

        XMVECTOR start_vec = XMVector3TransformCoord(world_start_vec, inverse_world_trans_mat);
        XMVECTOR end_vec = XMVector3TransformCoord(world_end_vec, inverse_world_trans_mat);
        XMVECTOR vec = XMVectorSubtract(end_vec, start_vec);
        XMVECTOR dir_vec = XMVector3Normalize(vec);
        XMVECTOR length_vec = XMVector3Length(vec);
        // ���C�̒���
        float near_t;
        XMStoreFloat(&near_t, length_vec);
        // �O�p�`(��)�Ƃ̌�������
        const std::vector<ModelResource::vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int material_index = -1;
        XMVECTOR hit_position;
        XMVECTOR hit_normal;
        //subset: �����}�e���A�����W�܂����|���S���̏W��
        for (const ModelResource::mesh::subset& subset : mesh.subsets)
        {
            // indexCount:�����_��
            for (UINT i = 0; i < subset.index_count; i += 3)
            {
                UINT index = subset.start_index_location + i;
                // �O�p�`�̒��_�𒊏o
                const ModelResource::vertex& a = vertices.at(indices.at(index));
                const ModelResource::vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::vertex& c = vertices.at(indices.at(index + 2));
                // convert to XMVECTOR
                XMVECTOR a_vec = XMLoadFloat3(&a.position);
                XMVECTOR b_vec = XMLoadFloat3(&b.position);
                XMVECTOR c_vec = XMLoadFloat3(&c.position);
                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                XMVECTOR ab_vec = XMVectorSubtract(b_vec, a_vec);
                XMVECTOR bc_vec = XMVectorSubtract(c_vec, b_vec);
                XMVECTOR ca_vec = XMVectorSubtract(a_vec, c_vec);
                // �O�p�`�̖@���x�N�g�����Z�o
                XMVECTOR normal_vec = XMVector3Cross(ab_vec, bc_vec);
                // ���ς̌��ʂ����łȂ���Η�����(���C�L���X�g���Ȃ�)
                XMVECTOR dot_vec = XMVector3Dot(dir_vec, normal_vec);
                float dot;
                XMStoreFloat(&dot, dot_vec);
                if (dot >= 0) continue;
                // ���C�ƕ��ʂ̌�_���Z�o
                XMVECTOR v_vec = XMVectorSubtract(a_vec, start_vec);
                XMVECTOR t_vec = XMVectorDivide(XMVector3Dot(v_vec, normal_vec), dot_vec);
                float t;
                XMStoreFloat(&t, t_vec);
                if (t < 0.0f || t > near_t) continue;  // ��_�܂ł̋��������܂łɌv�Z����
                                                       // �ŋߋ������傫���ꍇ�̓X�L�b�v
                XMVECTOR position_vec = XMVectorAdd(start_vec, XMVectorMultiply(dir_vec, t_vec));
                // ��_���O�p�`�̓����ɂ��邩����
                // 1��
                {
                    XMVECTOR v1_vec = a_vec - position_vec;
                    XMVECTOR cross1_vec = XMVector3Cross(v1_vec, ab_vec);
                    XMVECTOR dot1_vec = XMVector3Dot(cross1_vec, normal_vec);
                    float dot1;
                    XMStoreFloat(&dot1, dot1_vec);
                    if (dot1 < 0.0f) continue;
                }
                // 2��
                {
                    XMVECTOR v2_vec = b_vec - position_vec;
                    XMVECTOR cross2_vec = XMVector3Cross(v2_vec, bc_vec);
                    XMVECTOR dot2_vec = XMVector3Dot(cross2_vec, normal_vec);
                    float dot2;
                    XMStoreFloat(&dot2, dot2_vec);
                    if (dot2 < 0.0f) continue;
                }
                // 3��
                {
                    XMVECTOR v3_vec = c_vec - position_vec;
                    XMVECTOR cross3_vec = XMVector3Cross(v3_vec, ca_vec);
                    XMVECTOR dot3_vec = XMVector3Dot(cross3_vec, normal_vec);
                    float dot3;
                    XMStoreFloat(&dot3, dot3_vec);
                    if (dot3 < 0.0f) continue;
                }
                // �ŋߋ������X�V
                near_t = t;
                // ��_�Ɩ@�����X�V
                hit_position = position_vec;
                hit_normal = normal_vec;
                material_index = static_cast<int>(subset.material_unique_id);
            }
        }
        if (material_index >= 0)
        {
            // ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            XMVECTOR world_position_vec = XMVector3TransformCoord(hit_position, world_trans_mat);
            XMVECTOR world_cross_vec = world_position_vec - world_start_vec;
            XMVECTOR world_cross_length_vec = XMVector3Length(world_cross_vec);
            float distance;
            XMStoreFloat(&distance, world_cross_length_vec);
            // �q�b�g���ێ�
            if (result.distance > distance)
            {
                XMVECTOR world_normal_vec =
                    XMVector3TransformNormal(hit_normal, world_trans_mat);
                result.distance = distance;
                result.material_index = material_index;
                XMStoreFloat3(&result.position, world_position_vec);
                XMStoreFloat3(&result.normal, XMVector3Normalize(world_normal_vec));
                hit = true;
            }
        }
    }
    return hit;
}
