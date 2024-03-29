#include "sprite_emitter.h"
#include "coordinate_convert.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"
#include "texture.h"
#include "graphics.h"
#include "dialog.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
SpriteEmitter::SpriteEmitter(Param init_param, int max_particles)
	: max_vertices(max_particles * 4)
{
	this->max_particles = max_particles;
	param = init_param;
	param.emit_span = 1;
	active = false;
	create_com_object(max_particles);
	life_timer = 0;
	position = { 0,0,0 };
}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
SpriteEmitter::~SpriteEmitter()
{

}
//==============================================================
// 
//COMオブジェクト生成
// 
//==============================================================
void SpriteEmitter::create_com_object(int max_particles)
{
	Graphics& graphics = Graphics::instance();
	HRESULT hr{ S_OK };
	vertices.resize(max_vertices);
	const std::unique_ptr<vertex[]> vertices = std::make_unique<vertex[]>(max_vertices);
	// 頂点バッファの生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex) * max_vertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// バッファ生成
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = vertices.get();
	hr = graphics.get_device().Get()->CreateBuffer(&bd, &initData, vertex_buffer.GetAddressOf());
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
	graphics.get_device().Get()->CreateBuffer(&bd, &initData, index_buffer.GetAddressOf());
	delete[] index;

	CPU_instance_data.reset(new Instance[max_particles]);


	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(Instance) * max_particles, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		bufferDesc.StructureByteStride = sizeof(Instance);

		graphics.get_device().Get()->CreateBuffer(&bufferDesc, nullptr, instance_data.ReleaseAndGetAddressOf());
	}


	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	   D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
	   { "I_ROTATION", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
		0,D3D11_INPUT_PER_INSTANCE_DATA,    1 },  // Instance rotation quaternion
		{ "I_POSITION", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,    1 },
		{ "I_SCALE", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,    1 },
		{ "I_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1,
	   D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,  1 },
		  { "I_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },


	};
	// 頂点シェーダーオブジェクトの生成
	const char* cso_name_vs{ "shaders/sprite_particle_vs.cso" };
	create_vs_from_cso(graphics.get_device().Get(), cso_name_vs, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
	// ピクセルシェーダーオブジェクトの生成
	const char* cso_name_ps{ "shaders/sprite_particle_ps.cso" };
	create_ps_from_cso(graphics.get_device().Get(), cso_name_ps, pixel_shader.GetAddressOf());
	// 画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成
	//wchar_t filename[256] = { 0 };
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/Particle02.png", shader_resource_view.GetAddressOf(), &texture2d_desc);
	//load_texture_from_file(graphics.get_device().Get(), filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	constants = std::make_unique<Constants<OBJECT_CONSTANTS>>(graphics.get_device().Get());

}
//==============================================================
// 
//再生
// 
//==============================================================
void SpriteEmitter::play(DirectX::XMFLOAT3 pos)
{
	//すでに再生中なら処理しない
	if (active) return;
	active = true;
	//position = pos;
}

//==============================================================
// 
//パーティクルの発生
// 
//==============================================================
void SpriteEmitter::emit(float elapsed_time)
{
	//エミット処理
	emit_timer += elapsed_time;

	if (emit_timer >= param.emit_span)
	{
		int p_size = static_cast<int>(particles.size());
		if (p_size < max_particles)
		{
			particle_start_functions();
			//エミッターの設定に基づいてparticleを作成
			for (int i = 0; i < param.burst_num; i++)
			{
				particles.emplace_back(make_unique<Particles>(param.particle_init_param));
			}
			//タイマーリセット
			emit_timer = 0;
		}
	}
}
void SpriteEmitter::start()
{
	//position = get_game_object()->get_component<Transform>()->get_position();

}
//==============================================================
// 
//パーティクルとエミッターの更新
// 
//==============================================================
void SpriteEmitter::update(float elapsed_time)
{
	//パーティクルの更新
	for (auto& p : particles)
	{
		//各種設定によるパーティクルの更新
		particle_update_functions(elapsed_time, *p);
		//particleの更新
		p->update(elapsed_time);
	}
	//エミッターの更新
	{
		//エミッターの寿命更新、パーティクルの寿命処理
		life_update(elapsed_time);
		//パーティクルの破棄処理
		remove_update();
		//アクティブ状態でないときは生成処理を行わない
		if (!active) return;
		//エミッターの位置更新
		position_update(elapsed_time);
		//エミッターの姿勢更新
		orientation_update(elapsed_time);

		//エミット時間処理
		emit(elapsed_time);

		//画像サイズ設定
		texsize.x = texture2d_desc.Width / static_cast<float>(param.TileX);
		texsize.y = texture2d_desc.Height / static_cast<float>(param.TileY);
	}
}

