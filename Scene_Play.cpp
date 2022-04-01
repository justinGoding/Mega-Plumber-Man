#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "Game_Engine.h"
#include "Components.h"
#include "Action.h"

Scene_Play::Scene_Play(Game_Engine *game_engine, const std::string &level_path)
	: Scene(game_engine)
	, m_level_path(level_path)
{
	initialize(m_level_path);
}

void Scene_Play::initialize(const std::string &level_path)
{
	register_action(sf::Keyboard::P, "PAUSE");
	register_action(sf::Keyboard::Escape, "QUIT");
	register_action(sf::Keyboard::T, "TOGGLE_TEXTURE");		// Toggle drawing (T)extures
	register_action(sf::Keyboard::C, "TOGGLE_COLLISION");	// Toggle drawing (C)ollision Boxes
	register_action(sf::Keyboard::G, "TOGGLE_GRID");		// Toggle drawing (G)rid

	register_action(sf::Keyboard::D, "RIGHT");				// Toggle the player's right input
	register_action(sf::Keyboard::A, "LEFT");
	register_action(sf::Keyboard::W, "JUMP");
	register_action(sf::Keyboard::Space, "SHOOT");

	m_grid_text.setCharacterSize(12);
	m_grid_text.setFont(m_game->assets().get_font("Arial"));

	load_level(level_path);
}

c_Vec2 Scene_Play::grid_to_mid_pixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	// translates a grid position with cell sizes of 64 x 64 pixels to the pixel position

	c_Vec2 position;
	c_Vec2 entity_size= entity->get_component<c_Animation>().animation.get_size();

	position.x= gridX * m_grid_size.x + entity_size.x / 2;
	position.y= height() - (gridY * m_grid_size.y) - entity_size.y / 2; 

	return position;
}

void Scene_Play::load_level(const std::string &file_name)
{
	// reset the entity manager every time we load a level
	m_entity_manager= Entity_Manager();

	std::ifstream file(file_name);
	std::string string;

	while (file.good())
	{
		file >> string;

		if (string == "Tile")
		{
			std::string texture;
			c_Vec2 grid_pos;

			file >> texture >> grid_pos.x >> grid_pos.y;

			auto tile= m_entity_manager.add_entity(e_Tag::Tile);
			tile->add_component<c_Animation>(m_game->assets().get_animation(texture), true);
			tile->add_component<c_Transform>(grid_to_mid_pixel(grid_pos.x, grid_pos.y, tile));
			tile->add_component<c_Bounding_box>(m_game->assets().get_animation(texture).get_size());
		}
		else if (string == "Dec")
		{
			std::string texture;
			c_Vec2 grid_pos;

			file >> texture >> grid_pos.x >> grid_pos.y;

			auto dec= m_entity_manager.add_entity(e_Tag::Tile);
			dec->add_component<c_Animation>(m_game->assets().get_animation(texture), true);
			dec->add_component<c_Transform>(grid_to_mid_pixel(grid_pos.x, grid_pos.y, dec));
		}
		else if (string == "Player")
		{
			file >> m_player_config.X >> m_player_config.Y >> m_player_config.CX >> m_player_config.CY 
				>> m_player_config.SPEED >> m_player_config.JUMP >> m_player_config.MAXSPEED 
				>> m_player_config.GRAVITY >> m_player_config.WEAPON;

			spawn_player();
		}
		else if (string == "Goomba")
		{
			file >> m_goomba_config.CX >> m_goomba_config.CY >> m_goomba_config.SPEED >> m_goomba_config.MAXSPEED >> m_goomba_config.GRAVITY;
		}
		else if (string == "Enemy")
		{
			std::string enemy_type;
			c_Vec2 grid_pos;

			file >> enemy_type >> grid_pos.x >> grid_pos.y;

			spawn_enemy(enemy_type, grid_pos);
		}
	}

	// NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
	//		 Componenets are now returned as references rather than pointers
	//		 If you do not specify a reference variable type, it will COPY the component
	//		 Here is an example:
	//
	//		 This will COPY the transform into the variable 'transform1' - it is INCORRECT
	//		 Any changes you make to transform1 will not be changed inside the entity
	//		 auto transform1= entity->get<c_Transform>()
	//
	//		 This will REFERENCE the transform with the variable 'transform2' - it is CORRECT
	//		 Now any changes you make to transform2 will be changed inside the entity
	//		 auto &transform2= entity->get<c_Transform>()
}

