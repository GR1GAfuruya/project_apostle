#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
#include "material_manager.h"
class MeshEffect
{
	public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
		DirectX::XMFLOAT2 scroll_direction = { 0,0 };
		float scroll_speed = 0;
		float threshold = 0;
	};

	struct TranslationParam
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//��]��
		DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
		//�X�P�[��
		DirectX::XMFLOAT3 scale = { 1,1,1 };
		//���x
		DirectX::XMFLOAT3 velosity = { 0,0,0 };
	};

	struct LifeParam
	{

	};

	//�I�u�W�F�N�g�̎�
	enum class AXIS
	{
		RIGHT,
		UP,
		FORWARD,
	};


public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	MeshEffect(Graphics& graphics, const char* fbx_filename);
	~MeshEffect(){};
	//�Đ�
	void play(DirectX::XMFLOAT3 pos);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time);
	//�`��
	void render(Graphics& graphics);

	void debug_gui(string str_id);



	//�������֐�
	void set_init_position(DirectX::XMFLOAT3 p) { init_translation_param.position = p; }
	void set_init_orientation(DirectX::XMFLOAT4 o) { init_translation_param.orientation = o; }
	void set_init_scale(DirectX::XMFLOAT3 s) { init_translation_param.scale = s; }
	void set_init_scale(float s) { init_translation_param.scale = { s,s,s }; }
	void set_init_color(DirectX::XMFLOAT4 c) { constants.get()->data.particle_color = c; }
	void set_init_velocity(DirectX::XMFLOAT3 v) { init_translation_param.velosity = v; }
	//Setter
	void set_position(DirectX::XMFLOAT3 p) { translation_param.position = p; }
	void set_orientation(DirectX::XMFLOAT4 o) { translation_param.orientation = o; }
	void set_scale(DirectX::XMFLOAT3 s) { translation_param.scale = s; }
	void set_scale(float s) { translation_param.scale = { s,s,s }; }
	void set_color(DirectX::XMFLOAT4 c) { constants.get()->data.particle_color = c; }
	void set_velocity(DirectX::XMFLOAT3 v) { translation_param.velosity = v; }
	void set_life_span(float l) { life_span = l; }
	void set_is_loop(bool loop) { is_loop = loop; }
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	void rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec);
	void set_material(Material* m) { material = m; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return translation_param.position; }
	DirectX::XMFLOAT4 get_orientation() { return translation_param.orientation; }
	DirectX::XMFLOAT3 get_scale() { return translation_param.scale; }
	DirectX::XMFLOAT3 get_velosity() { return translation_param.velosity; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }

	void reset_orientation();
protected:
	void dissolve_update(float elapsed_time);

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
public:
	std::unique_ptr<Constants<CONSTANTS>> constants{};
	//��]�X�s�[�h
	DirectX::XMFLOAT3 rot_speed = { 0,0,0 };
protected:

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	std::unique_ptr<MeshShader> shader = nullptr;
	// �X�P���^�����b�V���̎���
	std::unique_ptr <SkeletalMesh> model;

	Material* material;
	TranslationParam init_translation_param;
	TranslationParam translation_param;
	
	//�g�����X�t�H�[��
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	
	//��������
	float life_time = 0;
	//����
	float life_span = FLT_MAX;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//���[�v�Đ����邩�ǂ���
	bool is_loop = false;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;

	string instance_id;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

	const int MATERIAL_START_SLOT = 20;

};