static DirectX::XMFLOAT3 ang = { 0,0,0 };
//==============================================================
// 
//パーティクルの描画
// 
//==============================================================
void SpriteEmitter::render(Camera* camera)
{
	ReplaceBufferContents(instance_data.Get(), sizeof(Instance) * this->max_particles, CPU_instance_data.get());
	Graphics& graphics = Graphics::instance();
	//パーティクル
	{
		graphics.get_dc()->VSSetShader(vertex_shader.Get(), nullptr, 0);
		graphics.get_dc()->PSSetShader(pixel_shader.Get(), nullptr, 0);
		graphics.get_dc()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);

		active_count = 0;
		for (int i = 0; i < particles.size(); i++)
		{

			particles.at(i).get()->look_at_camera(*camera);
			particles.at(i).get()->set_angle(ang);
			CPU_instance_data[i].position = particles.at(i).get()->get_position();
			CPU_instance_data[i].quaternion = particles.at(i).get()->get_orientation();
			CPU_instance_data[i].scale = particles.at(i).get()->get_scale();
			CPU_instance_data[i].color = particles.at(i).get()->get_color();
			//スプライトシーとアニメーション
			int tile_num = static_cast<int>(param.TileX * param.TileY * particles.at(i).get()->get_life_rate() * animation_rate);
			int total_tile_num = param.TileX * param.TileY - 1;
			if (tile_num > total_tile_num) tile_num = total_tile_num;
			texpos.x = static_cast<int>(tile_num % param.TileX) * texsize.x;
			texpos.y = static_cast<int>(tile_num / param.TileX) * texsize.y;

			float u0{ texpos.x / texture2d_desc.Width };
			float v0{ texpos.y / texture2d_desc.Height };
			CPU_instance_data[i].tip_texcoord = { u0, v0 };
			const DirectX::XMFLOAT4 color = particles.at(i)->get_color();

			float u1 = 1.0f / param.TileX;
			float v1 = 1.0f / param.TileY;

			// 左上
			vertices.at(0 + active_count * 4).position = { -1.0f, 1.0f, 1.0f };
			vertices.at(0 + active_count * 4).texcoord.x = 0;
			vertices.at(0 + active_count * 4).texcoord.y = 0;

			// 右上
			vertices.at(1 + active_count * 4).position = { 1.0f, 1.0f, 1.0f };
			vertices.at(1 + active_count * 4).texcoord.x = u1;
			vertices.at(1 + active_count * 4).texcoord.y = 0;

			// 左下
			vertices.at(2 + active_count * 4).position = { -1.0f, -1.0f, 1.0f };
			vertices.at(2 + active_count * 4).texcoord.x = 0;
			vertices.at(2 + active_count * 4).texcoord.y = v1;

			// 右下
			vertices.at(3 + active_count * 4).position = { 1.0f, -1.0f, 1.0f };
			vertices.at(3 + active_count * 4).texcoord.x = u1;
			vertices.at(3 + active_count * 4).texcoord.y = v1;

			active_count++;
		}
		DirectX::XMFLOAT4X4 parent_tra = get_game_object().get()->transform.get_transform();
		transform = Math::calc_world_matrix(parent_tra, scale, orientation, position);

		//transform = Math::calc_world_matrix(scale, orientation, position);
		/*DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
		};*/
		XMStoreFloat4x4(&constants->data.world, DirectX::XMLoadFloat4x4(&transform));
		constants->bind(graphics.get_dc().Get(), 0, CB_FLAG::VS);

		//---------＜描画に掛かる各種処理＞----------//
		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
		hr = graphics.get_dc()->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		size_t vertex_count = vertices.size();
		vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
		if (data != nullptr)
		{
			const vertex* p = vertices.data();
			memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
		}

		graphics.get_dc()->Unmap(vertex_buffer.Get(), 0);

		UINT Strides[] = { sizeof(vertex), sizeof(Instance) };
		UINT Offsets[] = { 0, 0 };

		ID3D11Buffer* Buffers[] = { vertex_buffer.Get(), instance_data.Get() };
		graphics.get_dc().Get()->IASetVertexBuffers(0, _countof(Strides), Buffers, Strides, Offsets);
		graphics.get_dc()->IASetInputLayout(input_layout.Get());
		graphics.get_dc()->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		//graphics.get_dc()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		graphics.get_dc()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//描画命令
		graphics.get_dc().Get()->DrawIndexedInstanced(6, active_count, 0, 0, 0);

		graphics.get_dc()->VSSetShader(nullptr, nullptr, 0);
		graphics.get_dc()->GSSetShader(nullptr, nullptr, 0);
		graphics.get_dc()->PSSetShader(nullptr, nullptr, 0);

	}
}

