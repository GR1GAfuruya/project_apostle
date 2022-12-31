#include "emitter.h"
#include "coordinate_convert.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"
#include "texture.h"
Emitter::Emitter(Graphics& graphics, int max_particles)
	: max_vertices(max_particles * 4)
{
	this->max_particles = max_particles;
	emit_span = 1;
	active = true;

	HRESULT hr{ S_OK };
	vertices = new vertex[this->max_particles];

	// 頂点バッファの生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex) * this->max_particles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	// バッファ生成
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = vertices;
	hr = graphics.get_device().Get()->CreateBuffer(&bd, &initData, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

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
}


void Emitter::emit(Graphics& graphics, float elapsed_time)
{
	//エミッターがアクティブ状態でないなら処理しない
	if (!active) return;
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

	//エミッターの位置更新
	position_update(elapsed_time);
	//エミッターの寿命更新、パーティクルの寿命処理
	life_update(elapsed_time);
	//パーティクルの破棄処理
	remove_update();

	//エミット時間処理
	
	emit(graphics, elapsed_time);
}

void Emitter::render(Graphics& graphics, Camera& camera)
{
	graphics.get_dc()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	graphics.get_dc()->GSSetShader(geometry_shader.Get(), nullptr, 0);
	graphics.get_dc()->PSSetShader(pixel_shader.Get(), nullptr, 0);
	graphics.get_dc()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	//DirectX::XMFLOAT4X4 m = Math::get_inv_mat(camera.get_view());
	//// 右＆上軸取得
	//auto right = -DirectX::XMFLOAT3(m._11, m._12, m._13);
	//auto up = DirectX::XMFLOAT3(m._21, m._22, m._23);

	for (int i = 0; i < particles.size(); i++) 
	{
		const DirectX::XMFLOAT3 pos = particles.at(i)->get_position();
		const DirectX::XMFLOAT2 size = particles.at(i)->get_scale();
		const DirectX::XMFLOAT4 color = particles.at(i)->get_color();
		// 左上
		vertices[i].position = pos;
		vertices[i].texcoord.x = 0;
		vertices[i].texcoord.y = 0;
		vertices[i].color = color;
		
	}
	
	// 頂点バッファ更新
	graphics.get_dc()->UpdateSubresource(vertex_buffer, 0,
			NULL, vertices, 0, 0);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	graphics.get_dc()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	//graphics.get_dc()->IASetInputLayout(input_layout.Get());
	//graphics.get_dc()->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	//graphics.get_dc()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	graphics.get_dc()->Draw(static_cast<UINT>(particles.size()), 0);

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
	life_timer += elapsed_time;

	if (life_timer > duration)
	{
		active = false;
	}
	
	for (auto& p : particles)
	{
		if (!p->get_is_active())
		{
			//removes.push_back(move(p));
		}
	}
		std::erase(particles, nullptr);
}

void Emitter::remove_update()
{
	//破壊処理
	removes.clear();
}