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

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* cso_name_vs{ "shaders/sprite_particle_vs.cso" };
	//create_vs_from_cso(graphics.get_device().Get(), cso_name_vs, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* cso_name_ps{ "shaders/sprite_particle_ps.cso" };
	create_ps_from_cso(graphics.get_device().Get(), cso_name_ps, pixel_shader.GetAddressOf());
	// �摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g(ID3D11ShaderResourceView)�̐���
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
		//�G�~�b�g����
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
				//�^�C�}�[���Z�b�g
				emit_timer = 0;
			}
		}
	}
}


void Emitter::update(float elapsed_time)
{
	//�p�[�e�B�N���̍X�V
	for (auto& p : particles)
	{
		p->update(elapsed_time);
	}
	//�G�~�b�^�[�̎����X�V�A�p�[�e�B�N���̎�������
	life_update(elapsed_time);
	//�p�[�e�B�N���̔j������
	remove_update();
	//�A�N�e�B�u��ԂłȂ��Ƃ��͐����������s��Ȃ�
	if (!active) return;
	//�G�~�b�^�[�̈ʒu�X�V
	position_update(elapsed_time);
	
	//�G�~�b�g���ԏ���
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
	// �E���㎲�擾
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
	//�ʒu�X�V
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
		//�����J�n���Ԃ𒴂�����A�N�e�B�u��
		timer += elapsed_time;
		if (timer > emit_start_time)
		{
			active = true;
		}
	}

	//���C�t���؂ꂽ��A�N�e�B�u��Ԃ𒅂�
	if (life_timer > duration)
	{
		active = false;
	}
	//�p�[�e�B�N�����A�N�e�B�u����Ȃ��Ȃ����烊���[�u�̃R���e�i�ɕ��荞��
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
	//�j�󏈗�
	removes.clear();
}