void Scene_Play::spawn_player()
{
	m_player= m_entity_manager.add_entity(e_Tag::Player);
	m_player->add_component<c_Animation>(m_game->assets().get_animation("Stand"), true);
	m_player->add_component<c_Transform>(grid_to_mid_pixel(m_player_config.X, m_player_config.Y, m_player));
	m_player->add_component<c_Bounding_box>(c_Vec2(m_player_config.CX, m_player_config.CY));
	m_player->add_component<c_Input>();
	m_player->add_component<c_Gravity>(m_player_config.GRAVITY);
	m_player->add_component<c_State>("standing");
}

void Scene_Play::spawn_bullet(std::shared_ptr<Entity> entity)
{
	if (m_player->get_component<c_Input>().can_shoot)
	{
		c_Vec2 player_position= entity->get_component<c_Transform>().position;

		auto bullet= m_entity_manager.add_entity(e_Tag::Bullet);
		bullet->add_component<c_Animation>(m_game->assets().get_animation(m_player_config.WEAPON), true);
		bullet->add_component<c_Transform>(player_position);
		bullet->get_component<c_Transform>().velocity= c_Vec2(10 * entity->get_component<c_Transform>().scale.x, 0);
		bullet->add_component<c_Bounding_box>(m_game->assets().get_animation(m_player_config.WEAPON).get_size());
		bullet->add_component<c_Lifespan>(180, m_current_frame);
	}
}

void Scene_Play::spawn_coin(std::shared_ptr<Entity> question)
{
	c_Vec2 question_pos= question->get_component<c_Transform>().position;
	c_Vec2 coin_pos= question_pos;
	coin_pos.y-= 64;


	auto coin= m_entity_manager.add_entity(e_Tag::Dec);
	coin->add_component<c_Animation>(m_game->assets().get_animation("Coin"), false);
	coin->add_component<c_Transform>(coin_pos);
}

void Scene_Play::spawn_enemy(std::string enemy_type, c_Vec2 grid_pos)
{
	
	auto enemy= m_entity_manager.add_entity(e_Tag::Enemy);
	enemy->add_component<c_Animation>(m_game->assets().get_animation(enemy_type), true);
	enemy->add_component<c_Transform>(grid_to_mid_pixel(grid_pos.x, grid_pos.y, enemy));
	enemy->get_component<c_Transform>().velocity= c_Vec2(-m_goomba_config.SPEED, 0);
	enemy->add_component<c_Bounding_box>(c_Vec2(m_goomba_config.CX, m_goomba_config.CY));
	enemy->add_component<c_Gravity>(m_goomba_config.GRAVITY);

}

void Scene_Play::update()
{
	m_current_frame++;
	m_entity_manager.update();

	if (!m_paused)
	{
		s_movement();
		s_lifespan();
		s_animation();
	}

	s_collision();
	s_render();
}

