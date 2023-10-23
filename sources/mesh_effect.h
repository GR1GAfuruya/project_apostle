#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
#include "material_manager.h"
#include "camera.h"
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

	struct EffectParam
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//��]��
		DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
		//�X�P�[��
		DirectX::XMFLOAT3 scale = { 1,1,1 };
		//���x
		DirectX::XMFLOAT3 velosity = { 0,0,0 };
		//�J���[
		DirectX::XMFLOAT4 color = { 0,0,0,1 };
		//��������
		float life_duration = FLT_MAX;
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
	MeshEffect(const char* fbx_filename);
	~MeshEffect(){};
	//�Đ�
	void play(DirectX::XMFLOAT3 pos);
	//��~
	void stop();
	//�X�V
	void update(float elapsed_time);
	//�`��
	void render();//�t���X�^���J�����O��
	void render(Camera* camera);//�t���X�^���J�����O����

	void debug_gui(string str_id);


	//�������֐�
	void set_init_position(DirectX::XMFLOAT3 p) { init_effect_param.position = p; }
	void set_init_orientation(DirectX::XMFLOAT4 o) { init_effect_param.orientation = o; }
	void set_init_scale(DirectX::XMFLOAT3 s) { init_effect_param.scale = s; }
	void set_init_scale(float s) { init_effect_param.scale = { s,s,s }; }
	void set_init_color(DirectX::XMFLOAT4 c) { init_effect_param.color = c; }
	void set_init_velocity(DirectX::XMFLOAT3 v) { init_effect_param.velosity = v; }
	void set_init_life_duration(float d) { init_effect_param.life_duration = d; }
	void set_life_duration(float d) { effect_param.life_duration = d; }
	//Setter
	void set_position(DirectX::XMFLOAT3 p) { effect_param.position = p; }
	void set_orientation(DirectX::XMFLOAT4 o) { effect_param.orientation = o; }
	void set_scale(DirectX::XMFLOAT3 s) { effect_param.scale = s; }
	void set_scale(float s) { effect_param.scale = { s,s,s }; }
	void set_color(DirectX::XMFLOAT4 c) { constants.get()->data.particle_color = c; }
	void set_velocity(DirectX::XMFLOAT3 v) { effect_param.velosity = v; }
	void set_is_loop(bool loop) { is_loop = loop; }
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	void rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec);
	void set_material(Material* m) { material = m; };
	void set_dissolve_flag(bool flag) { is_dissolve = flag; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return effect_param.position; }
	DirectX::XMFLOAT4 get_orientation() { return effect_param.orientation; }
	DirectX::XMFLOAT3 get_scale() { return effect_param.scale; }
	DirectX::XMFLOAT3 get_velosity() { return effect_param.velosity; }
	DirectX::XMFLOAT4 get_color() { return constants.get()->data.particle_color; }
	float get_life_rate() { return life_time/ effect_param.life_duration; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }
	Material get_material() { return *material; }
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
	EffectParam init_effect_param;
	EffectParam effect_param;
	
	//�g�����X�t�H�[��
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	
	//��������
	float life_time = 0;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//���[�v�Đ����邩�ǂ���
	bool is_loop = false;
	//�f�B�]���u���邩�ǂ���
	bool is_dissolve = true;
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
