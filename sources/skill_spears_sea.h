#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
#include "light.h"
class SpearsSea : public Skill
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
		//���̒���
		float spear_length;
		//�����̐L�ї�
		float spear_length_rate;
		//�^�[�Q�b�g�Ɍ������X�s�[�h
		float speed;
		//�Ǐ]���鎞��
		float follow_time;

	};
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam);
	~SpearsSea();

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics, Camera* camera)override;

	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================
	void spear_emit(int index_offset, int emit_max_num);

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
	static constexpr int MAX_NUM = 60;
	static constexpr float FOLLOW_TIME = 0.7f;
	//static constexpr DirectX::XMFLOAT3 SPEAR_SIZE = { 0.5f,0.5f,1.0f };

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	//�C���X�^���V���O���b�V��
	std::unique_ptr<InstanceMeshEffect> instance_mesh;
	//�|�C���g���C�g
	std::shared_ptr<PointLight> spear_light;

	PublicParam param;
	//�����o������
	int emit_num;
	//�t�B�j�b�V���������̃t���O
	bool finish;
	//�Ǐ]���鎞��
	float follow_timer;
	//���̐L�т钷��
	float spear_maximum_extension = 0.6f;
	//�^�[�Q�b�g�̈ʒu
	DirectX::XMFLOAT3 target_position;
	//���o�����ŏ��̃t���[���̂ݍs�������Ɏg���ϐ�
	bool init_flag;
	int init_emit_num;
	
};