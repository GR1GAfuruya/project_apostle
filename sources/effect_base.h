#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
class EffecttBase
{
public:
	EffecttBase() {}
	virtual ~EffecttBase() {}
	//�Đ�
	virtual void play(DirectX::XMFLOAT3 pos) = 0;
	//��~
	virtual void stop();
	//�X�V
	virtual void update(Graphics& graphics, float elapsed_time) = 0;
	//�`��
	virtual void render(Graphics& graphics) = 0;

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
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	DirectX::XMFLOAT4 get_orientation() { return orientation; }
	DirectX::XMFLOAT3 get_scale() { return scale; }
	DirectX::XMFLOAT3 get_velosity() { return velosity; }
	bool get_active() { return active; }
protected:
	
	std::unique_ptr<MeshShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

	//�ʒu
	DirectX::XMFLOAT3 position = {0,0,0};
	//��]��
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//�X�P�[��
	DirectX::XMFLOAT3 scale = {1,1,1};
	//���x
	DirectX::XMFLOAT3 velosity = {0,0,0};
	//��������
	float life_time = 0;
	//�A�N�e�B�u��Ԃ�
	bool active = false;
	//�f�o�b�OGUI�t���O
	bool display_imgui = true;
};
