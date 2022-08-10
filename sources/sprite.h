#pragma once
#include<d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>
#include<string>

#include "constant.h"

class Sprite
{
public:
	//�����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee420044(v=vs.85) ���s�v���O����
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419858(v=vs.85)	���s�v���O����
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419857(v=vs.85)	���̓A�Z���u���[ �X�e�[�W�̓��̓f�[�^�ɃA�N�Z�X
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;		//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419532(v=vs.85)�@�\��������Ă��Ȃ��������[�ł���o�b�t�@�[ ���\�[�X�ɃA�N�Z�X
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;		//https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nn-d3d11-id3d11shaderresourceview 
													   //�V�F�[�_�[-���\�[�X-�r���[�C���^�[�t�F�C�X�́A�V�F�[�_�[�������_�����O���ɃA�N�Z�X�ł���T�u���\�[�X���w��
	//�萔�o�b�t�@
	struct SCROLL_CONSTANTS
	{
		DirectX::XMFLOAT2 scroll_direction;
		DirectX::XMFLOAT2 scroll_speed;
	}scroll_data;
	std::unique_ptr<Constants<SCROLL_CONSTANTS>> scroll_constants;
	DirectX::XMFLOAT2 scroll_direction;

	D3D11_TEXTURE2D_DESC texture2dDesc;
	//�����o�֐�
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle, float sx, float sy, float sw, float sh);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

	void textout(ID3D11DeviceContext* immediate_context, std::string s,
		float x, float y, float w, float h, float r, float g, float b, float a);

	Sprite(ID3D11Device* device, const wchar_t* filename);
	Sprite(ID3D11Device* device);
	~Sprite();

	//���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
private:
	
};
