#ifndef ENTITY_H
#define ENTITY_H
#include "Math.h"
#include <cstdint>

class Entity
{
public:
	Math::Vec3 position;
	Math::Vec3 oldPosition;
	Math::Vec3 dir;

	uint8_t state = 2;
	uint8_t frame = 0;

	float walkSpeed = 3;
	float runSpeed = 5;

	float speed = 3.0f;

public:
	void update();

};

#endif