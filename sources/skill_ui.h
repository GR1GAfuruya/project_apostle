#pragma once
#include "graphics.h"
#include "sprite_batch.h"
//==============================================================
// 
// SkillUI�N���X :�X�L����UI�Ɋւ��镔�����܂Ƃ߂��N���X
// 
//==============================================================
class SkillUI
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct SlotsUi
	{
		DirectX::XMFLOAT2 center_pos;
		float radius;
		float add_ang;
		DirectX::XMFLOAT2 icon_pos;
		DirectX::XMFLOAT2 texsize;
		float size;
		float expansion;
		float expansion_speed;
		DirectX::XMFLOAT4 color;

	};
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillUI(Graphics& graphics, const wchar_t* filename);
	~SkillUI(){}
	//������
	void initialize(SlotsUi init_param, int slots_num);
	//�X�V
	void update(Graphics& graphics,float elapsed_time);
	//�I�����̃X�L����\��
	void icon_render(Graphics& graphics);
	//���݃Z�b�g����Ă���X�L���̃A�C�R����\��
	void selected_skill_icon_render(Graphics& graphics, DirectX::XMFLOAT2 pos);
	//�f�o�b�OGUI
	void debug_gui(string str_id);
	//�X�L���I����Ԃ���ݒ�
	void set_skill_select(bool s) { skill_select = s; };
	//�I������Ă���X�L���̔ԍ���ݒ�
	void set_selected_skill_index(int i) { selected_skill_index = i; };
private:
	//�X�L���A�C�R���̎Q�Ɖ摜
	std::unique_ptr<SpriteBatch> skill_slot_icon;
	//�`��Ɋւ���p�����[�^�[
	SlotsUi slots_ui;
	//�X�L�����I�΂�Ă��邩
	bool skill_select;
	//�I������Ă���X�L���̔ԍ�
	int selected_skill_index;
	//�X�L���̑���
	int slots_num;
	//�X�L���A�C�R���̓W�J�Ɋւ���p�����[�^�[
	float expansion_start;
	float expansion_end;
	float add_alpha_speed;

	float add_ang_start;
	float add_ang_end;
	float add_ang_lerp_speed;
	//Imgui��\�����邩
	bool display_imgui = false;
};