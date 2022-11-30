#pragma once
#pragma once
#include "mesh_shader.h"
#include "instancing_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
#include "material.h"
class InstanceMeshEffect
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
		DirectX::XMFLOAT2 scroll_direction = { 0,0 };
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
	InstanceMeshEffect(Graphics& graphics, const char* fbx_filename, const int max_instance);
	~InstanceMeshEffect() {};
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

	void set_position(DirectX::XMFLOAT3 p, int index) { model->set_position(p, index); }
	void set_orientation(DirectX::XMFLOAT4 o, int index) { model->set_orientation(o, index); }
	void set_scale(DirectX::XMFLOAT3 s, int index) { model->set_scale(s, index); }
	void set_scale(float s, int index) { model->set_scale(s,index); }

	void set_velocity(DirectX::XMFLOAT3 v) { velosity = v; }
	void set_life_span(float l) { life_span = l; }
	void set_is_loop(bool loop) { is_loop = loop; }
	DirectX::XMFLOAT4 set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang, int index);
	DirectX::XMFLOAT4 set_rotate_quaternion(AXIS axis, float ang, int index);
	DirectX::XMFLOAT4 rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec, int index);
	void set_material(Material* m) { material = m; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	DirectX::XMFLOAT4 get_orientation() { return orientation; }
	DirectX::XMFLOAT3 get_scale() { return scale; }

	DirectX::XMFLOAT3 get_position(int index) { return  model->get_position(index); }
	DirectX::XMFLOAT4 get_orientation(int index) { return  model->get_orientation(index); }
	DirectX::XMFLOAT3 get_scale(int index) { return  model->get_scale(index);; }

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
	std::unique_ptr<InstanceMesh> model = nullptr;
protected:
	Material* material;

	//�ʒu
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//��]��
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//�X�P�[��
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	//�g�����X�t�H�[��
	//���x
	DirectX::XMFLOAT3 velosity = { 0,0,0 };

	//��������
	float life_time = 0;
	//����
	float life_span = 0;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//���[�v�Đ����邩�ǂ���
	bool is_loop = false;
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;


	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

	const int MATERIAL_START_SLOT = 20;

};
