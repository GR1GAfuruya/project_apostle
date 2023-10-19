#pragma once
#include "game_object.h"
class Component
{
public:
	Component() {}
	virtual ~Component() {}

	// ���O�擾
	virtual const char* get_name() const = 0;


	virtual void start() {}
	virtual void update(float elapsed_time) = 0;
	virtual void render(Camera* camera) {}
	virtual void on_gui() {}

	// �A�N�^�[�ݒ�
	void set_game_object(std::shared_ptr<GameObject> game_object) { this->game_object = game_object; }

	// �A�N�^�[�擾
	std::shared_ptr<GameObject> get_game_object() { return game_object.lock(); }

private:
	std::weak_ptr<GameObject>	game_object;
};