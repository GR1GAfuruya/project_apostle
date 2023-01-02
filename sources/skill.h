#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "primitive.h"
#include "mesh_effect.h"
#include "light.h"
#include "user.h"
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

	virtual void initialize(Graphics& graphics) {};
	virtual void update(Graphics& graphics, float elapsed_time) = 0;
	virtual void render(Graphics& graphics) = 0;

	//�X�L�����I���������ǂ���
	bool is_skill_end() { return skill_end_flag; };

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
	float get_power() { return power; }
	float get_invinsible_time() { return invinsible_time; }
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
	//�X�L�����I��������
	bool skill_end_flag = false;
	//�X�L�����q�b�g�������ǂ���
	bool is_hit = false;
	//�U����
	float power = 0.0f;
	//�Ώۂɗ^���閳�G����
	float invinsible_time = 0.0f;
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