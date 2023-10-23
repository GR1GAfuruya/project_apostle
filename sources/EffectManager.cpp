#include "EffectManager.h"
#include "transform.h"

std::shared_ptr<GameObject> EffectManager::create()
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

void EffectManager::remove(std::shared_ptr<GameObject> game_object)
{
	remove_game_objects.emplace_back(game_object);
}

void EffectManager::update(float elapsed_time)
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

	for (const std::shared_ptr<GameObject>& actor : remove_game_objects)
	{
		std::vector<std::shared_ptr<GameObject>>::iterator itStart = std::find(start_game_objects.begin(), start_game_objects.end(), actor);
		if (itStart != start_game_objects.end())
		{
			start_game_objects.erase(itStart);
		}

		std::vector<std::shared_ptr<GameObject>>::iterator itUpdate = std::find(update_game_objects.begin(), update_game_objects.end(), actor);
		if (itUpdate != update_game_objects.end())
		{
			update_game_objects.erase(itUpdate);
		}

		/*std::set<std::shared_ptr<GameObject>>::iterator itSelection = selectionActors.find(actor);
		if (itSelection != selectionActors.end())
		{
			selectionActors.erase(itSelection);
		}*/
	}
	remove_game_objects.clear();
}

void EffectManager::update_transform()
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

void EffectManager::render(Camera* camera)
{
	for (std::shared_ptr<GameObject>& game_object : update_game_objects)
	{
		game_object->render(camera);
		game_object->on_gui();
	}
}

