#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
class Emitter
{
public:
	Emitter(Graphics& graphics, int max_particles);
	~Emitter();

	void create_emitter(Graphics& graphics);

	void emit(Graphics& graphics);

	void update(Graphics& graphics, float elapsed_time);

	void render(Graphics& graphics, Camera& camera);

	void debug_gui(Graphics& graphics, string id);

	//==============================================================
	// 
	// �Z�b�^�[�ƃQ�b�^�[
	// 
	//==============================================================

	//Setter
	inline void set_position(DirectX::XMFLOAT3 p) { position = p; }
	inline void set_emit_span(float span) { emit_span = span; }
	inline void set_is_loop(bool loop) { is_loop = loop; }
	inline void set_material(Material* m) { material = m; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }

	struct InitParam
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//�ˏo����
		DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
		//����
		float life_time = FLT_MAX;
		//���[�v�Đ����邩�ǂ���
		bool is_loop = false;
		//�����������鎞��
		float emit_time;
		// 1�b�Ԃɉ����������邩
		float emit_rate;
		// �����Ԋu
		float emit_span;

	};

private:
	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	void remove_update();

	//�ʒu
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//�ˏo����
	DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
	//���x
	DirectX::XMFLOAT3 velocity;

	//��������Ă���̎���
	float timer = 0;
	//����
	float life_time = 1;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//���[�v�Đ����邩�ǂ���
	bool is_loop = false;
	//�����������鎞��
	float emit_time;
	// 1�b�Ԃɉ����������邩
	float emit_rate; 
	// �����Ԋu
	float emit_span;
	// ���݂̔����J�E���g
	int emit_count; 

	//�p�[�e�B�N�����i�[����R���e�i
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;

	Material* material;
	std::unique_ptr<SpriteBatch> sprite = nullptr;

	const int MATERIAL_START_SLOT = 20;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;

	int max_particles;

};