#pragma once

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class Vector3					
{
public:
	float x, y, z;
	Vector3(void); 
	Vector3(float X, float Y, float Z);
	void Init(float ix=0.0f, float iy=0.0f, float iz=0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const Vector3& v) const;
	bool operator!=(const Vector3& v) const;	
	__forceinline Vector3&	operator+=(const Vector3 &v);			
	__forceinline Vector3&	operator-=(const Vector3 &v);		
	__forceinline Vector3&	operator*=(const Vector3 &v);			
	__forceinline Vector3&	operator*=(float s);
	__forceinline Vector3&	operator/=(const Vector3 &v);		
	__forceinline Vector3&	operator/=(float s);	
	__forceinline Vector3&	operator+=(float fl);
	__forceinline Vector3&	operator-=(float fl);
	inline float	Length() const;
	__forceinline float LengthSqr(void) const
	{ 
		CHECK_VALID(*this);
		return (x*x + y*y + z*z);		
	}
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance &&
				y > -tolerance && y < tolerance &&
				z > -tolerance && z < tolerance);
	}
	float	NormalizeInPlace();
	__forceinline float	DistToSqr(const Vector3 &vOther) const;
	float	Dot(const Vector3& vOther) const;			
	float	Length2D(void) const;		
	float	Length2DSqr(void) const;
	Vector3& operator=(const Vector3 &vOther);
	Vector3	operator-(void) const;
	Vector3	operator+(const Vector3& v) const;	
	Vector3	operator-(const Vector3& v) const;	
	Vector3	operator*(const Vector3& v) const;	
	Vector3	operator/(const Vector3& v) const;	
	Vector3	operator*(float fl) const;
	Vector3	operator/(float fl) const;
};
//===============================================
inline void Vector3::Init( float ix, float iy, float iz )    
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}
//===============================================
inline Vector3::Vector3(float X, float Y, float Z)
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}
//===============================================
inline Vector3::Vector3(void){ }
//===============================================
inline void Vector3::Zero()
{
	x = y = z = 0.0f;
}
//===============================================
inline void VectorClear( Vector3& a )
{
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector3& Vector3::operator=(const Vector3 &vOther)	
{
	CHECK_VALID(vOther);
	x=vOther.x; y=vOther.y; z=vOther.z; 
	return *this; 
}
//===============================================
inline float& Vector3::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline float Vector3::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline bool Vector3::operator==( const Vector3& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool Vector3::operator!=( const Vector3& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
__forceinline void VectorCopy( const Vector3& src, Vector3& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
__forceinline  Vector3& Vector3::operator+=(const Vector3& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}
//===============================================
__forceinline  Vector3& Vector3::operator-=(const Vector3& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}
//===============================================
__forceinline  Vector3& Vector3::operator*=(float fl)	
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector3& Vector3::operator*=(const Vector3& v)	
{ 
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector3&	Vector3::operator+=(float fl) 
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector3&	Vector3::operator-=(float fl) 
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector3& Vector3::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector3& Vector3::operator/=(const Vector3& v)	
{ 
	CHECK_VALID(v);
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline float Vector3::Length(void) const	
{
	CHECK_VALID(*this);
	
	float root = 0.0f;

	float sqsr = x*x + y*y + z*z;

	__asm
	{
		sqrtss xmm0, sqsr
		movss root, xmm0
	}

	return root;
}
//===============================================
inline float Vector3::Length2D(void) const
{
	CHECK_VALID(*this);
	
	float root = 0.0f;

	float sqst = x*x + y*y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
}
//===============================================
inline float Vector3::Length2DSqr(void) const
{ 
	return (x*x + y*y); 
}
//===============================================
inline Vector3 CrossProduct(const Vector3& a, const Vector3& b) 
{ 
	return Vector3( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); 
}
//===============================================
float Vector3::DistToSqr(const Vector3 &vOther) const
{
	Vector3 delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}
//===============================================
inline float Vector3::NormalizeInPlace()
{
	Vector3& v = *this;

	float iradius = 1.f / ( this->Length() + 1.192092896e-07F ); //FLT_EPSILON
	
	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
}
//===============================================
inline Vector3 Vector3::operator+(const Vector3& v) const	
{ 
	Vector3 res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;	
}
//===============================================
inline Vector3 Vector3::operator-(const Vector3& v) const	
{ 
	Vector3 res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;	
}
//===============================================
inline Vector3 Vector3::operator*(float fl) const	
{ 
	Vector3 res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;	
}
//===============================================
inline Vector3 Vector3::operator*(const Vector3& v) const	
{ 
	Vector3 res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;	
}
//===============================================
inline Vector3 Vector3::operator/(float fl) const	
{ 
	Vector3 res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;	
}
//===============================================
inline Vector3 Vector3::operator/(const Vector3& v) const	
{ 
	Vector3 res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float Vector3::Dot( const Vector3& vOther ) const
{
	const Vector3& a = *this;
	
	return( a.x*vOther.x + a.y*vOther.y + a.z*vOther.z ); 
}

#include <math.h>
inline Vector3 rotate_vec3d(Vector3 origin, float pitch, float yaw, Vector3 p)
{
	p -= origin;

	float newx = cos(yaw) * p.x + sin(yaw) * sin(pitch) * p.y - sin(yaw) * cos(pitch) * p.z;
	float newy = p.y = 0 + cos(pitch) * p.y + sin(pitch) * p.z;
	float newz = p.z = sin(yaw) * p.x + cos(yaw) * -sin(pitch) * p.y + cos(yaw) * cos(pitch) * p.z;

	return p + origin;
}

typedef float matrix3x4[3][4];
inline void VectorTransform(const Vector3& vSome, const matrix3x4& vMatrix, Vector3& vOut)
{
	for (auto i = 0; i < 3; i++)
		vOut[i] = vSome.Dot((Vector3&)vMatrix[i]) + vMatrix[i][3];
}

inline float VectorNormalize(Vector3 &vDirection)
{
	float flLength = vDirection.Length();

	if (!flLength)
		vDirection.x = 0, vDirection.y = 0;
	else
		vDirection /= flLength;

	return flLength;
}

#define PI 3.14159265358979323846f
#define DEG2RAD1( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG1( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
inline void AngleVectors(const Vector3 &angles, Vector3 *forward)
{

	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	sy = sinf(DEG2RAD1(angles[1])); // yaw
	cy = cosf(DEG2RAD1(angles[1]));

	sp = sinf(DEG2RAD1(angles[0])); // pitch
	cp = cosf(DEG2RAD1(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

inline void VectorAngles(Vector3 &forward, Vector3 &angles)
{
	float tmp, yaw, pitch;

	if (forward.y == 0 && forward.x == 0)
	{
		yaw = 0;

		if (forward.z > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = RAD2DEG1(atan2f(forward.y, forward.x));

		if (yaw < 0)
			yaw += 360;

		tmp = forward.Length2D();
		pitch = RAD2DEG1(atan2f(-forward.z, tmp));

		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

inline void ClampAngle(Vector3& qaAng)
{
	while (qaAng[0] > 89)
		qaAng[0] -= 180;

	while (qaAng[0] < -89)
		qaAng[0] += 180;

	while (qaAng[1] > 180)
		qaAng[1] -= 360;

	while (qaAng[1] < -180)
		qaAng[1] += 360;

	qaAng.z = 0;
}