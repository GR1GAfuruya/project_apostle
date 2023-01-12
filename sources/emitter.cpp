#include "emitter.h"
#include "coordinate_convert.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"
#include "texture.h"
Emitter::Emitter(Graphics& graphics, int max_particles)
	: max_vertices(max_particles * 4)
{
	this->max_particles = 1;
	emit_span = 1;
	active = false;

	HRESULT hr{ S_OK };
	vertices = new vertex[4*this->max_particles];

	// 頂点バッファの生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertex) * 4 * this->max_particles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	// バッファ生成
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = vertices;
	hr = graphics.get_device().Get()->CreateBuffer(&bd, NULL, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	インデックス生成
	int* index = new int[6 * this->max_particles];
	for (int i = 0; i < this->max_particles; i++)
	{
		index[0 + i * 6] = 0 + i * 4;
		index[1 + i * 6] = 1 + i * 4;
		index[2 + i * 6] = 2 + i * 4;

		index[3 + i * 6] = 1 + i * 4;
		index[4 + i * 6] = 3 + i * 4;
		index[5 + i * 6] = 2 + i * 4;
	}

	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * 6 * this->max_particles;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	// バッファ生成
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = index;
	graphics.get_device().Get()->CreateBuffer(&bd, NULL, index_buffer.GetAddressOf());

	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	   D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
	};
	// 頂点シェーダーオブジェクトの生成
	const char* cso_name_vs{ "shaders/sprite_particle_vs.cso" };
	create_vs_from_cso(graphics.get_device().Get(), cso_name_vs, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));

	// ジオメトリシェーダーオブジェクトの生成
	const char* cso_name_gs{ "shaders/sprite_particle_gs.cso" };
	create_gs_from_cso(graphics.get_device().Get(), cso_name_gs, geometry_shader.GetAddressOf());
	// ピクセルシェーダーオブジェクトの生成
	const char* cso_name_ps{ "shaders/sprite_particle_ps.cso" };
	create_ps_from_cso(graphics.get_device().Get(), cso_name_ps, pixel_shader.GetAddressOf());
	// 画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/Particle01.png", shader_resource_view.GetAddressOf(), &texture2d_desc);
}

Emitter::~Emitter()
{
	particles.clear();
	removes.clear();
	delete[] vertices;
}

void Emitter::play(DirectX::XMFLOAT3 pos)
{
	active = true;

}


void Emitter::emit(Graphics& graphics, float elapsed_time)
{
	//エミット処理
	emit_timer += elapsed_time;
	
	if (emit_timer >= emit_span)
	{
		int p_size = static_cast<int>(particles.size());
		if (p_size < max_particles)
		{
			Particles::InitParam init_param;
			init_param.position = position;
			init_param.life_time = 0.5;
			init_param.scale = { 1,1 };
			init_param.velocity = emit_dir;
			for (int i = 0; i < burst_num; i++)
			{
				particles.emplace_back(make_unique<Particles>(graphics, init_param));
			}
			//タイマーリセット
			emit_timer = 0;
		}
	}
}


void Emitter::update(Graphics& graphics, float elapsed_time)
{
	//パーティクルの更新
	for (auto& p : particles)
	{
		p->update(graphics, elapsed_time);
	}
	//エミッターの寿命更新、パーティクルの寿命処理
	life_update(elapsed_time);
	//パーティクルの破棄処理
	remove_update();
	//アクティブ状態でないときは生成処理を行わない
	if (!active) return;
	//エミッターの位置更新
	position_update(elapsed_time);

	//エミット時間処理
	emit(graphics, elapsed_time);
}

