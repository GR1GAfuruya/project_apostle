#include "particle.h"

#include "shader.h"
#include "SimpleMath.h"
#include "texture.h"
#include "imgui_include.h"

/***************
 未完成
 *************/

Particles::Particles(Graphics& graphics)
{
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Effects/Textures/Particle04.png", max_particles);

}

Particles::~Particles()
{
	
	vertices.clear();
}

void Particles::initialize(Graphics& graphics)
{
	////パーティクルの実態を生成してリストに追加
	//for(int i = 0; i < max_particles; i++)
	//{
	//	Particle* p = new Particle();
	//	particles.emplace_back(p);
	//}
	////頂点のリストのサイズをパーティクル分確保
	//vertices.resize(max_particles);
	//// 頂点バッファの生成
	//D3D11_BUFFER_DESC bufferDesc;
	//ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(Vertex) * max_particles;
	//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bufferDesc.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA subresourceData{};
	//ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//subresourceData.pSysMem = vertices.data(); //初期化データへのポインター
	//subresourceData.SysMemPitch = 0; //テクスチャーにある1本の線の先端から隣の線までの距離
	//subresourceData.SysMemSlicePitch = 0; //1つの深度レベルの先端から隣の深度レベルまでの距離
	//HRESULT hr{ S_OK };
	//hr = graphics.get_device().Get()->CreateBuffer(&bufferDesc, &subresourceData, vertex_buffer.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//

	//ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(int) * 6 * max_particles;
	//bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bufferDesc.CPUAccessFlags = 0;
	//// バッファ生成
	//ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	////subresourceData.pSysMem = index;
	//graphics.get_device().Get()->CreateBuffer(&bufferDesc, &subresourceData, &index_buffer);

	//create_vs_from_cso(graphics.get_device().Get(), "shaders/sprite_vs.cso", vertex_shader.GetAddressOf(), nullptr,nullptr,0);
	//create_ps_from_cso(graphics.get_device().Get(), "shaders/sprite_ps.cso", pixel_shader.GetAddressOf());


	//hr = load_texture_from_file(graphics.get_device().Get(), L".\\resources\\Sprite\\LoadingIcon.png", shader_resource_view.GetAddressOf(), &texture2d_desc);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 全パーティクル消滅状態
	data = new Particle[max_particles];
	for (int i = 0; i < max_particles; i++) {
		data[i].is_active = false;
	}
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

void Particles::update(Graphics& graphics,float elapsed_time)
{
	
	//emitter.emit_life_time += deltaTime;
	//if (emitter.emit_life_time <= emitter.emit_life) {
	//	//1125 エミッター
	//	emitter.emit_time += deltaTime;
	//	while (emitter.emit_time >= 1.0f / emitter.emit_rate)
	//	{
	//		emit();
	//		emitter.emit_time -= 1.0f / emitter.emit_rate;
	//	}
	//}

	for (int i = 0; i < emitter.emit_count; i++) {
		// Velocityに重力かける
		//data[i].Velocity.y += -9.8f * deltaTime;
		/*data[i].velocity.x += data[i].force.x * elapsed_time;
		data[i].velocity.y += data[i].force.y * elapsed_time;
		data[i].velocity.z += data[i].force.z * elapsed_time;*/
		data[i].position.x += data[i].velocity.x * elapsed_time;
		data[i].position.y += data[i].velocity.y * elapsed_time;
		data[i].position.z += data[i].velocity.z * elapsed_time;
		////1125 寿命判定
		//data[i].time += elapsed_time;
		//if (data[i].time >= data[i].life_time)
		//{
		//	data[i].time = 0.0f;
		//	data[i].is_active = false;
		//}

	}
	
}

void Particles::render(Graphics& graphics)
{
#if USE_IMGUI
	ImGui::Begin("CPU_Particle");
	ImGui::DragInt("particle_count", &emitter.emit_count);
	ImGui::End();
#endif // 0
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	sprite->begin(graphics.get_dc().Get());
	for (int i = 0; i < emitter.emit_count; i++)
	{
		sprite->render(graphics.get_dc().Get(), { data[i].position.x, data[i].position.y +5.0f}, { 0.2f, 0.2f });
	}
	sprite->end(graphics.get_dc().Get());
}

void Particles::emit()
{
	data[emitter.emit_count].is_active = true;
	data[emitter.emit_count].time = 0.0f;
	data[emitter.emit_count].life_time = 5;
	data[emitter.emit_count].scale = {5,5,5};

	data[emitter.emit_count].position = emitter.position;
	DirectX::XMFLOAT3 posrange = { 5,5,5 };
	data[emitter.emit_count].position.x +=
		posrange.x * (rand() % 1000 * 0.001f);
	data[emitter.emit_count].position.y +=
		posrange.y * (rand() % 1000 * 0.001f);
	data[emitter.emit_count].position.z +=
		posrange.z * (rand() % 1000 * 0.001f);

	data[emitter.emit_count].velocity = { 3.01f,0,0};
	data[emitter.emit_count].force = {1.01f,0,0};

	// カウント更新
	emitter.emit_count++;
	if (emitter.emit_count >= max_particles) emitter.emit_count = 0;
}

void Particles::set_emit_param(float life, float rate,DirectX::XMFLOAT2 emitpos)
{
	emitter.emit_life = life;
	emitter.emit_rate = rate;
	emitter.position = { emitpos.x,emitpos.y ,0};
	
}
