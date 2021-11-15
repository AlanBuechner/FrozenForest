#ifndef CAMERA_H
#define CAMERA_H

#include "Math.h"

struct Camera
{
	Math::Vec3 Position;
	Math::Vec3 Dir;

	float fov = 70.0f;
	float near = 0.1f, far = 40.0f;
};

#endif