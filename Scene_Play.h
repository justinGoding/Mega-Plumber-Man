#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "Entity_Manager.h"

class Scene_Play : public Scene
{
	struct player_config
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct goomba_config
	{
		float CX, CY, SPEED, MAXSPEED, GRAVITY;
	};

protected:
	
	std::shared_ptr<Entity> m_player;
	std::string				m_level_path;
	player_config			m_player_config;
	goomba_config			m_goomba_config;
	bool					m_draw_textures= true;
	bool					m_draw_collision= false;
	bool					m_draw_grid= false;
	const c_Vec2			m_grid_size= { 64, 64 };
	sf::Text				m_grid_text;

	void initialize(const std::string &level_path);

	void load_level(const std::string &filename);

	virtual void on_end();

	void spawn_player();
	void spawn_bullet(std::shared_ptr<Entity> entity);
	void spawn_coin(std::shared_ptr<Entity> question);
	void spawn_enemy(std::string enemy_type, c_Vec2 grid_pos);

	c_Vec2 grid_to_mid_pixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	void			s_movement();
	void			s_lifespan();
	void			s_animation();
	virtual void	s_do_action(const Action &action);
	void			s_collision();
	virtual void	s_render();
	void			s_enemy_spawner();
	void			s_debug();

	void draw_line(const c_Vec2 &p1, const c_Vec2 &p2);

public:

	Scene_Play(Game_Engine *game_engine, const std::string &level_path);

	virtual void update();
};