#include "static_mesh.h"

using namespace DirectX;
StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates)
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t current_index{ 0 };

	std::vector<XMFLOAT3>positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<wstring> mtl_filenames;

	std::wifstream fin(obj_filename);
	_ASSERT_EXPR(fin, L"OBJ file not found.");

	//�s�̍X�V
	wchar_t command[256];
	while (fin)
	{
		//��ԏ��1�s���擾
		fin >> command;

		if (0 == wcscmp(command, L"v"))
		{
			//���_���W���擾
			float x, y, z;
			//��؂�P�ʂŃf�[�^���擾���Ă���
			fin >> x >> y >> z;
			positions.push_back({ x, y, z });
			//1�ԏ�ɂ���1�s���폜
			fin.ignore(1024, L'\n');   //ignor(n, delim)  ��1�����F�������̏�� ��2�����F���̕����������܂œ��͂��Ď̂Ă鏈�������s���� //https://bit.ly/2Wwma3M)
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			//�@�����擾
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back({ i, j, k });

			//1�ԏ�ɂ���1�s���폜
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))
		{
			float u, v;
			fin >> u >> v;
			texcoords.push_back({ u,flipping_v_coordinates ? 1.0f - v : v });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))
		{
			for (size_t i = 0; i < 3; i++)
			{
				vertex vertex;
				size_t v, vt, vn;

				// �P�ԏ�ɂ���P�s�ڂ̂P�ڂ̃f�[�^���擾��
				// �f�[�^�̂����̂P�ڂ̐��l���擾
				fin >> v;
				vertex.position = positions.at(v - 1);
				if (L'/' == fin.peek())
				{
					fin.ignore(1);
					if (L'/' != fin.peek())
					{
						//�X���b�V����؂�Ŏ��̐��l���擾
						fin >> vt;
						vertex.texcoord = texcoords.at(vt - 1);
					}
					if (L'/' == fin.peek())
					{
						fin.ignore(1);
						fin >> vn;
						vertex.normal = normals.at(vn - 1);
					}
				}
				//���_�f�[�^��ݒ�
				vertices.push_back(vertex);

				//���_�C���f�b�N�X��ݒ�
				indices.push_back(current_index++);
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"mtllib"))
		{
			//�}�e���A���t�@�C�������擾
			wchar_t mtllib[256];
			fin >> mtllib;
			mtl_filenames.push_back(mtllib);
		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			wchar_t usemtl[MAX_PATH]{ 0 };
			fin >> usemtl;
			subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
		}
		else
		{
			fin.ignore(1024, L'\n');
		}

	}
	fin.close();

	//��납�珇�ɂ��������邽�߂̑��삷�邽�߂̃C�e���[�^�[���擾
	std::vector<subset>::reverse_iterator iterator = subsets.rbegin();

	//�Ō�̃T�u�Z�b�g�̃C���f�b�N�X���@���@���b�V���̑��C���f�b�N�X���@�[�@�Ō�̃T�u�Z�b�g�̃C���f�b�N�X�J�n�ʒu
	iterator->index_count = static_cast<uint32_t>(indices.size()) - iterator->index_start;
	for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
	{
		iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
	}
	// MTL �t�@�C�����@OBJ�@�t�@�C������ݒ�
	std::filesystem::path mtl_filename(obj_filename);

	mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename()); //�p�X�Ɋ܂܂��t�@�C������u��������

	//�}�e���A���t�@�C�����J��
	fin.open(mtl_filename);
	_ASSERT_EXPR(fin, L"MTL file not found.");
	/*if (materials.size() == 0)
	{
		for (const subset& subset : subsets)
		{
			materials.push_back({ subset.usemtl });
		}
	}*/

	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"map_Kd"))
		{
			fin.ignore();

			//�e�N�X�`���t�@�C������ǂݍ���
			wchar_t map_Kd[256];
			fin >> map_Kd;

			//�e�N�X�`���t�@�C�����Ƀp�X�����t����
			std::filesystem::path path(obj_filename);
			path.replace_filename(std::filesystem::path(map_Kd).filename());
			//materials.rbegin()->texture_filename = path;
			materials.rbegin()->texture_filenames[0] = path; //UNIT16
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
		{
			fin.ignore();
			wchar_t map_bump[256];
			fin >> map_bump;
			std::filesystem::path path(obj_filename);
			path.replace_filename(std::filesystem::path(map_bump).filename());
			materials.rbegin()->texture_filenames[1] = path;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"newmtl"))
		{
			fin.ignore();
			wchar_t newmtl[256];
			material material;
			fin >> newmtl;
			material.name = newmtl;
			materials.push_back(material);
		}
		else if (0 == wcscmp(command, L"Kd"))
		{
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Kd = { r, g, b, 1 };
			fin.ignore(1024, '\n');

		}
		else if (0 == wcscmp(command, L"Ka"))
		{
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ka = { r, g, b, 1 };
			fin.ignore(1024, '\n');

		}
		else if (0 == wcscmp(command, L"Ks"))
		{
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ks = { r, g, b, 1 };
			fin.ignore(1024, '\n');

		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();

	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

	

	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	//���_�V�F�[�_�[�I�u�W�F�N�g�̍쐬
	/*create_vs_from_cso(device, "shaders/static_mesh_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));*/
	create_vs_from_cso(device, "shaders/ramp_shader_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̍쐬
	create_ps_from_cso(device, "shaders/ramp_shader_ps.cso", pixel_shader.GetAddressOf());

	//�萔�o�b�t�@
	object_constants = std::make_unique<Constants<OBJECT_CONSTANTS>>(device);


	//�ǂݍ��񂾃e�N�X�`���𐶐�����6
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//load_texture_from_file(device, texture_filename.c_str(),
	//	shader_resource_view.GetAddressOf(), &texture2d_desc);

	//materials.rbegin()->texture_filenames[1] = L".\\resources\\Rock\\Rock-Texture-Surface.jpg";  //�i�o�O�������j���ڃp�X�w�肷��Ɠ\��t���\

	//�}�e���A���e�N�X�`���ǂݍ���

	for (auto& material : materials)
	{
		if (material.texture_filenames[0].size() > 0)
		{
			load_texture_from_file(device, material.texture_filenames[0].c_str(), material.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
		}
		else
		{
			make_dummy_texture(device, material.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 16);
		}
		if (material.texture_filenames[1].size() > 0)
		{
			load_texture_from_file(device, material.texture_filenames[1].c_str(), material.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
		}
		else
		{
			make_dummy_texture(device, material.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 16);
		}
	}

	load_texture_from_file(device, L".\\resources\\ramp.png",
		ramp_texture.GetAddressOf(), &texture2d_desc);

	//�o�E���f�B���O�{�b�N�X�F���_��ݒ�
	for (const vertex& v : vertices)
	{
		bounding_box[0].x = std::min<float>(bounding_box[0].x, v.position.x);
		bounding_box[0].y = std::min<float>(bounding_box[0].y, v.position.y);
		bounding_box[0].z = std::min<float>(bounding_box[0].z, v.position.z);
		bounding_box[1].x = std::max<float>(bounding_box[1].x, v.position.x);
		bounding_box[1].y = std::max<float>(bounding_box[1].y, v.position.y);
		bounding_box[1].z = std::max<float>(bounding_box[1].z, v.position.z);
	}
}



void StaticMesh::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };
	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void StaticMesh::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
	ID3D11PixelShader* bound_pixel_shader)
{
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	bound_pixel_shader ? immediate_context->PSSetShader(bound_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	//�e�N�X�`���̏����V�F�[�_�[���ɐݒ�
	for (const material& material : materials)
	{
		immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
		immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

		//�����v�p�̃e�N�X�`��
		immediate_context->PSSetShaderResources(2, 1, ramp_texture.GetAddressOf());
		//�萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
		 object_constants->data = { world, material.Ka, material.Kd, material.Ks };
		XMStoreFloat4(&object_constants->data.Ka, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
		object_constants->bind(immediate_context, 0, CB_FLAG::PS_VS);
		for (const subset& subset : subsets)
		{
			if (material.name == subset.usemtl)
			{
				immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}

	
	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

