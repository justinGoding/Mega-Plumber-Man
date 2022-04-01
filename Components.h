#pragma once

#include "Common.h"
#include "Animation.h"
#include "Assets.h"

class Component
{
public:
	bool has= false;
};
class c_Transform : public Component
{
public: 
	c_Vec2 position=			{ 0.0, 0.0 };
	c_Vec2 previous_position=	{ 0.0, 0.0 };
	c_Vec2 scale=				{ 1.0, 1.0 };
	c_Vec2 velocity=			{ 0.0, 0.0 };
	float  angle=				0;

	c_Transform() {}
	c_Transform(const c_Vec2 &p)
		: position(p) {}
	c_Transform(const c_Vec2 &p, const c_Vec2 &sp, const c_Vec2 &sc, float a)
		: position(p), previous_position(p), velocity(sp), scale(sc), angle(a) {}
};

class c_Lifespan : public Component
{
public:
	int lifespan= 0;
	int frame_created= 0;

	c_Lifespan() {}
	c_Lifespan(int duration, int frame)
		: lifespan(duration), frame_created(frame) {}
};

class c_Input : public Component
{
public:
	bool up=		false;
	bool down=		false;
	bool left=		false;
	bool right=		false;
	bool shoot=		false;
	bool can_shoot= true;
	bool can_jump=	true;

	c_Input() {}
};

class c_Bounding_box : public Component
{
public:
	c_Vec2 size;
	c_Vec2 half_size;

	c_Bounding_box() {}
	c_Bounding_box(const c_Vec2 &s)
		: size(s), half_size(s.x / 2, s.y / 2) {}
};

class c_Animation : public Component
{
public:
	Animation animation;
	bool repeat= false;
	
	c_Animation() {}
	c_Animation(const Animation &animation, bool r)
		: animation(animation), repeat(r) {}
};

class c_Gravity : public Component
{
public:
	float gravity= 0;

	c_Gravity() {}
	c_Gravity(float g) : gravity(g) {}
};

class c_State : public Component
{
public:
	std::string state= "jumping";
	
	c_State() {}
	c_State(const std::string &s) : state(s) {}
};

