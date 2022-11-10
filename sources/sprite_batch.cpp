#include "sprite_batch.h" // UNIT.09
#include "misc.h"

#include <sstream>

#include <functional>


#include "texture.h"
#include "shader.h"


#if _DEBUG
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
#else
CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
#endif
//�𑜓x�ɂ�鍷���̔{��
CONST FLOAT MAGNI_RESOLUTION_WIDTH{ SCREEN_WIDTH / 1280.0f };
//�𑜓x�ɂ�鍷���̔{��
CONST FLOAT MAGNI_RESOLUTION_HEIGHT{ SCREEN_HEIGHT / 720.0f };

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites)
	:  maxVertices(maxSprites * 6)
{
	HRESULT hr{S_OK};

	//���_���̃Z�b�g
	//vertex vertices[]
	//{
	//	{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0, 0} },//���_����
	//	{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1, 0} },//���_�E��
	//	{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0, 1} },//���_����
	//	{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1, 1} },//���_�E��
	//};
	const std::unique_ptr<vertex[]> vertices = std::make_unique<vertex[]>(maxVertices);

	//���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * maxVertices);//�o�b�t�@�[�̃T�C�Y
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
	create_vs_from_cso(device, "shaders/sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
	                   input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "shaders/sprite_ps.cso", pixel_shader.GetAddressOf());


	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
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


void SpriteBatch::render(ID3D11DeviceContext* dc,
                         DirectX::XMFLOAT2 position,
                         //��`�̍���̍��W(�X�N���[�����W�n)
                         DirectX::XMFLOAT2 scale,
                         //��`�̃T�C�Y(�X�N���[�����W�n)
                         DirectX::XMFLOAT4 color,
                         float angle/*degree*/
)
{
	// UNIT.06
	render(dc, position, scale, color, angle, { 0.0f, 0.0f }, { static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height) });
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale)
{
	render(dc, { position },{ scale },{ 1.0f, 1.0f, 1.0f, 1.0f}, 0.0f,
		{ 0.0f, 0.0f }, { static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height) });
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
	DirectX::XMFLOAT4 color,float angle,
	DirectX::XMFLOAT2 tex_pos, DirectX::XMFLOAT2 tex_size
)
{
	//�X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{}; //�r���[�|�[�g�̐��@�̒�`�@https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport); //���X�^���C�U�X�e�[�W�Ƀo�C���h���ꂽ�r���[�|�[�g�̔z����擾

	//�𑜓x�̈Ⴂ�ɂ�鍷����␳
	position = { position.x * MAGNI_RESOLUTION_WIDTH, position.y * MAGNI_RESOLUTION_HEIGHT };
	scale = { scale.x * MAGNI_RESOLUTION_WIDTH, scale.y * MAGNI_RESOLUTION_HEIGHT };
	
	//render�����o�֐��̈��������`�̊e���_�̈ʒu���v�Z����
	// (x0, y0) *----* (x1, y1)
	//			|   /|
	//			|  / |
	//			| /	 |
	//			|/	 |
	// (x2, y2),*----* (x3, y3)


	// left-top
	float x0{ position.x - scale.x };
	float y0{ position.y - scale.y };
	// right-top
	float x1{ position.x + (fabsf(tex_size.x) * scale.x) };
	float y1{ position.y - scale.y };
	// left-bottom
	float x2{ position.x - scale.x };
	float y2{ position.y + (fabsf(tex_size.y) * scale.y) };
	// right-bottom
	float x3{ position.x + (fabsf(tex_size.x)  * scale.x) };
	float y3{ position.y + (fabsf(tex_size.y) * scale.y) };

	



	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = position.x + static_cast<float>(texture2d_desc.Width) * scale.x * 0.5f;
	float cy = position.y + static_cast<float>(texture2d_desc.Height) * scale.y * 0.5f;
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
	float u0{ tex_pos.x / texture2d_desc.Width };
	float v0{ tex_pos.y / texture2d_desc.Height };

	float u1{ (tex_pos.x + tex_size.x) / texture2d_desc.Width };
	float v1{ (tex_pos.y + tex_size.y) / texture2d_desc.Height };

	vertices.push_back({ {x0, y0, 0}, color, {u0, v0} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x3, y3, 0}, color, {u1, v1} });
	
	
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 texpos, DirectX::XMFLOAT2 texsize)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	dc->RSGetViewports(&num_viewports, &viewport);

	position = { position.x * MAGNI_RESOLUTION_WIDTH, position.y * MAGNI_RESOLUTION_HEIGHT };
	scale = { scale.x * MAGNI_RESOLUTION_WIDTH, scale.y * MAGNI_RESOLUTION_HEIGHT };

	// render�����o�֐��̈����idx, dy, dw, dh�j�����`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
	//  (x0, y0) *----* (x1, y1)
	//           |   /|
	//           |  / |
	//           | /  |
	//           |/   |
	//  (x2, y2) *----* (x3, y3)

	// left-top
	float x0{ position.x - (pivot.x * scale.x) };
	float y0{ position.y - (pivot.y * scale.y) };
	// right-top
	float x1{ position.x + ((fabsf(texsize.x) - pivot.x) * scale.x) };
	float y1{ position.y - (pivot.y * scale.y) };
	// left-bottom
	float x2{ position.x - (pivot.x * scale.x) };
	float y2{ position.y + ((fabsf(texsize.y) - pivot.y) * scale.y) };
	// right-bottom
	float x3{ position.x + ((fabsf(texsize.x) - pivot.x) * scale.x) };
	float y3{ position.y + ((fabsf(texsize.y) - pivot.y) * scale.y) };

	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = position.x;
	float cy = position.y;
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	rotate(x0, y0, cx, cy, cos, sin);
	rotate(x1, y1, cx, cy, cos, sin);
	rotate(x2, y2, cx, cy, cos, sin);
	rotate(x3, y3, cx, cy, cos, sin);

	// Convert to NDC space
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	float u0{ texpos.x / texture2d_desc.Width };
	float v0{ texpos.y / texture2d_desc.Height };
	float u1{ (texpos.x + texsize.x) / texture2d_desc.Width };
	float v1{ (texpos.y + texsize.y) / texture2d_desc.Height };

	if (scale.x >= 0)
	{
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
	}
	else
	{
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
	}

}

