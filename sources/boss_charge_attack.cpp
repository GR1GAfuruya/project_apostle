#include "boss_charge_attack.h"
#include "user.h"
#include "noise.h"
#include "light_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
ChargeAttack::ChargeAttack(Graphics& graphics)
{
	//core�̏����ݒ�
	{
		core = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_sphere.fbx");
		core->set_material(MaterialManager::instance().mat_fire_distortion.get());
		core->set_init_color(FIRE_COLOR);
	}
	//wave�̏����ݒ�
	{
		wave = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/torus.fbx");
		wave->set_material(MaterialManager::instance().mat_fire_distortion.get());
		wave->set_init_color(FIRE_COLOR);
	}
	//tornado�̏����ݒ�
	{
		tornado = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
		tornado->set_material(MaterialManager::instance().mat_fire_distortion.get());
		tornado->set_init_color(FIRE_COLOR);
	}
	//��tornado�̏����ݒ�
	{
		tornado_black = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
		tornado_black->set_material(MaterialManager::instance().mat_fire_distortion.get());
		tornado_black->set_init_color({ 0.3f,0.2f,0.0f,0.8f });
	}
	//�\���G�t�F�N�g
	{
		omen_effect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_aura.fbx");
		omen_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		omen_effect->set_init_life_duration(0.5f);
		omen_effect->set_init_scale({0.0f, 0.0f, 11.0f});
		omen_effect->set_init_color({ FIRE_COLOR });
	}
	//�萔�o�b�t�@�����ݒ�
	constants = std::make_unique<Constants<ChargeAttackConstants>>(graphics.get_device().Get());
	//particle�̏����ݒ�
	{
		particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(), 70000);
		particle.get()->initialize(graphics);
		particle.get()->set_emitter_rate(6000);
		particle.get()->set_emitter_life_time(4);
		particle.get()->set_particle_life_time(5);
		particle.get()->set_particle_size(DirectX::XMFLOAT2(0.2f, 0.2f));
		particle.get()->set_color(FIRE_COLOR);

	}
	//���e�I
	{
		const int METEORE_MAX_NUM = 12;
		meteores = std::make_unique<Meteore>(graphics, METEORE_MAX_NUM);
		create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_emit.cso", emit_cs.ReleaseAndGetAddressOf());
		create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_update.cso", update_cs.ReleaseAndGetAddressOf());
		meteo_span = ATTACK_TIME / (meteores->get_max_num() + 1);
		meteo_launch_radius = 5;
	}
	//�g���l�[�h�̕����̃_���[�W�ݒ�
	{
		attack_param.power = 10;
		attack_param.invinsible_time = 0.5f;
	}

	const float range = 20.0f;
	boss_light = make_shared<PointLight>(graphics, position, range, DirectX::XMFLOAT3(FIRE_COLOR.x, FIRE_COLOR.y, FIRE_COLOR.z));

}
//==============================================================
// 
//����
// 
//==============================================================
void ChargeAttack::chant(DirectX::XMFLOAT3 pos)
{
	//�A�N�e�B�u��Ԃ�
	active = true;
	is_charge_max = false;
	attack_time = 0;
	charge_attack_update = &ChargeAttack::charging_update;
	//�^�C�}�[���Z�b�g
	life_time = 0.0f;
	//�R�A�ʒu�ݒ�
		position = pos;
		const float offset = 70;
		DirectX::XMFLOAT3 core_pos = { position.x, position.y + offset, position.z };
	{
		core->set_init_scale(0);
		core->play(core_pos);
		core->set_is_loop(true);
	}
	//wave�����ݒ�
	{
		DirectX::XMFLOAT3 wave_pos = { position.x, position.y + offset, position.z };
		wave->set_init_scale(0.0f);
		wave->play(wave_pos);
		wave->constants->data.threshold = 0;
	}
	//�g���l�[�h������
	{
		tornado->set_init_scale(0);
		tornado->play(pos);
		//tornado->constants->data.scroll_direction = { 0.0f,-0.2f };
		tornado->constants->data.threshold = 0;
	}
	//�g���l�[�h��������
	{
		tornado_black->set_init_scale(0);
		tornado_black->play(pos);
		//tornado_black->constants->data.scroll_direction = { 0.0f,-0.2f };
		tornado_black->constants->data.threshold = 0;
	}
	//�\���G�t�F�N�g������
	{
		omen_effect->play(pos);
	}
	//GPU�p�[�e�B�N��
	DirectX::XMFLOAT3 emit_pos{};
	emit_pos.y = core_pos.y;
	const float radius = 70.0f;
	for (float theta = 0; theta < 360.0f; theta += 120.0f)
	{
		emit_pos.x = core_pos.x + (radius * sinf(DirectX::XMConvertToRadians(theta)));
		emit_pos.z = core_pos.z + (radius * cosf(DirectX::XMConvertToRadians(theta)));
		particle.get()->set_emitter_pos(emit_pos);
		particle.get()->launch_emitter(emit_cs);
	}
	//�T�u�J���[�ݒ�
	const DirectX::XMFLOAT3 sub_color = { 3.0f,0.5f,3.8f };
	constants->data.particle_sub_color = sub_color;

	//覐�
	{
		meteo_time = 0;
		meteores->initialize();
		meteo_launch_count = 0;
		for (int i = 0; i < meteores->get_max_num(); i++)
		{
			const int range = 20;
			const float ofset = meteo_launch_radius;
			const float random = fabs(Noise::instance().random_range(ofset, range));
			meteores->create_on_circle(core->get_position(), random, i);
		}
	}
	//�R���W����������
	{
		tornado_colider.center = pos;
		tornado_colider.radius = 0;
	}
	//�R���X�^���g�o�b�t�@�̃R�A�|�W�V�����o�^
	constants->data.core_pos = core_pos;
	//���C�g�ǉ�
	boss_light->set_position({ position.x,position.y + 10.0f,position.z });
	LightManager::instance().register_light("charge_light", boss_light);
}
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
void ChargeAttack::stop()
{
	active = false;
	core->stop();

	wave->stop();

	LightManager::instance().delete_light(boss_light->name);
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void ChargeAttack::update(Graphics& graphics, float elapsed_time,Camera* camera)
{
	if (active)
	{
		life_time += elapsed_time;
		//�X�V
		(this->*charge_attack_update)(graphics, elapsed_time, camera);
		core->update(graphics, elapsed_time);
		wave->update(graphics, elapsed_time);
		tornado->update(graphics, elapsed_time);
		tornado_black->update(graphics, elapsed_time);
		omen_effect->update(graphics, elapsed_time);
	}
	meteores->update(graphics, elapsed_time);

	particle.get()->update(graphics.get_dc().Get(), elapsed_time, update_cs.Get());
}
//==============================================================
// 
//�`��
// 
//==============================================================
void ChargeAttack::render(Graphics& graphics, Camera* camera)
{

	if (active)
	{
		core->render(graphics, camera);

		tornado->render(graphics, camera);
		tornado_black->render(graphics, camera);
		wave->render(graphics, camera);
		omen_effect->render(graphics, camera);
	}

	//覐Ε`��
	meteores->render(graphics, camera);

	particle->render(graphics.get_dc().Get(), graphics.get_device().Get());

}
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
void ChargeAttack::debug_gui(const char* str_id)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "boss_charge", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("boss_charge");
		ImGui::DragFloat3("position", &position.x, 0.1f);
		ImGui::DragFloat("core_gravitation", &constants->data.core_gravitation, 0.1f, 0);
		ImGui::DragFloat3("particle_sub_color", &constants->data.particle_sub_color.x, 0.1f, 0);
		ImGui::DragFloat("meteo_span", &meteo_span, 0.1f, 0);
		ImGui::DragFloat("core_radius", &constants->data.core_radius, 1, 0);
		ImGui::DragFloat("launch_radius", &meteo_launch_radius, 1, 0);
		ImGui::DragFloat("radius", &tornado_colider.radius, 1, 0);
		ImGui::End();
	}
