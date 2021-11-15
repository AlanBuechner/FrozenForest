#ifndef MATH_H
#define MATH_H

namespace Math
{
	struct Vec3
	{
		Vec3 operator-();

		float x = 0, y = 0, z = 0;
	};

	float Dot(const Vec3& v1, const Vec3& v2);

}

#endif