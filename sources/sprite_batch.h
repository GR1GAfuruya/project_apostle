#pragma once


#include <d3d11.h>
// UNIT.10
#include <wrl.h>
#include <directxmath.h>

// UNIT.09
#include <memory>
#include <vector>

#include "constant.h"

class SpriteBatch
{
public:
	//�����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee420044(v=vs.85) ���s�v���O����
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419858(v=vs.85)	���s�v���O����
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419857(v=vs.85)	���̓A�Z���u���[ �X�e�[�W�̓��̓f�[�^�ɃA�N�Z�X
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;		//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419532(v=vs.85)�@�\��������Ă��Ȃ��������[�ł���o�b�t�@�[ ���\�[�X�ɃA�N�Z�X
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;		//https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nn-d3d11-id3d11shaderresourceview 
													   //�V�F�[�_�[-���\�[�X-�r���[�C���^�[�t�F�C�X�́A�V�F�[�_�[�������_�����O���ɃA�N�Z�X�ł���T�u���\�[�X���w��

	D3D11_TEXTURE2D_DESC texture2d_desc;

	//�萔�o�b�t�@
	struct SCROLL_CONSTANTS
	{
		DirectX::XMFLOAT2 scroll_direction;
		DirectX::XMFLOAT2 scroll_speed;
	};
	std::unique_ptr<Constants<SCROLL_CONSTANTS>> scroll_constants;

	DirectX::XMFLOAT2 scroll_direction;

	D3D11_TEXTURE2D_DESC get_texture2d_desc()const { return texture2d_desc; }
	//�����o�֐�
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
	            DirectX::XMFLOAT4 color,
	            float angle);

	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale
		);

	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
		DirectX::XMFLOAT4 color,float angle,
		DirectX::XMFLOAT2 tex_pos, DirectX::XMFLOAT2 tex_size);

	void begin(ID3D11DeviceContext* immediate_context,
		ID3D11PixelShader* replaced_pixel_shader = nullptr/*UNIT.10*/, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr/*UNIT.10*/);

	void end(ID3D11DeviceContext* immediate_context);

	SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites);
	~SpriteBatch();

	//���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
private:
	//�����o�ϐ�
	const size_t maxVertices;
	std::vector<vertex> vertices;
};
