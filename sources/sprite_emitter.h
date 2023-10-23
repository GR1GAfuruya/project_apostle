#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
//#include "data_file.h"
class SpriteEmitter
{
public:
	//==============================================================
	// 
	// �\����
	// 
	//==============================================================

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
	// public�֐�
	// 
	//==============================================================
	SpriteEmitter(InitParam param, int max_particles);
	~SpriteEmitter();

	void play(DirectX::XMFLOAT3 pos);


	void emit(float elapsed_time);

	void update(float elapsed_time);

	void render(Camera& camera);

	void debug_gui(string id);

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


	InitParam init_param;
private:
	// ���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Instance
	{
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT2 tip_texcoord;
	};

	struct OBJECT_CONSTANTS
	{
		DirectX::XMFLOAT4X4 world;
	};

	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	void remove_update();

	void ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data);

	void create_com_object(int max_particles);
	//�ʒu
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//�p��
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//�傫��
	DirectX::XMFLOAT3 scale = { 1,1,1 };
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
	//�摜�ʒu
	DirectX::XMFLOAT2 texpos = {  };
	//�摜�T�C�Y
	DirectX::XMFLOAT2 texsize = {  };

	float animation_rate = 1;

	//�p�[�e�B�N�����i�[����R���e�i
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;
	//COM�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_data;

	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	std::vector<vertex> vertices;
	std::unique_ptr<Instance[]> CPU_instance_data;

	const int MATERIAL_START_SLOT = 20;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;
	//�q�̃G�~�b�^�[�������ł���p�[�e�B�N���̍ő吔
	int max_particles;
	//�A�N�e�B�u��Ԃ̃p�[�e�B�N����
	int active_count;
	//�p�[�e�B�N���̏����p�����[�^�[
	Particles::Param particle_init_param;

};