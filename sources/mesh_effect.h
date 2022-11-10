#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
class MeshEffect
{
private:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
		DirectX::XMFLOAT2 scroll_direction = {0,0};
		float scroll_speed = 0;
		float threshold = 0;
	};
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	//MeshEffect(){}
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

	//�I�u�W�F�N�g�̎�
	enum class AXIS
	{
		RIGHT,
		UP,
		FORWARD,
	};
	//Setter
	void set_position(DirectX::XMFLOAT3 p) { position = p; }
	void set_orientation(DirectX::XMFLOAT4 o) { orientation = o; }
	void set_scale(DirectX::XMFLOAT3 s) { scale = s; }
	void set_scale(float s) { scale = { s,s,s }; }
	void set_velocity(DirectX::XMFLOAT3 v) { velosity = v; }
	void set_life_span(float l) { life_span = l; }
	void set_is_loop(bool loop) { is_loop = loop; }
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	void rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec);
	void register_shader_resource(ID3D11Device* device, const wchar_t* filename);
	void register_shader_resource(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void create_pixel_shader(ID3D11Device* device, const char* cso_name);
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	DirectX::XMFLOAT4 get_orientation() { return orientation; }
	DirectX::XMFLOAT3 get_scale() { return scale; }
	DirectX::XMFLOAT3 get_velosity() { return velosity; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }

	void reset_orientation();
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
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	std::unique_ptr<MeshShader> shader = nullptr;
	// �X�P���^�����b�V���̎���
	std::unique_ptr <SkeletalMesh> model;
	//�ʒu
	DirectX::XMFLOAT3 position = {0,0,0};
	//��]��
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//�X�P�[��
	DirectX::XMFLOAT3 scale = {1,1,1};
	//�g�����X�t�H�[��
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	//���x
	DirectX::XMFLOAT3 velosity = {0,0,0};
	
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
