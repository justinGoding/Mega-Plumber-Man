#include "Game_Engine.h"
#include "Assets.h"
#include "Scene_Play.h"
#include "Scene_Menu.h"

Game_Engine::Game_Engine(const std::string &path)
{
	initialize(path);
}

void Game_Engine::initialize(const std::string &path)
{
	m_assets.load_from_file(path);

	m_window.create(sf::VideoMode(1280, 768), "Definitely Not Mario");
	m_window.setFramerateLimit(60);

	change_scene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> Game_Engine::current_scene()
{
	return m_scene_map[m_current_scene];
}

bool Game_Engine::is_running()
{
	return m_running && m_window.isOpen();
}

const Assets &Game_Engine::assets() const
{
	return m_assets;
}

sf::RenderWindow &Game_Engine::window()
{
	return m_window;
}

void Game_Engine::run()
{
	while (is_running())
	{
		update();
	}
}

void Game_Engine::s_user_input()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			// if the current scene does not have an action associated with this key, skip the event
			if (current_scene()->get_action_map().find(event.key.code) == current_scene()->get_action_map().end()) { continue; }

			// determine the start or end action by whether it was key press or release
			const std::string action_type= (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send the action to the scene
			current_scene()->do_action(Action(current_scene()->get_action_map().at(event.key.code), action_type));
		}
	}
}

void Game_Engine::change_scene(const std::string &scene_name, std::shared_ptr<Scene> scene, bool end_current_scene)
{
	// If a scene was passed, add it to the map with the scene name
	if (scene)
	{
		m_scene_map[scene_name]= scene;
	}
	else
	{
		// If no scene was passed and the scene name is not in the map then return a warning and exit
		if (m_scene_map.find(scene_name) == m_scene_map.end())
		{
			std::cerr << "Could not find scene!";
			exit(-1);
		}
	}

	if (end_current_scene)
	{
		m_scene_map.erase(m_scene_map.find(m_current_scene));
	}

	m_current_scene= scene_name;
}

void Game_Engine::update()
{
	if (!is_running()) { return; }

	if (m_scene_map.empty()) { return; }

	s_user_input();
	current_scene()->update();	
	window().display();
}

void Game_Engine::quit()
{
	m_running= false; 
}