#pragma once
#include "instance_mesh_effect.h"
#include "move_behavior.h"
#include "primitive.h"
#include "damage_func.h"
#include"mesh_effect.h"
class Meteore :public MoveBehavior
{
public:
	//==============================================================
	// 
	// �\����
	// 
	//==============================================================
	struct MeteoreParam
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 move_vec;
		DirectX::XMFLOAT3 scale;
		bool is_calc_velocity = false;
		bool is_hit = false;
		Sphere colider_sphere;
	};

	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================

	Meteore(Graphics& graphics,int max_num);
	~Meteore() {};

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui();
	//����
	void create(DirectX::XMFLOAT3 position, int index) { params[index].position = position; }
	void create_on_circle(DirectX::XMFLOAT3 center, float radius, int index);

	//�㏸
	void rising(float elapsed_time,DirectX::XMFLOAT3 target_position, float target_scale, float rise_speed,int index);

	//�ʂɎˏo
	void launch(DirectX::XMFLOAT3 init_vec, float speed,int index);
	//��Ďˏo
	void all_launch(DirectX::XMFLOAT3 init_vec, float speed);

	//�ʒu�擾
	const DirectX::XMFLOAT3& get_position(int index) const { return params[index].position; }
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position, int index) { this->params[index].position = position; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3& get_scale(int index) const { return params[index].scale; }
	//velocity�擾
	const DirectX::XMFLOAT3& get_velocity(int index) const { return params[index].velocity; }

	const int get_max_num() const { return MAX_NUM; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT3& scale, int index) { this->params[index].scale = scale; }
	//���˒��̂ݑ��x�v�Z
	void update_velocity(float elapsed_time, int index);
	//�v���C���[�̍U���Ƃ̓����蔻��
	void calc_meteore_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);


private:
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================
	//�ړ��Ɋւ���p�����[�^�[���Z�b�g
	void move(float vx, float vz, float speed, int index);
	//�������͍X�V����
	void update_vertical_velocity(float elapsed_frame, int index) ;
	//�����ړ��X�V����
	void update_vertical_move(float elapsed_time, int index);
	//�������͍X�V����
	void update_hrizontal_velocity(float elapsed_frame, int index) ;
	//�����ړ��X�V����
	void update_horizontal_move(float elapsed_time, int index) ;
	//覐΂������������̏���
	void on_hit(int index);
	//==============================================================
	// 
	// private�ϐ�
	// 
	//==============================================================
	std::unique_ptr<InstanceMeshEffect> main_effect;
	std::vector<std::unique_ptr<MeshEffect>> meteo_wave;
	std::unique_ptr<MeteoreParam[]> params;
	float ray_power = 5.0f;

	int MAX_NUM;
	float radius;
	//�U����
	int power;
	//�t�^���G����
	float invinsible_time;

	bool show_imgui;

};