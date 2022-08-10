#include "SpriteDissolve.h"
#include "misc.h"

#include <sstream>

// UNIT.04
#include <functional>

// UNIT.05
//#include <WICTextureLoader.h>

// UNIT.10
#include "texture.h"
#include "shader.h"

SpriteDissolve::SpriteDissolve(ID3D11Device* device, const wchar_t* filename, size_t maxSprites, const wchar_t* mask_texture_filename)
	:  maxVertices(maxSprites * 6)
{
	HRESULT hr{ S_OK };

	//���_���̃Z�b�g
	//vertex vertices[]
	//{
	//	{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0, 0} },//���_����
	//	{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1, 0} },//���_�E��
	//	{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0, 1} },//���_����
	//	{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1, 1} },//���_�E��
	//};
	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(maxVertices) };

	//���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = sizeof(vertex) * maxVertices; //�o�b�t�@�[�̃T�C�Y
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@�����ʁB�@�X�V�p�x�͏d�v�ȗv�f
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩������		�t���O�͘_���a�őg�ݍ��킹����
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU�A�N�Z�X�̃t���O	�K�v�Ȃ��ꍇ�͂O
	bufferDesc.MiscFlags = 0;
	//���̑��̃t���O�ihttps://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416267(v=vs.85)�@���Q�Ɓj
	bufferDesc.StructureByteStride = 0; //�\���̂��\�����o�b�t�@�[��\���ꍇ�A���̍\���̂̃T�C�Y (�o�C�g�P��) �ł��B
	D3D11_SUBRESOURCE_DATA subresourceData{};
	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416284(v=vs.85)
	subresourceData.pSysMem = vertices.get(); //�������f�[�^�ւ̃|�C���^�[
	subresourceData.SysMemPitch = 0; //�e�N�X�`���[�ɂ���1�{�̐��̐�[����ׂ̐��܂ł̋���
	subresourceData.SysMemSlicePitch = 0;; //1�̐[�x���x���̐�[����ׂ̐[�x���x���܂ł̋���

	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�萔�o�b�t�@
	scroll_constants = std::make_unique<Constants<SCROLL_CONSTANTS>>(device);

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	// UNIT.10
	create_vs_from_cso(device, "shaders/sprite_dissolve_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
		input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "shaders/sprite_dissolve_ps.cso", pixel_shader.GetAddressOf());


	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = load_texture_from_file(device, mask_texture_filename, mask_shader_resource_view.GetAddressOf(), &mask_texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


inline auto rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;


	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};


void SpriteDissolve::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//��`�̍���̍��W(�X�N���[�����W�n)
	float dw, float dh,		//��`�̃T�C�Y(�X�N���[�����W�n)
	float r, float g, float b, float a,
	float angle/*degree*/
)
{
	// UNIT.06
	render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteDissolve::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
	render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteDissolve::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//��`�̍���̍��W(�X�N���[�����W�n)
	float dw, float dh,		//��`�̃T�C�Y(�X�N���[�����W�n)
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy, float sw, float sh
)
{
	//�X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{}; //�r���[�|�[�g�̐��@�̒�`�@https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
	UINT numViewports{ 1 };
	immediate_context->RSGetViewports(&numViewports, &viewport); //���X�^���C�U�X�e�[�W�Ƀo�C���h���ꂽ�r���[�|�[�g�̔z����擾

	//render�����o�֐��̈��������`�̊e���_�̈ʒu���v�Z����
	// (x0, y0) *----* (x1, y1)
	//			|   /|
	//			|  / |
	//			| /	 |
	//			|/	 |
	// (x2, y2),*----* (x3, y3)


	// left-top
	float x0{ dx };
	float y0{ dy };
	//right-top
	float x1{ dx + dw };
	float y1{ dy };
	//left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	//right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };





	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	rotate(x0, y0, cx, cy, cos, sin);
	rotate(x1, y1, cx, cy, cos, sin);
	rotate(x2, y2, cx, cy, cos, sin);
	rotate(x3, y3, cx, cy, cos, sin);


	////�X�N���[�����W�n����NDC�ւ̍��W�Ԋ҂��s��
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	//�e�N�Z�����W�n����e�N�X�`�����W�n�ւ̕ϊ�
	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };

	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };

	vertices.push_back({ {x0, y0, 0}, {r, g, b, a}, {u0, v0} });
	vertices.push_back({ {x1, y1, 0}, {r, g, b, a}, {u1, v0} });
	vertices.push_back({ {x2, y2, 0}, {r, g, b, a}, {u0, v1} });
	vertices.push_back({ {x2, y2, 0}, {r, g, b, a}, {u0, v1} });
	vertices.push_back({ {x1, y1, 0}, {r, g, b, a}, {u1, v0} });
	vertices.push_back({ {x3, y3, 0}, {r, g, b, a}, {u1, v1} });


}

void SpriteDissolve::begin(ID3D11DeviceContext* immediate_context,
	ID3D11PixelShader* replaced_pixel_shader/*UNIT.10*/, ID3D11ShaderResourceView* replaced_shader_resource_view/*UNIT.10*/)
{
	vertices.clear();
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// UNIT.10
	//immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	if (replaced_shader_resource_view)
	{
		HRESULT hr{ S_OK };
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		replaced_shader_resource_view->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		texture2d->GetDesc(&texture2d_desc);

		immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
		
	}
	else
	{
		immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	
		immediate_context->PSSetShaderResources(1, 1, mask_shader_resource_view.GetAddressOf());
	}

	//�萔�o�b�t�@�̍X�V
	scroll_constants->bind(immediate_context, 2, ::CB_FLAG::PS_VS);
}

void SpriteDissolve::end(ID3D11DeviceContext* immediate_context)
{
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);


	//���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//TRIANGLELIST�ɂ�����3�p�`�P�ɂȂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());
	//�V�F�[�_�[�̃o�C���h

	//�v���~�e�B�u�̕`��
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);

	// UNIT.10
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shader_resource_view.Get()->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->GetDesc(&texture2d_desc);
}