//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void SpriteEmitter::on_gui()
{
	imgui_menu_bar("effect", "emitter", display_imgui);
#if USE_IMGUI
	if (display_imgui)
	{
		ImGui::PushID(ImGui::GetID("emitter"));
		ImGui::Begin("WindowName##unique_id");
		ImGui::Text(get_game_object()->get_name().c_str());
		ImGui::Text(get_name());
		if (ImGui::Button("ChangeTexture")) { change_main_texture(); };
		ImGui::DragFloat3("position", &position.x, 0.1f);
		ImGui::DragFloat3("emit_dir", &emit_dir.x, 0.01f);
		ImGui::DragFloat("duration", &param.duration, 0.1f);
		ImGui::DragFloat("life_timer", &life_timer, 0.1f);
		ImGui::DragFloat("particle_life_timer", &param.particle_init_param.life_time, 0.1f);
		ImGui::DragFloat3("acceleration", &param.particle_init_param.transition.acceleration.x, 0.01f);
		ImGui::DragFloat4("color", &param.particle_init_param.color.x, 0.1f);
		ImGui::DragFloat2("scale", &param.particle_init_param.scaling.scale.x, 0.1f);
		ImGui::DragFloat2("e_scale", &scale.x, 0.1f);
		if (particles.size() > 0)
		{
			int tile_num = static_cast<int>(param.TileX * param.TileY * particles.at(0).get()->get_life_rate() * animation_rate);
			ImGui::DragInt("tile_num", &tile_num, 1, 1);
			float liferate = particles.at(0).get()->get_life_rate();
			ImGui::DragFloat("liferate", &liferate, 0.1f);
		}
		ImGui::DragInt("burst_num", &param.burst_num, 0.1f);
		ImGui::DragFloat("emit_timer", &emit_timer, 0.1f);
		ImGui::DragFloat("emit_span", &param.emit_span, 0.01f, 0.001f);
		ImGui::DragInt("TileX", &param.TileX, 1, 1);
		ImGui::DragInt("TileY", &param.TileY, 1, 1);


		ImGui::DragFloat("animation_rate", &animation_rate, 0.1f, 0);
		ImGui::DragFloat3("ang", &ang.x, 0.1f);

		ImGui::DragFloat2("texsize", &texsize.x, 1);

		int p_num = particles.size();
		//	int v_num = vertices.size();
		ImGui::DragInt("particle_num", &p_num);
		//	ImGui::DragInt("vertex_num", &v_num);
		ImGui::Checkbox("active", &active);

		ImGui::End();
		ImGui::PopID();

	}
#endif // USE_IMGUI

}
void SpriteEmitter::change_main_texture()
{
	if (SUCCEEDED(CoInitialize(nullptr))) { 
		Graphics& graphics = Graphics::instance();
		auto path =move(file_select(nullptr).get());
		if (path != nullptr)
		{
			load_texture_from_file(graphics.get_device().Get(), path, shader_resource_view.GetAddressOf(), &texture2d_desc);
		}
		CoUninitialize();
	}
}
//==============================================================
// 
//エミッターの位置更新
// 
//==============================================================
void SpriteEmitter::position_update(float elapsed_time)
{
	//位置更新
	position += velocity;
}
//==============================================================
// 
//エミッターの寿命更新
// 
//==============================================================
void SpriteEmitter::life_update(float elapsed_time)
{
	//ライフが切れたらアクティブ状態を着る
	if (life_timer > param.duration)
	{
		if (param.is_loop)
		{
			life_timer = 0.0f;
		}
		else
		{
			active = false;
		}
	}

	if (active)
	{
		life_timer += elapsed_time;
	}
	else
	{
		////生成開始時間を超えたらアクティブに
		//timer += elapsed_time;
		//if (timer > emit_start_time)
		//{
		//	active = true;
		//}
	}

	//パーティクルがアクティブでなくなったらリムーブのコンテナに放り込む
	for (auto& p : particles)
	{
		if (!p->get_is_active())
		{
			removes.push_back(move(p));
		}
	}
	std::erase(particles, nullptr);
}
void SpriteEmitter::orientation_update(float elapsed_time)
{
	DirectX::XMFLOAT3 Axis;
	Axis = Math::get_posture_right(orientation);
	orientation = Math::rot_quaternion(orientation, Axis, rotation.x);

	Axis = Math::get_posture_up(orientation);
	orientation = Math::rot_quaternion(orientation, Axis, rotation.y);

	Axis = Math::get_posture_forward(orientation);
	orientation = Math::rot_quaternion(orientation, Axis, rotation.z);
}
//==============================================================
// 
//パーティクルの破棄
// 
//==============================================================
void SpriteEmitter::remove_update()
{
	//破壊処理
	removes.clear();
}

void SpriteEmitter::particle_start_functions()
{
	param.particle_init_param.transition.position = position;
	param.particle_init_param.transition.velocity = param.init_speed * Math::get_posture_up(orientation);
}

void SpriteEmitter::particle_update_functions(float elapsed_time, Particles& p)
{
}

void SpriteEmitter::ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	Graphics& graphics = Graphics::instance();
	graphics.get_dc().Get()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, bufferSize);
	graphics.get_dc().Get()->Unmap(buffer, 0);
}


