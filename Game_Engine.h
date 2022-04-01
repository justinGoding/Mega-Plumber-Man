#pragma once

#include "Common.h"
#include "Scene.h"
#include "Assets.h"

#include <memory>

typedef std::map<std::string, std::shared_ptr<Scene>> Scene_Map;

class Game_Engine
{
protected:

	sf::RenderWindow	m_window;
	Assets				m_assets;
	std::string			m_current_scene;
	Scene_Map			m_scene_map;
	size_t				m_simulation_speed= 1;
	bool				m_running= true;

	void initialize(const std::string &path);
	void update();

	void s_user_input();

	std::shared_ptr<Scene> current_scene();

public:

	Game_Engine(const std::string &path);					

	void change_scene(const std::string &scene_name, std::shared_ptr<Scene> scene, bool end_current_scene= false);

	void quit();
	void run();

	sf::RenderWindow &window();
	const Assets &assets() const;
	bool is_running();
};