void Emitter::render(Graphics& graphics, Camera& camera)
{
	graphics.get_dc()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	//graphics.get_dc()->GSSetShader(geometry_shader.Get(), nullptr, 0);
	graphics.get_dc()->PSSetShader(pixel_shader.Get(), nullptr, 0);
	graphics.get_dc()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);

	float komaU = 1.0f / 1;
	float komaV = 1.0f / 1;
	int komaNum = 1 * 1;
	DirectX::XMFLOAT4X4 m = Math::get_inv_mat(camera.get_view());
	// 右＆上軸取得
	auto right = -DirectX::XMFLOAT3(m._11, m._12, m._13);
	auto up = DirectX::XMFLOAT3(m._21, m._22, m._23);

	active_count = 0;
	for (int i = 0; i < particles.size(); i++)
	{
		const DirectX::XMFLOAT3 pos = particles.at(i)->get_position();
		const float size = 10;
		const DirectX::XMFLOAT4 color = particles.at(i)->get_color();
		//particles.at(i)->set_velocity({ 0,-1,0 });
		// 左上
		vertices[0].position.x = pos.x - right.x * size + up.x * size;
		vertices[0].position.y = pos.y - right.x * size + up.x * size;
		vertices[0].position.z = pos.z - right.x * size + up.x * size;
		vertices[0].texcoord.x = 0.0f;
		vertices[0].texcoord.y = 0.0f;
		vertices[0].color = color;
		// 右上
		vertices[1].position.x = pos.x + right.x * size + up.x * size;
		vertices[1].position.y = pos.y + right.y * size + up.y * size;
		vertices[1].position.z = pos.z + right.z * size + up.z * size;
		vertices[1].texcoord.x = 1.0f;
		vertices[1].texcoord.y = 0.0f;
		vertices[1].color = color;
		// 左下
		vertices[2].position.x = pos.x - right.x * size - up.x * size;
		vertices[2].position.y = pos.y - right.y * size - up.y * size;
		vertices[2].position.z = pos.z - right.z * size - up.z * size;
		vertices[2].texcoord.x = 0.0f;
		vertices[2].texcoord.y = 1.0f;
		vertices[2].color = color;
		// 右下
		vertices[3].position.x = pos.x + right.x * size - up.x * size;
		vertices[3].position.y = pos.y + right.y * size - up.y * size;
		vertices[3].position.z = pos.z + right.z * size - up.z * size;
		vertices[3].texcoord.x = 1.0f;
		vertices[3].texcoord.y = 1.0f;
		vertices[3].color = color;

	}

	//// 頂点バッファ更新
	graphics.get_dc()->UpdateSubresource(vertex_buffer.Get(), 0,
		nullptr, vertices, 0, 0);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	graphics.get_dc()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	graphics.get_dc()->IASetInputLayout(input_layout.Get());
	graphics.get_dc()->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	//graphics.get_dc()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	graphics.get_dc()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//graphics.get_dc()->Draw(particles.size(), 0);
	graphics.get_dc()->DrawIndexed(6, 0, 0);

	graphics.get_dc()->VSSetShader(nullptr, nullptr, 0);
	graphics.get_dc()->GSSetShader(nullptr, nullptr, 0);
	graphics.get_dc()->PSSetShader(nullptr, nullptr, 0);
}

void Emitter::debug_gui(Graphics& graphics, string id)
{
	imgui_menu_bar("effect", "emitter", display_imgui);
#if USE_IMGUI
	if (display_imgui)
	{
		ImGui::Begin(id.c_str());
		ImGui::DragFloat3("position", &position.x, 0.1f);
		ImGui::DragFloat3("emit_dir", &emit_dir.x, 0.1f);
		ImGui::DragFloat("duration", &duration, 0.1f);
		ImGui::DragFloat("life_timer", &life_timer, 0.1f);
		ImGui::DragFloat("emit_timer", &emit_timer, 0.1f);
		ImGui::DragFloat("emit_span", &emit_span, 0.1f);
		int p_num = particles.size();
		int r_num = removes.size();
	//	int v_num = vertices.size();
		ImGui::DragInt("particle_num", &p_num);
		ImGui::DragInt("removes_num", &r_num);
	//	ImGui::DragInt("vertex_num", &v_num);
		ImGui::Checkbox("active", &active);
		ImGui::End();
		
	}
#endif // USE_IMGUI

}

void Emitter::position_update(float elapsed_time)
{
	//位置更新
	position += velocity;
}

void Emitter::life_update(float elapsed_time)
{

	if (active)
	{
		life_timer += elapsed_time;
	}
	else
	{
		//生成開始時間を超えたらアクティブに
		timer += elapsed_time;
		if (timer > emit_start_time)
		{
			active = true;
		}
	}

	//ライフが切れたらアクティブ状態を着る
	if (life_timer > duration)
	{
		active = false;
	}
	//パーティクルがアクティブじゃなくなったらリムーブのコンテナに放り込む
	for (auto& p : particles)
	{
		if (!p->get_is_active())
		{
			removes.push_back(move(p));
		}
	}
	std::erase(particles, nullptr);
}

void Emitter::remove_update()
{
	//破壊処理
	removes.clear();
}