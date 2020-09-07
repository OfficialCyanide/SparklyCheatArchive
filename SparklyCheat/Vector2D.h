#pragma once
#include "SDK.h"

struct Vector2
{
public:
	float x, y;

	Vector2() {}
	Vector2(float x_, float y_) { x = x_; y = y_; }

	__forceinline Vector2&	operator+=(const Vector2 &v);

	Vector2& operator=(const Vector2 &vOther);
	Vector2 operator+(const Vector2 &v) const;
	Vector2 operator-(const Vector2 &v) const;
	Vector2 operator/(const Vector2 &v) const;
	Vector2 operator*(const Vector2 &v) const;
	Vector2 operator+(float fl) const;
	Vector2 operator-(float fl) const;
	Vector2 operator/(float fl) const;
	Vector2 operator*(float fl) const;

	float toAngle();
};

__forceinline  Vector2& Vector2::operator+=(const Vector2& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x; y += v.y;
	return *this;
}
inline Vector2& Vector2::operator=(const Vector2 &vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y;
	return *this;
}
inline Vector2 Vector2::operator+(const Vector2& v) const
{
	Vector2 res;
	res.x = x + v.x;
	res.y = y + v.y;
	return res;
}
inline Vector2 Vector2::operator-(const Vector2& v) const
{
	Vector2 res;
	res.x = x - v.x;
	res.y = y - v.y;
	return res;
}
inline Vector2 Vector2::operator/(const Vector2& v) const
{
	Vector2 res;
	res.x = x / v.x;
	res.y = y / v.y;
	return res;
}
inline Vector2 Vector2::operator*(const Vector2& v) const
{
	Vector2 res;
	res.x = x * v.x;
	res.y = y * v.y;
	return res;
}
inline Vector2 Vector2::operator+(float fl) const
{
	Vector2 res;
	res.x = x + fl;
	res.y = y + fl;
	return res;
}
inline Vector2 Vector2::operator-(float fl) const
{
	Vector2 res;
	res.x = x - fl;
	res.y = y - fl;
	return res;
}
inline Vector2 Vector2::operator/(float fl) const
{
	Vector2 res;
	res.x = x / fl;
	res.y = y / fl;
	return res;
}
inline Vector2 Vector2::operator*(float fl) const
{
	Vector2 res;
	res.x = x * fl;
	res.y = y * fl;
	return res;
}
inline float Vector2::toAngle()
{
	return atan2(this->x, this->y);
}

inline Vector2 rotate_vec2d(Vector2 origin, float radians, Vector2 p)
{
	float s = sin(radians);
	float c = cos(radians);

	// translate point back to origin:
	p.x -= origin.x;
	p.y -= origin.y;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + origin.x;
	p.y = ynew + origin.y;
	return p;
}
inline Vector2 vectorAngle(float radians, float length)
{
	return Vector2(sin(radians) * length, cos(radians) * length);
}