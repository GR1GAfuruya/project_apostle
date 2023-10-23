#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "primitive.h"
#include "mesh_effect.h"
#include "light.h"
#include "user.h"
#include "damage_func.h"
class Skill
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	enum class CollisionType
	{
		SPHERE,
		CYLINDER,
		CUBE,
		CAPSULE,

	};


	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================

	Skill() {};
	virtual ~Skill() {};

	virtual void initialize() {};
	virtual void update(float elapsed_time) = 0;
	virtual void render(Camera* camera) = 0;

	//�X�L�����I���������ǂ���
	bool is_skill_end() { return skill_end_flag; };
	//�X�L�����q�b�g���Ă��邩
	bool is_skill_hit() { return is_hit; };

	CollisionType get_collision_type() { return collision_type; }
	//�����蔻����̎擾
	Capsule& get_colider() { return attack_colider; }

	//�|�W�V�����̃Q�b�^�[
	const DirectX::XMFLOAT3 get_position() { return position; }
	//�X�L�����q�b�g�����Ƃ��ɌĂԊ֐�
	virtual void skill_hit() {};
	//�f�o�b�OGUI
	virtual void debug_gui(string str_id) = 0;
	void set_is_skill_hit(bool h) { is_hit = h; }
protected:


	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	//�����蔻��̃^�C�v
	CollisionType collision_type{};
	//�����蔻��i�J�v�Z���j
	Capsule attack_colider{};
	//�X�L���̎���
	float life_time = 0;
	//�X�L���̎���
	float skill_duration = 0;

	//�X�L�����I��������
	bool skill_end_flag = false;
	//�X�L�����q�b�g�������ǂ���
	bool is_hit = false;
	//�ʒu
	DirectX::XMFLOAT3 position{};
	//����
	DirectX::XMFLOAT3 velocity{};
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};