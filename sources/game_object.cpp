#include "component.h"
#include "game_object.h"
#include "transform.h"


void GameObject::start()
{

	for (int i = 0; i < components.size(); ++i)//”ÍˆÍFor•¶‚¾‚Æ—v‘f”‚ª•Ï‚í‚ç‚È‚¢‚Ì‚ÅSize‚Å
	{
		components[i]->start();
	}

}

void GameObject::update(float elapsed_time)
{
	for (std::shared_ptr<Component>& component : components)
	{
		component->update(elapsed_time);
	}
}

void GameObject::render(Camera* camera)
{
	for (std::shared_ptr<Component>& component : components)
	{
		component->render(camera);
	}
}

void GameObject::on_gui()
{
	for (std::shared_ptr<Component>& component : components)
	{
		component->on_gui();
	}

}

std::shared_ptr<GameObject> GameObjectManager::create()
{
	std::shared_ptr<GameObject> game_object = std::make_shared<GameObject>();
	{
		static int id = 0;
		char name[256];
		::sprintf_s(name, sizeof(name), "game_object%d", id++);
		game_object->set_name(name);
	}
	start_game_objects.emplace_back(game_object);
	return game_object;
}

void GameObjectManager::remove(std::shared_ptr<GameObject> game_object)
{
}

void GameObjectManager::update(float elapsed_time)
{
	for (std::shared_ptr<GameObject>& game_object : start_game_objects)
	{
		game_object->start();
		update_game_objects.emplace_back(game_object);
	}
	start_game_objects.clear();

	for (std::shared_ptr<GameObject>& game_object : update_game_objects)
	{
		game_object->update(elapsed_time);
	}
}

void GameObjectManager::update_transform()
{
	for (std::shared_ptr<GameObject>& game_object : update_game_objects)
	{
		std::shared_ptr<Transform> transform = game_object->get_component<Transform>();
		if (transform != nullptr)
		{
			transform->set_transform(Math::calc_world_matrix(transform->get_scale(),
				transform->get_orientation(), transform->get_position()));
		}
	}
}

void GameObjectManager::render(Camera* camera)
{
	for (std::shared_ptr<GameObject>& game_object : update_game_objects)
	{
		game_object->render(camera);
		game_object->on_gui();
	}
}
