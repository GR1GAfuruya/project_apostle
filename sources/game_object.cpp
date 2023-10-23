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
