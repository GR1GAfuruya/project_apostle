#include "boss_attack_skill1.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
BossAttackSkill1::BossAttackSkill1(Graphics& graphics)
{
	range = 10;
}

//==============================================================
// 
//����
// 
//==============================================================
void BossAttackSkill1::chant(DirectX::XMFLOAT3 pos)
{
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera) ->void {return charge_state_update(graphics, elapsed_time, camera); };
}

//==============================================================
// 
//��~
// 
//==============================================================
void BossAttackSkill1::stop()
{
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void BossAttackSkill1::update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	state_update(graphics, elapsed_time, camera);
}



//==============================================================
// 
//�`���[�W���Ă���Ƃ��̍X�V
// 
//==============================================================
void BossAttackSkill1::charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{

}

//==============================================================
// 
//�`���[�W���Ă���Ƃ��̍X�V
// 
//==============================================================
void BossAttackSkill1::attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{

}

//==============================================================
// 
//�`��
// 
//==============================================================
void BossAttackSkill1::render(Graphics& graphics)
{
}

//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void BossAttackSkill1::debug_gui(const char* str_id)
{

}

void BossAttackSkill1::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{

}