void Scene_Play::s_movement()
{
	auto &player_transform= m_player->get_component<c_Transform>();
	auto &player_input= m_player->get_component<c_Input>();

	float player_velocity_x= 0.0f;

	// adds player's horizontal speed based on player input
	if (player_input.right)		{ player_velocity_x+= m_player_config.SPEED; }
	if (player_input.left)		{ player_velocity_x-= m_player_config.SPEED; }
	
	// If player inputs a jump and they can jump then it adds the jump
	// speed in the y direction and sets can_jump to false
	if (player_input.up && m_player->get_component<c_Input>().can_jump)
	{
		player_transform.velocity.y+= m_player_config.JUMP; 
		m_player->get_component<c_Input>().can_jump= false;
	}
	// If the player is no inputting jump and moving upwards and not
	// bouncing then the player's y velocity is set to 0 so they start falling
	else if (!player_input.up && player_transform.velocity.y < 0 && m_player->get_component<c_State>().state != "bouncing")
	{
		player_transform.velocity.y= 0.0f;
	}
	
	player_transform.velocity.x= player_velocity_x;
	
	

	// changes the direction the sprite is facing
	if (player_transform.velocity.x > 0)
	{
		player_transform.scale.x= 1;
	}
	else if (player_transform.velocity.x < 0)
	{
		player_transform.scale.x= -1;
	}
	
	// adds gravity in the y direction if an entity has
	// a gravity component
	// sets the previous position and new position
	for (auto &e : m_entity_manager.get_entities())
	{
		auto &transform= e->get_component<c_Transform>();

		if (e->get_component<c_Gravity>().has)
		{
			transform.velocity.y+=e->get_component<c_Gravity>().gravity;
		}
		transform.previous_position= transform.position;
		transform.position+= transform.velocity;
	}

	// Sets a max speed for the player 
	if (player_transform.velocity.x > m_player_config.MAXSPEED)
	{
		player_transform.velocity.x= m_player_config.MAXSPEED;
	}
	else if ((player_transform.velocity.x < -m_player_config.MAXSPEED))
	{
		player_transform.velocity.x= -m_player_config.MAXSPEED;
	}
	if (player_transform.velocity.y > m_player_config.MAXSPEED)
	{
		player_transform.velocity.y= m_player_config.MAXSPEED;
	}
	else if (player_transform.velocity.y < -m_player_config.MAXSPEED)
	{
		player_transform.velocity.y= -m_player_config.MAXSPEED;
	}
	
}

void Scene_Play::s_lifespan()
{
	for (auto &e : m_entity_manager.get_entities())
	{
		if (e->get_component<c_Lifespan>().has)
		{
			auto &lifespan= e->get_component<c_Lifespan>();
			if (m_current_frame >= lifespan.frame_created + lifespan.lifespan)
			{
				e->destroy();
			}
		}
	}
}

