#include "Entity.h"
#include "Input.h"
#include "Time.h"

#include "Map.h"

#include <math.h>
#include <stdio.h>

void Entity::update()
{

	if(dir == Math::Vec3{0, 0, 0})
	{

		if(Input::IsKeyDown(Key::LEFT))
		{
			dir = Math::Vec3{-1, 0, 0};
			state = 3;
		}

		if(Input::IsKeyDown(Key::RIGHT))
		{
			dir = Math::Vec3{1, 0, 0};
			state = 1;
		}

		if(Input::IsKeyDown(Key::UP))
		{
			dir = Math::Vec3{0, 0, -1};
			state = 0;
		}
		
		if(Input::IsKeyDown(Key::DOWN))
		{
			dir = Math::Vec3{0, 0, 1};
			state = 2;
		}

		Math::Vec3 np = position + dir;
		uint32_t nx = (int)np.x / CHUNK_SIZE;
		uint32_t ny = (int)np.z / CHUNK_SIZE;
		uint32_t x = (int)position.x / CHUNK_SIZE;
		uint32_t y = (int)position.z / CHUNK_SIZE;
		if(dir != Math::Vec3{0, 0, 0} && (nx != x || ny != y))
		{
			currentChunk = GetCurrentChunk(nx, ny);
			surroundingChunks[0] = GetCurrentChunk(nx+1, ny);
			surroundingChunks[1] = GetCurrentChunk(nx-1, ny);
			surroundingChunks[2] = GetCurrentChunk(nx, ny+1);
			surroundingChunks[3] = GetCurrentChunk(nx, ny-1);
			surroundingChunks[4] = GetCurrentChunk(nx+1, ny+1);
			surroundingChunks[5] = GetCurrentChunk(nx-1, ny-1);
			surroundingChunks[6] = GetCurrentChunk(nx+1, ny-1);
			surroundingChunks[7] = GetCurrentChunk(nx-1, ny+1);
		}
			
		
		oldPosition = position;
		speed = (Input::IsKeyDown(Key::B) ? runSpeed : walkSpeed);
	}
	else
	{
		position += dir * DELTA_TIME * speed;

		float d = 1;
		if(dir.x != 0)
			d = abs((oldPosition.x + dir.x) - position.x);
		else if(dir.z != 0)
			d = abs((oldPosition.z + dir.z) - position.z);

		if(d == 0 || d == 1) frame = 0;
		else if(d < 0.33f && d > 0) frame = 1;
		else if(d < 0.66f && d > 0.33f) frame = 0;
		else if(d < 1 && d > 0.66) frame = 2;

		if(d < 0.01f)
		{
			position = oldPosition + dir;
			dir = Math::Vec3{0,0,0};
		}
	}
}