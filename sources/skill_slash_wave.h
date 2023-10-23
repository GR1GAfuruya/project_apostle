#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
#include "light.h"
class SlashWave : public Skill
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct PublicParam
	{
		//��������
		float skill_duration;
		//���a
		float radius;
		//�����蔻��̔��a
		float collider_radius;
		//�^�[�Q�b�g�Ɍ������X�s�[�h
		float speed;
	};
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SlashWave(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* target_dir, PublicParam initparam);
	~SlashWave();

	void initialize() override;
	void update(float elapsed_time)override;
	void render(Camera* camera)override;

	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	//�C���X�^���V���O���b�V��
	std::unique_ptr<MeshEffect> slash_mesh;
	//�|�C���g���C�g
	std::shared_ptr<PointLight> slash_light;

	PublicParam param;

	//�^�[�Q�b�g�̈ʒu
	std::unique_ptr<DirectX::XMFLOAT3> launch_position;
	std::unique_ptr<DirectX::XMFLOAT3> target_direction;


};
