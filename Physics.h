#pragma once

#include "Common.h"
#include "Entity.h"

namespace Physics
{
	c_Vec2 get_overlap(std::shared_ptr<Entity> a, std::shared_ptr <Entity> b);
	c_Vec2 get_previous_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
}