#endif
	core->debug_gui("core");

	wave->debug_gui("wave");
	tornado->debug_gui("tornad");
	tornado_black->debug_gui("tornado_black");
	particle->debug_gui("boss_charge");
}

//==============================================================
// 
//�v���C���[�Ƃ̓����蔻��
// 
//==============================================================
void ChargeAttack::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
	//���e�I�̓����蔻��
	meteores->calc_meteore_vs_player(capsule_start, capsule_end, colider_radius, damaged_func);

	if (Collision::sphere_vs_capsule(tornado_colider.center, tornado_colider.radius,
		capsule_start, capsule_end, colider_radius))
	{
		//�����蔻��̈ʒu�Ƒ傫���X�V
		damaged_func(attack_param.power, attack_param.invinsible_time, WINCE_TYPE::NONE);
	}
}
//==============================================================
// 
//�`���[�W���̃A�b�v�f�[�g
// 
//==============================================================
void ChargeAttack::charging_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	//�X�V����
	float core_scale = lerp(core->get_scale().x, 0.3f, 0.2f * elapsed_time);
	core->set_scale(core_scale);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(graphics, elapsed_time);

	//�R�A�̏d�͐ݒ�
	constants->data.core_gravitation = 0.5f;

	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);

	//覐΂�n�ʂ��畂���オ�点��
	float min = 1;
	float max = 9;
	for (int i = 0; i < meteores->get_max_num(); i++)
	{
		//�����_���ȑ��x
		float random_speed = fabs(Noise::instance().random_range(min, max));
		min = 2;
		max = 5;
		//�����_���ȃT�C�Y
		float random_size = fabs(Noise::instance().random_range(min, max));
		meteores->rising(elapsed_time, core->get_position(), random_size, random_speed, i);
	}

	//�\���G�t�F�N�g
	{
		if (!omen_effect->get_active()) omen_effect->play(position);
		//�g����J��Ԃ��Ĕg�`���ۂ�
		const float scale_speed = 108.0f;
		const float omen_add_scale = omen_effect->get_scale().x + scale_speed * elapsed_time;
		omen_effect->set_scale({ omen_add_scale, omen_add_scale, omen_effect->get_scale().z });
		omen_effect->rotate_base_axis(MeshEffect::AXIS::FORWARD, { 0,1,0 });
	}
	//�`���[�W�������̏���
	if (is_charge_max)
	{
		core->set_scale(0.1f);
		charge_attack_update = &ChargeAttack::activities_update;
		tornado->set_scale({ 0.0f,15.0f,0.0f });
		tornado_black->set_scale({ 0.0f,15.0f,0.0f });
	}
}
//==============================================================
// 
//�`���[�W���������A���������Ƃ��̃A�b�v�f�[�g
// 
//==============================================================
void ChargeAttack::activities_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	
	auto fade_out = [=](float alpha) {return (std::max)(alpha - 0.5f * elapsed_time, 0.0f); };

	//�R�A�G�t�F�N�g
	{
		const float core_s = lerp(core->get_scale().x, 10.0f, 1.5f * elapsed_time);
		core->set_scale(core_s);
		core->constants->data.scroll_speed += elapsed_time;
		core->update(graphics, elapsed_time);

		//�R�A�̏d�͐ݒ�
		//���]�����ăp�[�e�B�N�����юU�点��
		constants->data.core_gravitation = -0.4f;
		constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);
	}
	//�g���G�t�F�N�g
	{
		DirectX::XMFLOAT3 wave_scale = wave->get_scale();
		wave_scale.x = lerp(wave_scale.x, 6.0f, 5.0f * elapsed_time);
		wave_scale.z = lerp(wave_scale.z, 6.0f, 5.0f * elapsed_time);
		wave->constants->data.threshold = lerp(wave->constants->data.threshold, 1.0f, 3.0f * elapsed_time);
		if (wave->get_scale().x > 5.5f)
		{
			wave_scale.x = 0.0f;
			wave_scale.z = 0.0f;
			wave->constants->data.threshold = 0;
		}
		wave->set_scale(wave_scale);
		wave->constants->data.scroll_speed += elapsed_time;
		wave->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);
	}
	//�����G�t�F�N�g
	{
		float tornado_scale_xz = lerp(tornado->get_scale().x, 15.0f, 5.0f * elapsed_time);
		tornado->set_scale({ tornado_scale_xz, tornado->get_scale().y, tornado_scale_xz });
		tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 260 * elapsed_time);
	}
	//������
	{
		float tornado_black_scale_xz = lerp(tornado_black->get_scale().x, 17.0f, 7.0f * elapsed_time);
		tornado_black->set_scale({ tornado_black_scale_xz, tornado_black->get_scale().y, tornado_black_scale_xz });
		tornado_black->set_rotate_quaternion(MeshEffect::AXIS::UP, 240 * elapsed_time);
	}
	//�R���C�_�[�̔��a�ݒ�
	{
		tornado_colider.radius = 50.0f;
	}

	//���e�I����
	attack_time += elapsed_time;
	if (attack_time <= ATTACK_TIME || meteo_launch_count < meteores->get_max_num())
	{
		meteo_time += elapsed_time;
		if (meteo_time >= meteo_span )
		{
			//power
			const float min = 50;
			const float max = 100;
			const float random = Noise::instance().random_range(min, max);
			float power = random;
			//����
			DirectX::XMFLOAT3 direction = Math::circumferential_placement(core->get_position(),
				power,meteo_launch_count, meteores->get_max_num());
			//���K��
			direction = Math::Normalize(direction);
			//�ˏo
			meteores->launch(direction, power, meteo_launch_count);
			//�Ԋu�������邽�߂̃^�C�}�[�����Z�b�g
			meteo_time = 0;
			//�ˏo�����J�E���g�𑝂₷
			meteo_launch_count++;
		};

		
	}

	//�|�X�g�G�t�F�N�g�̃��W�A���u���[���s
	{
		float rate = 0.8f;
		const float radial_power = lerp(camera->get_post_effect()->get_radial_blur_power(), 0.6f, rate * elapsed_time);
		camera->get_post_effect()->set_radial_blur_power(radial_power);

	}
	if (attack_time >= ATTACK_TIME) charge_attack_update = &ChargeAttack::vanishing_update;
}
//==============================================================
// 
//���Ŏ��̃A�b�v�f�[�g
// 
//==============================================================
void ChargeAttack::vanishing_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	//���X�ɏ����Ă����֐�
	const float fade_out_speed = 7.0f;
	auto fade_out = [=](float alpha) {return (std::max)(alpha - fade_out_speed * elapsed_time, 0.0f); };

	wave->constants->data.particle_color.w = fade_out(wave->constants->data.particle_color.w);
	//wave->constants->data.scroll_speed += elapsed_time;
	const float threshold_speed = 2.0f;
	wave->constants->data.threshold = (std::min)(wave->constants->data.threshold + threshold_speed * elapsed_time, 1.0f);
	wave->set_rotate_quaternion(MeshEffect::AXIS::UP, 520 * elapsed_time);

	//�g���l�[�h�̏���
	{
		float expand = 15 * elapsed_time;
		tornado->set_scale({ tornado->get_scale().x + expand,tornado->get_scale().y + expand,tornado->get_scale().z + expand });
		tornado->constants->data.particle_color.w = fade_out(tornado->constants->data.particle_color.w);
		tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);

		//�g���l�[�h�̃f�B�]���u����
		const float dissolve_rate = 2.0f;
		const float start_threshold_time = 0.5f;
		if (tornado->constants->data.threshold <= 1.0f)
		{
			tornado->constants->data.threshold += dissolve_rate * elapsed_time;
		}
	}

	//�g���l�[�h�̍������̏���
	{
		float expand = 15 * elapsed_time;
		tornado_black->set_scale({ tornado_black->get_scale().x + expand,tornado_black->get_scale().y + expand,tornado_black->get_scale().z + expand });
		tornado_black->constants->data.particle_color.w = fade_out(tornado_black->constants->data.particle_color.w);
		tornado_black->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);

		//�g���l�[�h�̃f�B�]���u����
		const float dissolve_rate = 1.0f;
		const float start_threshold_time = 0.5f;
		if (tornado_black->constants->data.threshold <= 1.0f)
		{
			tornado_black->constants->data.threshold += dissolve_rate * elapsed_time;
		}
	}
	//�R���C�_�[�̔��a�ݒ�
	{
		tornado_colider.radius = 0.0f;
	}

	//�|�X�g�G�t�F�N�g�̃��W�A���u���[���s
	{
		float rate = 0.8f;
		const float radial_power = lerp(camera->get_post_effect()->get_radial_blur_power(), 0.0f, rate * elapsed_time);
		camera->get_post_effect()->set_radial_blur_power(radial_power);

	}

	//�g���l�[�h����������I��
	if (tornado_black->constants->data.threshold > 0.9f)
	{
		//0�ɓ��B���Ă��Ȃ������狭���I�ɂO��
		camera->get_post_effect()->set_radial_blur_power(0);

		stop();
	}
}
