#include "sprite.h"
#include "texture.h"
#include <sstream>


Sprite::Sprite(ID3D11Device* device, const wchar_t* filename): scroll_data()
{
	HRESULT hr{S_OK};

	//���_���̃Z�b�g
	vertex vertices[]
	{
		{{-1.0, +1.0, 0}, {1, 1, 1, 1}, {0, 0}}, //���_����
		{{+1.0, +1.0, 0}, {1, 1, 1, 1}, {1, 0}}, //���_�E��
		{{-1.0, -1.0, 0}, {1, 1, 1, 1}, {0, 1}}, //���_����
		{{+1.0, -1.0, 0}, {1, 1, 1, 1}, {1, 1}}, //���_�E��
	};

	//���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = sizeof(vertices); //�o�b�t�@�[�̃T�C�Y
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@�����ʁB�@�X�V�p�x�͏d�v�ȗv�f
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩������		�t���O�͘_���a�őg�ݍ��킹����
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU�A�N�Z�X�̃t���O	�K�v�Ȃ��ꍇ�͂O
	bufferDesc.MiscFlags = 0;
	//���̑��̃t���O�ihttps://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416267(v=vs.85)�@���Q�Ɓj
	bufferDesc.StructureByteStride = 0; //�\���̂��\�����o�b�t�@�[��\���ꍇ�A���̍\���̂̃T�C�Y (�o�C�g�P��) �ł��B
	D3D11_SUBRESOURCE_DATA subresourceData{};
	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416284(v=vs.85)
	subresourceData.pSysMem = vertices; //�������f�[�^�ւ̃|�C���^�[
	subresourceData.SysMemPitch = 0; //�e�N�X�`���[�ɂ���1�{�̐��̐�[����ׂ̐��܂ł̋���
	subresourceData.SysMemSlicePitch = 0; //1�̐[�x���x���̐�[����ׂ̐[�x���x���܂ł̋���

	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* cso_name{"shaders/sprite_vs.cso"};

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ftell(fp)};
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{std::make_unique<unsigned char[]>(csoSz)};
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertex_shader.GetAddressOf());
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createvertexshader
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�萔�o�b�t�@
	scroll_constants = std::make_unique<Constants<SCROLL_CONSTANTS>>(device);

	//���̓��C�A�E�g�I�u�W�F�N�g�̐���
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
	hr = device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc),
	                               csoData.get(), csoSz, input_layout.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* cso_pname{"shaders/sprite_ps.cso"};

	FILE* fpx{};
	fopen_s(&fpx, cso_pname, "rb");
	_ASSERT_EXPR_A(fpx, "CSO File not found");

	fseek(fpx, 0, SEEK_END);
	long csoSzp{ftell(fpx)};
	fseek(fpx, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoPData{std::make_unique<unsigned char[]>(csoSzp)};
	fread(csoPData.get(), csoSzp, 1, fpx);
	fclose(fpx);


	hr = device->CreatePixelShader(csoPData.get(), csoSzp, nullptr, pixel_shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g(ID3D11ShaderResourceView)�̐���

	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2dDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

Sprite::Sprite(ID3D11Device* device)
{
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




void Sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//��`�̍���̍��W(�X�N���[�����W�n)
	float dw, float dh,		//��`�̃T�C�Y(�X�N���[�����W�n)
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy,float sw, float sh
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

	////�؂蔲��
	
	float sx0{ sx };
	float sy0{sy };
	//right-top
	float sx1{ sx + sw };
	float sy1{ sy };
	//left-bottom
	float sx2{ sx };
	float sy2{ sy + sh };
	//right-bottom
	float sx3{ sx + sw };
	float sy3{ sy + sh };



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
	sx0 = sx0 / texture2dDesc.Width;
	sy0 = sy0 / texture2dDesc.Height;

	sx1 =  sx1  / texture2dDesc.Width;
	sy1 =  sy1  / texture2dDesc.Height;

	sx2 =  sx2 / texture2dDesc.Width;
	sy2 =  sy2 / texture2dDesc.Height;

	sx3 =  sx3/ texture2dDesc.Width;
	sy3 = sy3  / texture2dDesc.Height;


	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		vertices[0].texcoord = { sx0, sy0 };		//�摜����̍��W
		vertices[1].texcoord = { sx1, sy1 };
		vertices[2].texcoord = { sx2, sy2 };
		vertices[3].texcoord = { sx3, sy3 };

	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//�f�o�C�X�̃[���x�[�X�̔z��ɃC���f�b�N�X��t���āA�V�F�[�_�[���\�[�X�̐ݒ���J�n ��2����:�ݒ肷��V�F�[�_�[���\�[�X�̐��@��3�����F�f�o�C�X�ɐݒ肷��V�F�[�_�[���\�[�X�r���[�C���^�[�t�F�C�X�̔z��
	//���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//TRIANGLELIST�ɂ�����3�p�`�P�ɂȂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());
	//�V�F�[�_�[�̃o�C���h
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-VSSetShader
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	//�v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);
}

void Sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//��`�̍���̍��W(�X�N���[�����W�n)
	float dw, float dh		//��`�̃T�C�Y(�X�N���[�����W�n)
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



	

	////�X�N���[�����W�n����NDC�ւ̍��W�Ԋ҂��s��
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		vertices[0].texcoord = { 0, 0 };		//�摜����̍��W
		vertices[1].texcoord = { 1, 0 };
		vertices[2].texcoord = { 0, 1 };
		vertices[3].texcoord = { 1, 1 };

	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//�f�o�C�X�̃[���x�[�X�̔z��ɃC���f�b�N�X��t���āA�V�F�[�_�[���\�[�X�̐ݒ���J�n ��2����:�ݒ肷��V�F�[�_�[���\�[�X�̐��@��3�����F�f�o�C�X�ɐݒ肷��V�F�[�_�[���\�[�X�r���[�C���^�[�t�F�C�X�̔z��
	//���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//TRIANGLELIST�ɂ�����3�p�`�P�ɂȂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());
	//�V�F�[�_�[�̃o�C���h
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-VSSetShader
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	//�v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);
}

void Sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float carriage = 0;
	for (const char c : s)
	{
		render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0,
			sw * (c & 0x0F), sh * (c >> 4), sw, sh);
		carriage += w;
	}
}

Sprite::~Sprite()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	shader_resource_view->Release();*/
	
}