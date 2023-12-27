#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
#include "component.h"
class Emitter :public Component
{
public:
	Emitter(int max_particles);
	virtual ~Emitter();
	virtual const char* get_name() const override { return "Emitter"; }

	virtual void play(DirectX::XMFLOAT3 pos);

	virtual void start() {}

	virtual void emit(float elapsed_time);

	virtual void update(float elapsed_time);

	virtual void render(Camera& camera);

	virtual void debug_gui(string id);

protected:
	virtual void emitter_update(float elapsed_time) {}

	virtual void particle_spawn(float elapsed_time) {}

	virtual void particle_update(float elapsed_time){}

	//==============================================================
	// 
	// �Z�b�^�[�ƃQ�b�^�[
	// 
	//==============================================================

	//Setter
	inline void set_emit_span(float span) { emit_span = span; }
	inline void set_is_loop(bool loop) { is_loop = loop; }

	float get_duration() { return duration; }
	bool get_active() { return active; }

	// ���_�t�H�[�}�b�g

	struct Param
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//�ˏo����
		DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
		//����
		float duration = 5;
		//���[�v�Đ����邩�ǂ���
		bool is_loop = false;
		//�����������鎞��
		float emit_time;
		// 1�b�Ԃɉ����������邩
		float emit_rate;
		// �����Ԋu
		float emit_span;
		//��x�ɕ��o���鐔
		int burst_num = 1;
		//�����J�n����
		float emit_start_time = 0;

		int TileX = 1;
		int TileY = 1;
		int tile_num = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("position", position),
				cereal::make_nvp("emit_dir", emit_dir),
				cereal::make_nvp("duration", duration),
				cereal::make_nvp("emit_span", emit_span),
				cereal::make_nvp("emit_start_time", emit_start_time)
			);
		}

	};
	//==============================================================
	// 
	//�@�p�u���b�N�ϐ�
	// 
	//==============================================================
public:
	Transform transform;
	Param param;
	//==============================================================
	// 
	//�@�v���e�N�e�b�h�ϐ�
	// 
	//==============================================================
protected:
	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	void remove_update();
	//���x
	DirectX::XMFLOAT3 velocity;

	//�O��p�[�e�B�N���𐶐����Ă���̎���
	float emit_timer = 0;
	//�G�~�b�^�[���������Ă���̎���
	float life_timer = 1;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//���[�v�Đ����邩�ǂ���
	bool is_loop = false;
	//�����������鎞��
	float duration;
	// 1�b�Ԃɉ����������邩
	float emit_rate; 
	// �����Ԋu
	float emit_span;
	// ���݂̔����J�E���g
	int emit_count; 
	//��x�ɕ��o���鐔
	int burst_num = 1;
	//����
	float timer = 0;
	//�����J�n����
	float emit_start_time;
	
	//�p�[�e�B�N�����i�[����R���e�i
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	
	
	const int MATERIAL_START_SLOT = 20;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;

	int max_particles;
	int active_count;

};