#include "emitter.h"
#include "coordinate_convert.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"
Emitter::Emitter(Graphics& graphics, int max_particles)
{
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Effects/Textures/Particle04.png", max_particles);
	this->max_particles = max_particles;
	emit_span = 1;
}

Emitter::~Emitter()
{
	particles.clear();
}

void Emitter::create_emitter(Graphics& graphics)
{
}

void Emitter::emit(Graphics& graphics)
{
	Particles::InitParam init_param;
	init_param.position = position;
	init_param.life_time = 5;
	init_param.scale = { 1,1 };
	init_param.velocity = emit_dir;

	particles.emplace_back(make_unique<Particles>(graphics, init_param));

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

	//エミット処理
	timer += elapsed_time;
	int p_size = static_cast<int>(particles.size());
	if (timer >= emit_span)
	{
		if (p_size < max_particles)
		{
			//パーティクル発生
			emit(graphics);
		}
		timer = 0;
	}

}

void Emitter::render(Graphics& graphics, Camera& camera)
{
	sprite->begin(graphics.get_dc().Get());
		//座標変換
	DirectX::XMFLOAT2 screen_pos;
	for (auto& p : particles)
	{
		screen_pos = conversion_2D(graphics.get_dc().Get(), camera, p->get_position());
		sprite->render(graphics.get_dc().Get(), screen_pos, p->get_scale(), p->get_color(), 0);
	}
	
	sprite->end(graphics.get_dc().Get());
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
		ImGui::DragFloat("life_time", &life_time, 0.1f);
		ImGui::DragFloat("emit_span", &emit_span, 0.1f);
		int p_num = particles.size();
		ImGui::DragInt("particle_num", &p_num, 0.1f);
		if (ImGui::Button("emit"))
		{
			emit(graphics);
		}
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
	life_time -= elapsed_time;

	if (timer > life_time)
	{
		active = false;
	}
	
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
	for (auto& r : removes)
	{
		//パーティクルの廃棄処理
		r.release();
	}
	//破棄リストをクリア
	removes.clear();
}