void SpriteBatch::begin(ID3D11DeviceContext* dc,
	ID3D11PixelShader* replaced_pixel_shader, ID3D11ShaderResourceView* replaced_shader_resource_view)
{
	vertices.clear();
	dc->VSSetShader(vertex_shader.Get(), nullptr, 0);
	replaced_pixel_shader ? dc->PSSetShader(replaced_pixel_shader, nullptr, 0) : dc->PSSetShader(pixel_shader.Get(), nullptr, 0);
	
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

		dc->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
	}
	else
	{
		dc->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//�f�o�C�X�̃[���x�[�X�̔z��ɃC���f�b�N�X��t���āA�V�F�[�_�[���\�[�X�̐ݒ���J�n ��2����:�ݒ肷��V�F�[�_�[���\�[�X�̐��@��3�����F�f�o�C�X�ɐݒ肷��V�F�[�_�[���\�[�X�r���[�C���^�[�t�F�C�X�̔z��
	}
	//�萔�o�b�t�@
	scroll_constants->bind(dc, 2, ::CB_FLAG::PS_VS);

}

void SpriteBatch::end(ID3D11DeviceContext* dc)
{
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = dc->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData)  };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(vertex) , p, vertex_count * sizeof(vertex));
	}

	dc->Unmap(vertex_buffer.Get(), 0);

	
	//���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	
	//�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//TRIANGLELIST�ɂ�����3�p�`�P�ɂȂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	dc->IASetInputLayout(input_layout.Get());
	//�V�F�[�_�[�̃o�C���h
	
	//�v���~�e�B�u�̕`��
	dc->Draw(static_cast<UINT>(vertex_count), 0);

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shader_resource_view.Get()->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->GetDesc(&texture2d_desc);
}

SpriteBatch::~SpriteBatch()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	shader_resource_view->Release();*/

}