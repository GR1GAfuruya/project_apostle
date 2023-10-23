#include "tutorial.h"

Tutorial::Tutorial()
{
	Graphics& graphics = Graphics::instance();
	//基本操作説明
	tex_basic_operation = std::make_unique<SpriteBatch>(graphics.get_device().Get(),
		L"./resources/Sprite/UI/Tutorial/operation.png", 1);
	//スキル選択説明
	tex_skill_select = std::make_unique<SpriteBatch>(graphics.get_device().Get(),
		L"./resources/Sprite/UI/Tutorial/slill_select.png", 1);
	//スキル発動説明
	tex_skill_chant = std::make_unique<SpriteBatch>(graphics.get_device().Get(),
		L"./resources/Sprite/UI/Tutorial/skill_chant.png", 1);
	//次に進むボタン
	tex_press_button = std::make_unique<SpriteBatch>(graphics.get_device().Get(),
		L"./resources/Sprite/UI/Tutorial/Press Button_A.png", 1);
	//チュートリアル展開
	tex_open_tutorial = std::make_unique<SpriteBatch>(graphics.get_device().Get(),
		L"./resources/Sprite/UI/Tutorial/open_tutorial.png", 1);
}

void Tutorial::initialize()
{
	Graphics& graphics = Graphics::instance();
	//初期はON
	is_tutorial = true;
	//選択されてるボード
	select_tutorial_board_num = 0;
	//基本操作説明の大きさと位置設定
	basic_operation_param.texsize = { static_cast<float>(tex_basic_operation->get_texture2d_desc().Width),
		static_cast<float>(tex_basic_operation->get_texture2d_desc().Height) };
	basic_operation_param.position = { skill_select_param.texsize.x * 0 ,0.0f };

	//スキル選択説明の大きさと位置設定
	skill_select_param.texsize = { static_cast<float>(tex_skill_select->get_texture2d_desc().Width),
		static_cast<float>(tex_skill_select->get_texture2d_desc().Height) };
	skill_select_param.position = { skill_select_param.texsize.x * 1 ,0.0f };

	//スキル発動説明の大きさと位置設定
	skill_chant_param.texsize = { static_cast<float>(tex_skill_chant->get_texture2d_desc().Width),
		static_cast<float>(tex_skill_chant->get_texture2d_desc().Height) };
	skill_chant_param.position = { skill_chant_param.texsize.x * 2 ,0.0f };

	//次に進むボタン
	press_button_param.position = { 540.0f,660.0f };
	press_button_param.scale = { 0.5f,0.5f };

	//チュートリアル画像展開
	open_tutorial_param.position = { 20.0f, 300.0f };
	open_tutorial_param.scale = { 0.6f,0.6f };
}

void Tutorial::update(float elapsed_time)
{
	GamePad& gamepad = Device::instance().get_game_pad();

	//基本操作→スキル選択説明→スキル発動説明の順で配置
	basic_operation_param.position = { skill_select_param.texsize.x * 0 - basic_operation_param.texsize.x * select_tutorial_board_num ,0.0f };
	skill_select_param.position = { skill_select_param.texsize.x * 1 - skill_select_param.texsize.x * select_tutorial_board_num ,0.0f };
	skill_chant_param.position = { skill_chant_param.texsize.x * 2 - skill_chant_param.texsize.x * select_tutorial_board_num ,0.0f };

	//Bボタンで次のボードへ
	if (gamepad.get_button_down() & GamePad::BTN_A)
	{
		next_bord();
	}

	//ボードが最終まで達したらチュートリアルを閉じリセット
	if (select_tutorial_board_num > MAX_TUTORIAL_BOARD_NUM)
	{
		select_tutorial_board_num = 0;
		is_tutorial = false;
	}
}

void Tutorial::render(ID3D11DeviceContext* dc)
{
	if (is_tutorial)
	{
		//基本操作説明
		tex_basic_operation->begin(dc);
		tex_basic_operation->render(dc, basic_operation_param.position, basic_operation_param.scale, basic_operation_param.pivot,
			basic_operation_param.color, basic_operation_param.angle, basic_operation_param.texpos, basic_operation_param.texsize);
		tex_basic_operation->end(dc);
		//スキル選択説明
		tex_skill_select->begin(dc);
		tex_skill_select->render(dc, skill_select_param.position, skill_select_param.scale, skill_select_param.pivot, skill_select_param.color,
			skill_select_param.angle, skill_select_param.texpos, skill_select_param.texsize);
		tex_skill_select->end(dc);
		//スキル発動説明
		tex_skill_chant->begin(dc);
		tex_skill_chant->render(dc, skill_chant_param.position, skill_chant_param.scale, skill_chant_param.pivot,
			skill_chant_param.color, skill_chant_param.angle, skill_chant_param.texpos, skill_chant_param.texsize);
		tex_skill_chant->end(dc);

		//次に進むボタン
		tex_press_button->begin(dc);
		tex_press_button->render(dc, press_button_param.position, press_button_param.scale);
		tex_press_button->end(dc);

	}

	debug_gui();
}

void Tutorial::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Tutorial", "tutorial", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("boss_charge");
		ImGui::DragFloat3("open_tutorial_param.position", &open_tutorial_param.position.x, 0.1f);
		ImGui::DragFloat2("open_tutorial_param.scale", &open_tutorial_param.scale.x, 0.1f, 0);
		ImGui::DragFloat3("press_button_param.position", &press_button_param.position.x, 0.1f);
		ImGui::DragFloat2("press_button_param.scale", &press_button_param.scale.x, 0.1f, 0);
		ImGui::End();
	}
#endif

}