void Scene_Play::s_collision()
{
	c_Vec2 overlap;
	c_Vec2 previous_overlap;

	// bullet collisions
	for (auto &b : m_entity_manager.get_entities(e_Tag::Bullet))
	{
		// Collisions with tiles
		for (auto &t : m_entity_manager.get_entities(e_Tag::Tile))
		{
			overlap= Physics::get_overlap(b, t);

			if (overlap.x > 0 && overlap.y > 0)
			{
				b->destroy();

				if (t->get_component<c_Animation>().animation.get_name() == "Brick")
				{
					t->add_component<c_Animation>(m_game->assets().get_animation("Explosion"), false);
					t->remove_component<c_Bounding_box>();
				}
			}
		}
		// Collisions with enemies
		for (auto &e : m_entity_manager.get_entities(e_Tag::Enemy))
		{
			overlap= Physics::get_overlap(b, e);

			if (overlap.x > 0 && overlap.y > 0)
			{
				b->destroy();

				e->add_component<c_Animation>(m_game->assets().get_animation("Explosion"), false);
				e->remove_component<c_Bounding_box>();
				e->remove_component<c_Gravity>();
				e->get_component<c_Transform>().velocity= c_Vec2(0, 0);
			}
		}
	}

	// default state for player is air and can_jump set to false will
	// adjust these states when certain collision conditions are met
	if (m_player->get_component<c_State>().state != "bouncing")
	{
		m_player->get_component<c_State>().state= "air";
	}
	m_player->get_component<c_Input>().can_jump= false;

	// Collisions between the player and tiles
	for (auto &t : m_entity_manager.get_entities(e_Tag::Tile))
	{
		overlap= Physics::get_overlap(m_player, t);

		// If the two bounding boxes overlap
		if (overlap.x > 0 && overlap.y > 0)
		{
			previous_overlap= Physics::get_previous_overlap(m_player, t);

			// If the overlap is horizontal
			if (previous_overlap.y > 0)
			{
				// If the player came from the left, push them out to the left
				if (m_player->get_component<c_Transform>().position.x < t->get_component<c_Transform>().position.x)
				{
					m_player->get_component<c_Transform>().position.x-= overlap.x;
				}
				// If the player came from the right push them out to the right
				else
				{
					m_player->get_component<c_Transform>().position.x+= overlap.x;
				}
			}
			// If the overlap is vertical
			if (previous_overlap.x > 0)
			{
				// If the player comes from above, the player is then on the ground and can jump
				if (m_player->get_component<c_Transform>().position.y < t->get_component<c_Transform>().position.y)
				{
					m_player->get_component<c_Transform>().position.y-= overlap.y;
					m_player->get_component<c_State>().state= "ground";
					m_player->get_component<c_Input>().can_jump= true;
				}
				// If the player comes from below
				else
				{
					m_player->get_component<c_Transform>().position.y+= overlap.y;

					if (t->get_component<c_Animation>().animation.get_name() == "Question")
					{
						spawn_coin(t);
						t->add_component<c_Animation>(m_game->assets().get_animation("Quest_Bounce"), false);
					}
					if (t->get_component<c_Animation>().animation.get_name() == "Brick")
					{
						t->add_component<c_Animation>(m_game->assets().get_animation("Explosion"), false);
						t->remove_component<c_Bounding_box>();
					}
				}

				// Reset vertical speed upon vertical tile collisions
				m_player->get_component<c_Transform>().velocity.y= 0.0;
			}
			
		}

		// if the player passes the flag then reset the level
		if (t->get_component<c_Animation>().animation.get_name() == "PoleTop")
		{
			if (m_player->get_component<c_Transform>().position.x > t->get_component<c_Transform>().position.x)
			{
				m_game->change_scene("PLAY", std::make_shared<Scene_Play>(m_game, m_level_path));
			}
		}
	}

	// Enemy collisions
	for (auto &e : m_entity_manager.get_entities(e_Tag::Enemy))
	{
		// Enemy collisions with the player
		overlap= Physics::get_overlap(m_player, e);

		// If the bounding boxes overlap
		if (overlap.x > 0 && overlap.y > 0)
		{
			previous_overlap= Physics::get_previous_overlap(m_player, e);

			// If the overlap is vertical
			if (previous_overlap.x > 0)
			{
				// If the player comes from above destroy the enemy
				if (m_player->get_component<c_Transform>().position.y < e->get_component<c_Transform>().position.y)
				{
					m_player->get_component<c_Transform>().velocity.y= -10.0f;
					m_player->get_component<c_State>().state= "bouncing";
					e->add_component<c_Animation>(m_game->assets().get_animation("GoombaSquash"), false);
					e->remove_component<c_Bounding_box>();
					e->remove_component<c_Gravity>();
					e->get_component<c_Transform>().velocity= c_Vec2(0, 0);

				}
				// If from below then respawn the player by resetting the scene
				else
				{
					m_game->change_scene("PLAY", std::make_shared<Scene_Play>(m_game, m_level_path));
				}
			}
			// If the overlap is horizontal, respawn the player by resetting the scene
			if (previous_overlap.y > 0)
			{
				m_game->change_scene("PLAY", std::make_shared<Scene_Play>(m_game, m_level_path));
			}
		}

		// Collisions between enemies and tiles
		for (auto &t : m_entity_manager.get_entities(e_Tag::Tile))
		{
			overlap= Physics::get_overlap(e, t);

			// If the bounding boxes overlap
			if (overlap.x > 0 && overlap.y > 0)
			{
				previous_overlap= Physics::get_previous_overlap(e, t);

				// If the overlap is horizontal
				if (previous_overlap.y > 0)
				{
					if (e->get_component<c_Transform>().position.x < t->get_component<c_Transform>().position.x)
					{
						e->get_component<c_Transform>().position.x-= overlap.x;
					}
					else
					{
						e->get_component<c_Transform>().position.x+= overlap.x;
					}

					// Turn the enemy around when it hits a wall
					e->get_component<c_Transform>().velocity.x= -e->get_component<c_Transform>().velocity.x;
					e->get_component<c_Transform>().scale.x*= -1;
				}
				// If the overlap is vertical
				if (previous_overlap.x > 0)
				{
					if (e->get_component<c_Transform>().position.y < t->get_component<c_Transform>().position.y)
					{
						e->get_component<c_Transform>().position.y-= overlap.y;
					}
					else
					{
						e->get_component<c_Transform>().position.y+= overlap.y;
					}

					e->get_component<c_Transform>().velocity.y= 0.0;
				}
			}
		}

		// If the enemy falls down a hole, the enemy dies
		if (e->get_component<c_Transform>().position.y > height())
		{
			e->destroy();
		}

		// If the enemy leaves the left bounds of the map, the enemy dies
		if (e->get_component<c_Transform>().position.x < 0)
		{
			e->destroy();
		}
	}
	
	// If the player falls down a hole, reset the level
	if (m_player->get_component<c_Transform>().position.y > height())
	{
		m_game->change_scene("PLAY", std::make_shared<Scene_Play>(m_game, m_level_path));
	}
	
	// If the player tries to leave the left bounds of the map it reset their position within the bounds
	if (m_player->get_component<c_Transform>().position.x < 0)
	{
		m_player->get_component<c_Transform>().position.x= 0;
	}
}

