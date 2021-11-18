#include "Math.h"

namespace Math
{
	float Dot(const Vec3& v1, const Vec3& v2)
	{
		return v1.x*v2.x + v1.y*v2.y;
	}
}