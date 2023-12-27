#include "emitter.h"
#include "coordinate_convert.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"
#include "texture.h"
Emitter::Emitter(int max_particles)
{
	Graphics& graphics = Graphics::instance();
	this->max_particles = 1;
	emit_span = 1;
	active = false;

	HRESULT hr{ S_OK };

	// 頂点シェーダーオブジェクトの生成
	const char* cso_name_vs{ "shaders/sprite_particle_vs.cso" };
	//create_vs_from_cso(graphics.get_device().Get(), cso_name_vs, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));

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
}

void Emitter::play(DirectX::XMFLOAT3 pos)
{
	active = true;

}


void Emitter::emit(float elapsed_time)
{
	if (active)
	{
		//エミット処理
		emit_timer += elapsed_time;

		if (emit_timer >= emit_span)
		{
			int p_size = static_cast<int>(particles.size());
			if (p_size < max_particles)
			{
				Particles::Param init_param;
				init_param.transition.position = transform.get_position();
				init_param.life_time = 0.5;
				init_param.scaling.scale = { 1,1,1 };
				init_param.transition.velocity = Math::get_posture_up(transform.get_orientation());
				for (int i = 0; i < burst_num; i++)
				{
					particles.emplace_back(make_unique<Particles>(init_param));
				}
				//タイマーリセット
				emit_timer = 0;
			}
		}
	}
}


void Emitter::update(float elapsed_time)
{
	//パーティクルの更新
	for (auto& p : particles)
	{
		p->update(elapsed_time);
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
	emit(elapsed_time);
}

void Emitter::render(Camera& camera)
{
	Graphics& graphics = Graphics::instance();
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


	graphics.get_dc()->VSSetShader(nullptr, nullptr, 0);
	graphics.get_dc()->GSSetShader(nullptr, nullptr, 0);
	graphics.get_dc()->PSSetShader(nullptr, nullptr, 0);
}

void Emitter::debug_gui(string id)
{
	imgui_menu_bar("effect", "emitter", display_imgui);
#if USE_IMGUI
	if (display_imgui)
	{
		ImGui::Begin(id.c_str());
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
	transform.set_position(transform.get_position() + velocity);
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