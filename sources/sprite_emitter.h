#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
class SpriteEmitter
{
public:
	SpriteEmitter(Graphics& graphics, int max_particles);
	~SpriteEmitter();

	void play(DirectX::XMFLOAT3 pos);


	void emit(Graphics& graphics, float elapsed_time);

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
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	float get_duration() { return duration; }
	bool get_active() { return active; }

	// ���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct InitParam
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
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//ID3D11Buffer* vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	vertex* vertices;

	const int MATERIAL_START_SLOT = 20;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;

	int max_particles;
	int active_count;

};