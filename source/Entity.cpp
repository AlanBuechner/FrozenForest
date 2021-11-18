#include "Entity.h"
#include "Input.h"
#include "Time.h"

void Entity::update()
{

	if(dir == Math::Vec3{0, 0, 0})
	{
		if(Input::IsKeyDown(Key::LEFT))
			dir = Math::Vec3{-1, 0, 0};

		if(Input::IsKeyDown(Key::RIGHT))
			dir = Math::Vec3{1, 0, 0};

		if(Input::IsKeyDown(Key::UP))
			dir = Math::Vec3{0, 0, -1};
		
		if(Input::IsKeyDown(Key::DOWN))
			dir = Math::Vec3{0, 0, 1};
		
		oldPosition = position;
		speed = (Input::IsKeyDown(Key::B) ? runSpeed : walkSpeed);
	}
	else
	{
		position += dir * DELTA_TIME * speed;

		float d = 1;
		if(dir.x != 0)
			d = (oldPosition.x + dir.x) - position.x;
		else if(dir.z != 0)
			d = (oldPosition.z + dir.z) - position.z;
		
		if(d < 0.1f && d > -0.1f)
			dir = Math::Vec3{0,0,0};
	}
}