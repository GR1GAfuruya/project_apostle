#pragma once

#include <memory>
#include <vector>
#include <set>
#include <string>
#include <DirectXMath.h>
#include "graphics.h"
#include "camera.h"
//�O���錾
class Component;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject() {}
	virtual~GameObject() {}

	virtual void start();
	virtual void update(float elapsed_time);
	virtual void render(Camera* camera);
	virtual void on_gui();

	// �R���|�[�l���g�ǉ�
	template<class T, class... Args>
	std::shared_ptr<T> add_component(Args... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->set_game_object(shared_from_this());
		components.emplace_back(component);
		return component;
	}

	// �R���|�[�l���g�擾
	template<class T>
	std::shared_ptr<T> get_component()
	{
		for (std::shared_ptr<Component>& component : components)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}

	std::string get_name() { return name; }
	// ���O�̐ݒ�
	void set_name(const char* name) { this->name = name; }


private:
	std::string name;
	std::vector<std::shared_ptr<Component>> components;
};

