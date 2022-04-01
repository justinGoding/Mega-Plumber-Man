#include "Physics.h"
#include "Components.h"

c_Vec2 Physics::get_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	c_Vec2 overlap= c_Vec2(0, 0);

	if (a->get_component<c_Bounding_box>().has && b->get_component<c_Bounding_box>().has)
	{
		c_Vec2 a_position= a->get_component<c_Transform>().position;
		c_Vec2 b_position= b->get_component<c_Transform>().position;

		c_Vec2 a_half_size= a->get_component<c_Bounding_box>().half_size;
		c_Vec2 b_half_size= b->get_component<c_Bounding_box>().half_size;

		c_Vec2 delta= c_Vec2(abs(a_position.x - b_position.x), abs(a_position.y - b_position.y));

		float ox= a_half_size.x + b_half_size.x - delta.x;
		float oy= a_half_size.y + b_half_size.y - delta.y;

		overlap= c_Vec2(ox, oy);
	}

	return overlap;
}

c_Vec2 Physics::get_previous_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	c_Vec2 overlap= c_Vec2(0, 0);

	if (a->get_component<c_Bounding_box>().has && b->get_component<c_Bounding_box>().has)
	{
		c_Vec2 a_position= a->get_component<c_Transform>().previous_position;
		c_Vec2 b_position= b->get_component<c_Transform>().previous_position;

		c_Vec2 a_half_size= a->get_component<c_Bounding_box>().half_size;
		c_Vec2 b_half_size= b->get_component<c_Bounding_box>().half_size;

		c_Vec2 delta= c_Vec2(abs(a_position.x - b_position.x), abs(a_position.y - b_position.y));

		float ox= a_half_size.x + b_half_size.x - delta.x;
		float oy= a_half_size.y + b_half_size.y - delta.y;

		overlap= c_Vec2(ox, oy);
	}

	return overlap;
}