void Scene_Play::s_do_action(const Action &action)
{
	if (action.type() == "START")
	{
			 if (action.name() == "TOGGLE_TEXTURE")		{ m_draw_textures= !m_draw_textures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_draw_collision= !m_draw_collision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_draw_grid= !m_draw_grid; }
		else if (action.name() == "PAUSE")				{ set_paused(); }
		else if (action.name() == "QUIT")				{ on_end(); }
		else if (action.name() == "RIGHT")				{ m_player->get_component<c_Input>().right= true; }
		else if (action.name() == "LEFT")				{ m_player->get_component<c_Input>().left= true; }
		else if (action.name() == "JUMP")				{ m_player->get_component<c_Input>().up= true;}
		else if (action.name() == "SHOOT")				{ spawn_bullet(m_player); m_player->get_component<c_Input>().can_shoot= false; }
	}
	else if (action.type() == "END")
	{
		if (action.name() == "RIGHT")					{ m_player->get_component<c_Input>().right= false; }
		if (action.name() == "LEFT")					{ m_player->get_component<c_Input>().left= false; }
		if (action.name() == "JUMP")					{ m_player->get_component<c_Input>().up= false;   }
		if (action.name() == "SHOOT")					{ m_player->get_component<c_Input>().can_shoot= true; }
	}
}

