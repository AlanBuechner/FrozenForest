#ifndef MATH_H
#define MATH_H

namespace Math
{
	struct Vec3
	{
		inline Vec3 operator-() { return {-x, -y, -z}; }

		inline bool operator==(const Vec3& other) { return x == other.x && y == other.y && z == other.z; }
		inline Vec3 operator+(const Vec3& other) { return {x+other.x, y + other.y, z + other.z}; }
		inline Vec3 operator-(const Vec3& other) { return {x - other.x, y - other.y, z - other.z}; }
		inline Vec3 operator*(const float other) { return {x * other, y * other, z * other}; }
		inline Vec3 operator+=(const Vec3& other) { return (*this = *this + other); }


		float x = 0, y = 0, z = 0;
	};

	float Dot(const Vec3& v1, const Vec3& v2);

}

#endif