void Scene_Play::s_animation()
{
	// Adding a component like this will override the existing component
	/*
	
	m_player->add_component<c_Animation>(...); // creates component

	m_player->add_component<c_Animation>(...); // overrides existing component

	*/

	//	for each entity with an animation, call entity->get_component<c_Animation>().animation.update()
	//	if the animation is not repeated, and it has ended, destroy the entity
	std::string animation_name= m_player->get_component<c_Animation>().animation.get_name();
	std::string player_state= m_player->get_component<c_State>().state;

	if (player_state == "ground")
	{
		if (m_player->get_component<c_Transform>().velocity.x != 0 && animation_name != "Run")
		{
			m_player->add_component<c_Animation>(m_game->assets().get_animation("Run"), true);
		}
		else if (m_player->get_component<c_Transform>().velocity.x == 0 && animation_name != "Stand")
		{
			m_player->add_component<c_Animation>(m_game->assets().get_animation("Stand"), true);
		}
	}
	else if (player_state == "air" && animation_name != "Air")
	{
		m_player->add_component<c_Animation>(m_game->assets().get_animation("Air"), true);
	}

	for (auto &e : m_entity_manager.get_entities())
	{
		if (e->get_component<c_Animation>().has)
		{
			if (!e->get_component<c_Animation>().repeat && e->get_component<c_Animation>().animation.has_ended())
			{
				if (e->get_component<c_Animation>().animation.get_name() == "Quest_Bounce")
				{
					e->add_component<c_Animation>(m_game->assets().get_animation("Question2"), true);
				}
				else
				{
					e->destroy();
				}
			}
			else
			{
				e->get_component<c_Animation>().animation.update();
			}
		}
	}
}

void Scene_Play::on_end()
{
	m_game->change_scene("MENU", nullptr, true);
}

void Scene_Play::draw_line(const c_Vec2 &p1, const c_Vec2 &p2)
{
	sf::Vertex line[]={ sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Play::s_render()
{
	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else		   { m_game->window().clear(sf::Color(50, 50, 150)); }

	// set the viewpoint of the window to be centered on the player if it's far enough right
	auto &player_position= m_player->get_component<c_Transform>().position;
	float window_center_x= std::max(m_game->window().getSize().x / 2.0f, player_position.x);
	sf::View view= m_game->window().getView();
	view.setCenter(window_center_x, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	// draw all Entity textures / animations
	if (m_draw_textures)
	{
		for (auto e : m_entity_manager.get_entities())
		{
			auto &transform= e->get_component<c_Transform>();

			if (e->has_component<c_Animation>())
			{
				auto &animation= e->get_component<c_Animation>().animation;
				animation.get_sprite().setRotation(transform.angle);
				animation.get_sprite().setPosition(transform.position.x, transform.position.y);
				animation.get_sprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.get_sprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangle shape
	if (m_draw_collision)
	{
		for (auto e : m_entity_manager.get_entities())
		{
			if (e->has_component<c_Bounding_box>())
			{
				auto &box= e->get_component<c_Bounding_box>();
				auto &transform= e->get_component<c_Transform>();
				sf::RectangleShape rectangle;
				rectangle.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rectangle.setOrigin(sf::Vector2f(box.half_size.x, box.half_size.y));
				rectangle.setPosition(transform.position.x, transform.position.y);
				rectangle.setFillColor(sf::Color(0, 0, 0, 0));
				rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));
				rectangle.setOutlineThickness(1);
				m_game->window().draw(rectangle);
			}
		}
	}

	// draw the grid so that students can easily debug
	if (m_draw_grid)
	{
		float left_x= m_game->window().getView().getCenter().x - width() / 2;
		float right_x= left_x + width() + m_grid_size.x;
		float next_grid_x= left_x - ((int)left_x % (int)m_grid_size.x);

		for (float x= next_grid_x; x < right_x; x+= m_grid_size.x)
		{
			draw_line(c_Vec2(x, 0), c_Vec2(x, height()));
		}

		for (float y= 0; y < height(); y+= m_grid_size.y)
		{
			draw_line(c_Vec2(left_x, height() - y), c_Vec2(right_x, height() - y));

			for (float x= next_grid_x; x < right_x; x+= m_grid_size.x)
			{
				std::string x_cell= std::to_string((int)x / (int)m_grid_size.x);
				std::string y_cell= std::to_string((int)y / (int)m_grid_size.y);
				m_grid_text.setString("( " + x_cell + " , " + y_cell + " )");
				m_grid_text.setPosition(x + 3, height() - y - m_grid_size.y + 2);
				m_game->window().draw(m_grid_text);
			}
		}
	}
}