#pragma once
#define _USE_MATH_DEFINES   // so we can use pi and other maths functions like e or sqrt

#include <cmath>
#include <algorithm>
#include <iostream>
#include "GamesEngineeringBase.h"
using namespace std;

// Macro - square - useful for squaring functions
#define SQ(x) ((x) * (x))

// Lerp Template
template<typename T>
static T lerp(const T a, const T b, float t)
{
	return a * (1.0f - t) + (b * t);
}

///////////////////////////////////////////////////////////////////////////////////////////////

class Vec3
{
public:
	union
	{
		float v[3];
		struct { float x, y, z; };  // v[0] is x, v[1] is y, v[2] is z
	};

	// Default Constructor
	Vec3() : x(0), y(0), z(0) {}

	// Constructor with parameters
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}


	Vec3 operator+(const Vec3& pVec) const   // takes a vec as input and add
	{
		return Vec3(
			v[0] + pVec.v[0], 
			v[1] + pVec.v[1], 
			v[2] + pVec.v[2]
		);   
	}


	Vec3 operator-(const Vec3& pVec) const    // takes a vec as input and subtracts
	{
		return Vec3(
			v[0] - pVec.v[0], 
			v[1] - pVec.v[1], 
			v[2] - pVec.v[2]
		);   
	}


	Vec3 operator*(const Vec3& pVec) const    // takes a vec as input and multiples
	{
		return Vec3(
			v[0] * pVec.v[0], 
			v[1] * pVec.v[1], 
			v[2] * pVec.v[2]
		);
	}


	Vec3 operator/(const Vec3& pVec) const    // divides vec by input vec
	{
		return Vec3(
			v[0] / pVec.v[0], 
			v[1] / pVec.v[1], 
			v[2] / pVec.v[2]
		);
	}


	Vec3& operator+=(const Vec3& pVec)    // add each component of vec input to this vector
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		return *this;
	}
	

	Vec3& operator-=(const Vec3& pVec)   // subtracts each component of vec input from this vector
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		return *this;
	}


	Vec3& operator*=(const Vec3& pVec)     // multiplies each component of vec input to this vector
	{
		v[0] *= pVec.v[0];
		v[1] *= pVec.v[1];
		v[2] *= pVec.v[2];
		return *this;
	}


	Vec3& operator/=(const Vec3& pVec)     // divides each component of vec input by this vector
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		return *this;
	}


	Vec3 operator*(const float val) const       // scales vec by scalar value
	{
		return Vec3(
			v[0] * val, 
			v[1] * val, 
			v[2] * val
		);
	}


	Vec3 operator/(const float val) const     // divides vector by scalar
	{
		return Vec3(
			v[0] / val, 
			v[1] / val, 
			v[2] / val
		);
	}


	Vec3 operator-() const    // unary negate, returns new vector pointing in opposite direction
	{
		return Vec3(
			-v[0], 
			-v[1], 
			-v[2]
		);
	}


	float length(Vec3& pVec) const   // length of a vector
	{
		return sqrt((pVec.v[0] * pVec.v[0]) + (pVec.v[1] * pVec.v[1]) + (pVec.v[2] * pVec.v[2]));
	}


	float lengthSquared(Vec3& pVec) const  // length squared of a vector
	{
		return ((pVec.v[0] * pVec.v[0]) + (pVec.v[1] * pVec.v[1]) + (pVec.v[2] * pVec.v[2]));
	}


	Vec3 normalize() const  // normalise a vector
	{
		float len = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		return Vec3(x * len, y * len, z * len);
	}


	float normalize_GetLength()   // normalises the vector and returns original length 
	{
		float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		float len = 1.0f / length;
		v[0] *= len; v[1] *= len; v[2] *= len;
		return length;
	}


	float Dot(const Vec3& pVec) const   // dot product between input vec and this vec
	{
		return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
	}


	Vec3 Cross(const Vec3& pVec) const    // cross product between this vec and input vec
	{
		return Vec3(
			v[1] * pVec.v[2] - v[2] * pVec.v[1],      
			v[2] * pVec.v[0] - v[0] * pVec.v[2],
			v[0] * pVec.v[1] - v[1] * pVec.v[0]
		);
	}


	Vec3 Max(const Vec3& pVec1, const Vec3& pVec2)    // returns a vec which is the maximum of two input vectors
	{
		return Vec3(
			max(pVec1.v[0], pVec2.v[0]),
			max(pVec1.v[1], pVec2.v[1]),
			max(pVec1.v[2], pVec2.v[2])
		);
	}


	Vec3 Min(const Vec3& pVec1, const Vec3& pVec2)     // returns a vec which is the minimum of two input vectors
	{
		return Vec3(
			min(pVec1.v[0], pVec2.v[0]),
			min(pVec1.v[1], pVec2.v[1]),
			min(pVec1.v[2], pVec2.v[2])
		);
	}


	float Max() const    // returns largest component of a vector
	{
		return max(x, max(y, z));   
	}


	float Min() const    // returns smallest component of a vector
	{
		return min(x, min(y, z));  
	}


	void Print(const Vec3& pVec)   // print a vector
	{
		cout << "(" << pVec.v[0] << ", " << pVec.v[1] << ", " << pVec.v[2] << ")" << std::endl;
	}
};


// Outside Vector class
float Dot(const Vec3& pVec1, const Vec3& pVec2)   // dot product between two input vectors
{
	return pVec1.x * pVec2.x + pVec1.y * pVec2.y + pVec1.z * pVec2.z;
}

// Outside Vector class
float Cross(const Vec3& pVec1, const Vec3& pVec2)    // cross product between two input vectors
{
	return (pVec1.v[1] * pVec2.v[2] - pVec1.v[2] * pVec2.v[1],
		pVec1.v[2] * pVec2.v[0] - pVec1.v[0] * pVec2.v[2],
		pVec1.v[0] * pVec2.v[1] - pVec1.v[1] * pVec2.v[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

class Vec4
{
public:
	union
	{
		float v[4];
		struct { float x, y, z, w; };
	};

	// Default Constructor
	Vec4() : x(0), y(0), z(0), w(0) {}


	// Constructor with parameters
	Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}


	// Constructor that takes a Vec3 with w = 1
	Vec4(const Vec3& pVec) : x(pVec.x), y(pVec.y), z(pVec.z), w(1.0f) {}


	Vec4 operator+(const Vec4& pVec) const   // adds input vec to this vec
	{

		return Vec4(
			v[0] + pVec.v[0], 
			v[1] + pVec.v[1], 
			v[2] + pVec.v[2], 
			v[3] + pVec.v[3]
		);
	}


	Vec4 operator-(const Vec4& pVec) const   // subtracts input vec from this vec
	{

		return Vec4(
			v[0] - pVec.v[0], 
			v[1] - pVec.v[1], 
			v[2] - pVec.v[2], 
			v[3] - pVec.v[3]
		);
	}


	Vec4 operator*(const Vec4& pVec) const    // multiplies input vec by this vec
	{
		return Vec4(
			v[0] * pVec.v[0], 
			v[1] * pVec.v[1], 
			v[2] * pVec.v[2], 
			v[3] * pVec.v[3]
		);
	}


	Vec4 operator/(const Vec4& pVec)    // divides vec by input vec
	{
		return Vec4(
			v[0] / pVec.v[0], 
			v[1] / pVec.v[1], 
			v[2] / pVec.v[2], 
			v[3] / pVec.v[3]
		);
	}


	Vec4& operator+=(const Vec4& pVec)    // adds each component of input vec to this vec
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		v[3] += pVec.v[3];
		return *this;
	}


	Vec4& operator-=(const Vec4& pVec)    // subtracts each component of input vec from this vec
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		v[3] -= pVec.v[3];
		return *this;
	}


	Vec4& operator*=(const Vec4& pVec)    // multiply each component of input vec to this vec
	{
		v[0] *= pVec.v[0];
		v[1] *= pVec.v[1];
		v[2] *= pVec.v[2];
		v[3] *= pVec.v[3];
		return *this;
	}


	Vec4& operator/=(const Vec4& pVec)    // divides each component of vec by input vec
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		v[3] /= pVec.v[3];
		return *this;
	}


	Vec4 operator*(const float val) const       // scales vec by scalar value
	{
		return Vec4(
			v[0] * val, 
			v[1] * val, 
			v[2] * val, 
			v[3] * val
		);
	}


	Vec4 operator/(const float val) const     // divide vec by scalar value
	{
		return Vec4(
			v[0] / val, 
			v[1] / val, 
			v[2] / val, 
			v[3] * val
		);
	}


	Vec4 operator-() const    // unary negate, returns new vector pointing in opposite direction
	{
		return Vec4(
			-v[0], 
			-v[1], 
			-v[2], 
			-v[3]
		);
	}


	Vec4 Max(const Vec4& pVec1, const Vec4& pVec2)    // returns a vec which is the maximum of two input vectors
	{
		return Vec4(
			max(pVec1.v[0], pVec2.v[0]),
			max(pVec1.v[1], pVec2.v[1]),
			max(pVec1.v[2], pVec2.v[2]),
			max(pVec1.v[3], pVec2.v[3])
			);
	}


	Vec4 Min(const Vec4& pVec1, const Vec4& pVec2)     // returns a vec which is the minimum of two input vectors
	{
		return Vec4(
			min(pVec1.v[0], pVec2.v[0]),
			min(pVec1.v[1], pVec2.v[1]),
			min(pVec1.v[2], pVec2.v[2]),
			min(pVec1.v[3], pVec2.v[3])
			);
	}


	float Max() const    // returns maximum component of a vec
	{
		return max(x, max(y, max(z, w)));
	}


	float Min() const    // returns minimum component of a vec
	{
		return min(x, min(y, min(z, w)));
	}


	void Print(const Vec4& pVec)   // print a vector
	{
		cout << "(" << pVec.v[0] << ", " << pVec.v[1] << ", " << pVec.v[2] << ", " << pVec.v[3] << ")" << std::endl;
	}

	void divisionByW(Vec4& pVec)    // converts point from homogenous space to 3D cartesian space
	{								// without this, objects won't appear smaller as you move further away					
		float inv = 1 / pVec.w;
		pVec.x *= inv;
		pVec.y *= inv;
		pVec.z *= inv;
		pVec.w = 1.0f;
	}

	Vec4 vertexToScreenSpace(Vec4& pVec, int width, int height)  // Converts a vertex from normalized device coordinates(NDC) to screen space pixel coordinates
	{
		return Vec4(
			((1.0f + pVec.x) / 2.0f) * width,
			((1.0f - pVec.y) / 2.0f)* height,
			pVec.z,
			1.0f
			);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////

class Matrix    // row major, not column major
{
public:
	union
	{
		float a[4][4];
		float m[16];
	};


	Matrix()     // default constructor, identity matrix in constructor
	{
		a[0][0] = 1; a[0][1] = 0; a[0][2] = 0; a[0][3] = 0;
		a[1][0] = 0; a[1][1] = 1; a[1][2] = 0; a[1][3] = 0;
		a[2][0] = 0; a[2][1] = 0; a[2][2] = 1; a[2][3] = 0;
		a[3][0] = 0; a[3][1] = 0; a[3][2] = 0; a[3][3] = 1;
	}


	Matrix(float m0, float m1, float m2, float m3,       // constructor to initialise elements in matrix
		float m4, float m5, float m6, float m7,
		float m8, float m9, float m10, float m11, 
		float m12, float m13, float m14, float m15)     
	{
		m[0] = m0; m[1] = m1; m[2] = m2; m[3] = m3;
		m[4] = m4; m[5] = m5; m[6] = m6; m[7] = m7;
		m[8] = m8; m[9] = m9; m[10] = m10; m[11] = m11;
		m[12] = m12; m[13] = m13; m[14] = m14; m[15] = m15;
	}


	Vec4 mul(const Vec4& pVec)   // multiply matrix by input vec4
	{
		return Vec4(
			pVec.x * m[0] + pVec.y * m[1] + pVec.z * m[2] + pVec.w * m[3],
			pVec.x * m[4] + pVec.y * m[5] + pVec.z * m[6] + pVec.w * m[7],
			pVec.x * m[8] + pVec.y * m[9] + pVec.z * m[10] + pVec.w * m[11],
			pVec.x * m[12] + pVec.y * m[13] + pVec.z * m[14] + pVec.w * m[15]
		);
	}


	Vec3 mulPoint(const Vec3& pVec)  // multiply matrix by vec3 to store posiiton
	{
		return Vec3(
			(pVec.x * m[0] + pVec.y * m[1] + pVec.z * m[2]) + m[3],
			(pVec.x * m[4] + pVec.y * m[5] + pVec.z * m[6]) + m[7],
			(pVec.x * m[8] + pVec.y * m[9] + pVec.z * m[10]) + m[11]
		);
	}


	Vec3 mulVec(const Vec3& pVec)   // multiply matrix by vec3 to store directions
	{
		return Vec3(
			(pVec.x * m[0] + pVec.y * m[1] + pVec.z * m[2]),
			(pVec.x * m[4] + pVec.y * m[5] + pVec.z * m[6]),
			(pVec.x * m[8] + pVec.y * m[9] + pVec.z * m[10])
		);
	}


	void translation(const Vec3& pVec)   // Translate a 4x4 matrix
	{
		m[3] += pVec.x;
		m[7] += pVec.y;
		m[11] += pVec.z;
	}


	void scaling(const Vec3& pVec)   // scale a 4x4 matrix by input vec3
	{
		m[0] = pVec.x;
		m[5] = pVec.y;
		m[10] = pVec.z;
	}


	void rotAroundX(float angle)  // rotate a 4x4 matrix around the x-axis
	{
		m[5] = cosf(angle);
		m[6] = -sinf(angle);
		m[9] = sinf(angle);
		m[10] = cosf(angle);
	}


	void rotAroundY(float angle)   // rotate a 4x4 matrix around the y-axis
	{
		m[0] = cosf(angle);
		m[2] = sinf(angle);
		m[8] = -sinf(angle);
		m[10] = cosf(angle);
	}


	void rotAroundZ(float angle)   // rotate a 4x4 matrix around the z-axis
	{
		m[0] = cosf(angle);
		m[1] = -sinf(angle);
		m[4] = sinf(angle);
		m[5] = cosf(angle);
	}


	Matrix multiply(const Matrix& matrix) const    // multiply this matrix by input matrix
	{
		Matrix ret;
		ret.m[0] = m[0] * matrix.m[0] + m[1] * matrix.m[4] + m[2] * matrix.m[8] + m[3] * matrix.m[12];
		ret.m[1] = m[0] * matrix.m[1] + m[1] * matrix.m[5] + m[2] * matrix.m[9] + m[3] * matrix.m[13];
		ret.m[2] = m[0] * matrix.m[2] + m[1] * matrix.m[6] + m[2] * matrix.m[10] + m[3] * matrix.m[14];
		ret.m[3] = m[0] * matrix.m[3] + m[1] * matrix.m[7] + m[2] * matrix.m[11] + m[3] * matrix.m[15];
		ret.m[4] = m[4] * matrix.m[0] + m[5] * matrix.m[4] + m[6] * matrix.m[8] + m[7] * matrix.m[12];
		ret.m[5] = m[4] * matrix.m[1] + m[5] * matrix.m[5] + m[6] * matrix.m[9] + m[7] * matrix.m[13];
		ret.m[6] = m[4] * matrix.m[2] + m[5] * matrix.m[6] + m[6] * matrix.m[10] + m[7] * matrix.m[14];
		ret.m[7] = m[4] * matrix.m[3] + m[5] * matrix.m[7] + m[6] * matrix.m[11] + m[7] * matrix.m[15];
		ret.m[8] = m[8] * matrix.m[0] + m[9] * matrix.m[4] + m[10] * matrix.m[8] + m[11] * matrix.m[12];
		ret.m[9] = m[8] * matrix.m[1] + m[9] * matrix.m[5] + m[10] * matrix.m[9] + m[11] * matrix.m[13];
		ret.m[10] = m[8] * matrix.m[2] + m[9] * matrix.m[6] + m[10] * matrix.m[10] + m[11] * matrix.m[14];
		ret.m[11] = m[8] * matrix.m[3] + m[9] * matrix.m[7] + m[10] * matrix.m[11] + m[11] * matrix.m[15];
		ret.m[12] = m[12] * matrix.m[0] + m[13] * matrix.m[4] + m[14] * matrix.m[8] + m[15] * matrix.m[12];
		ret.m[13] = m[12] * matrix.m[1] + m[13] * matrix.m[5] + m[14] * matrix.m[9] + m[15] * matrix.m[13];
		ret.m[14] = m[12] * matrix.m[2] + m[13] * matrix.m[6] + m[14] * matrix.m[10] + m[15] * matrix.m[14];
		ret.m[15] = m[12] * matrix.m[3] + m[13] * matrix.m[7] + m[14] * matrix.m[11] + m[15] * matrix.m[15];
		return ret;
	}


	float& operator[](int index)   // operator for natural access
	{
		return m[index];
	}


	Matrix invert()    // inverse of a 4x4 matrix
	{
		Matrix inv;
		inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
		inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
		inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
		inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
		inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
		inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
		inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
		inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
		inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
		inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
		inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
		inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
		inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
		float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
		
		if (det == 0) 
		{
			// Handle this case 
		}
		det = 1.0 / det;
		for (int i = 0; i < 16; i++) 
		{
			inv[i] = inv[i] * det;
		}
		return inv;
	}


	Matrix transpose(const Matrix& matrix) const   // transpose a 4x4 matrix
	{
		Matrix m;
		m.m[0] = matrix.m[0];
		m.m[1] = matrix.m[4];
		m.m[2] = matrix.m[8];
		m.m[3] = matrix.m[12];
		m.m[4] = matrix.m[1];
		m.m[5] = matrix.m[5];
		m.m[6] = matrix.m[9];
		m.m[7] = matrix.m[13];
		m.m[8] = matrix.m[2];
		m.m[9] = matrix.m[6];
		m.m[10] = matrix.m[10];
		m.m[11] = matrix.m[14];
		m.m[12] = matrix.m[3];
		m.m[13] = matrix.m[7];
		m.m[14] = matrix.m[11];
		m.m[15] = matrix.m[15];
		return m;
	}

	Matrix perspectiveProjection(float _aspect, float _fov, float _near, float _far)
	{
		Matrix m;
		float tanHalfFov = tan(_fov / 2.0f);
		m.m[0] = 1.0f / (_aspect * tanHalfFov);
		m.m[5] = 1.0f / tanHalfFov;
		m.m[10] = _far / (_far - _near);
		m.m[11] = -(_far * _near) / (_far - _near);
		m.m[14] = 1.0f;
		m.m[15] = 0;
		return m;
	} 

	Matrix lookAtMatrix(const Vec3& _to, const Vec3& _from, const Vec3& _up)
	{
		Matrix m;
		Vec3 direction = (_to - _from).normalize();
		Vec3 right = _up.Cross(direction).normalize();
		Vec3 correctedUp = direction.Cross(right).normalize();

		m.m[0] = right.x;
		m.m[1] = right.y;
		m.m[2] = right.z;
		m.m[3] = -_from.Dot(right);
		m.m[4] = correctedUp.x;
		m.m[5] = correctedUp.y;
		m.m[6] = correctedUp.z;
		m.m[7] = -_from.Dot(correctedUp);
		m.m[8] = direction.x;
		m.m[9] = direction.y;
		m.m[10] = direction.z;
		m.m[11] = -_from.Dot(direction);
		m.m[12] = 0.0f;
		m.m[13] = 0.0f;
		m.m[14] = 0.0f;
		m.m[15] = 1.0f;

		return m;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class SphereCoordinates
{
public:
	
	float theta, phi;
	
	// Default Constructor
	SphereCoordinates() : theta(0), phi(0) {}

	// Spherical Coordinates Constructor
	SphereCoordinates(float _theta, float _phi) : theta(_theta), phi(_phi) {}

	// Cartesian Coordinates Constructor
	SphereCoordinates(float x, float y, float z) 
	{
		FromVec3(x, y, z);
	}

	// Converts Cartesian to Spherical
	void FromVec3(float x, float y, float z)    // Obtain theta and phi from vector - z-up shading
	{
		theta = acos(x / z);
		phi = atan2f(y , x);
	}

	// Converts Sphereical to Cartesian
	Vec3 ToVec3()                         // returns a Vec3 given theta and phi - z-up shading
	{
		return Vec3(
			sinf(theta) * cosf(phi),
			sinf(theta) * sinf(phi),
			cosf(theta));
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class Quaternion
{
public:
	union
	{
		struct
		{
			float a, b, c, d;
		};

		float q[4];       // union allows array style access
	};

	// Standard Constructor to initialise the quaternion components
	Quaternion(float x, float y, float z, float w)
	{
		a = x;
		b = y;
		c = z;
		d = w;
	}

	Matrix toMatrix()    // Converts quaternion to 4x4 rotation matrix
	{
		float aa = a * a, ab = a * b, ac = a * c;
		float bb = b * b, bc = b * c, cc = c * c;
		float da = d * a, db = d * b, dc = d * c;
		Matrix m;
		m[0] = 1 - 2 * (bb + cc); m[1] = 2 * (ab - dc); m[2] = 2 * (ac + db); m[3] = 0;
		m[4] = 2 * (ab + dc); m[5] = 1 - 2 * (aa + cc); m[6] = 2 * (bc - da); m[7] = 0;
		m[8] = 2 * (ac - db); m[9] = 2 * (bc + da); m[10] = 1 - 2 * (aa + bb); m[11] = 0;
		m[12] = m[13] = m[14] = 0; m[15] = 1;
		return m;
	}

	Quaternion Normalised() const      // returns a unit Quaternion
	{
		float magnitude = sqrtf(a * a + b * b + c * c + d * d);

		if (magnitude > 0.0f)
		{
			float inverse = 1.0f / magnitude;

			return Quaternion{ a * inverse, b * inverse, c * inverse, d * inverse };
		}

		return Quaternion(0, 0, 0, 1); // return the identity quaternion as fallback
	}

	Quaternion Conjugate() const    // conjugate, inverse rotation
	{
		return Quaternion{ -a, -b, -c, d };
	}

	Quaternion operator-() const     // unary negate, same rotation
	{
		return Quaternion{ -a, -b, -c, -d };
	}

	Quaternion Slerp(Quaternion q1, Quaternion q2, float time)     // slerp between two input quaternions over input time
	{
		// finds dot product between two quaternions
		float dotProduct = q1.a * q2.a + q1.b * q2.b + q1.c * q2.c + q1.d * q2.d;
		
		// get the shortest path
		if (dotProduct < 0.0f) 
		{
			// negate Quaternion 2
			q2.a = -q2.a;
			q2.b = -q2.b;
			q2.c = -q2.c;
			q2.d = -q2.d;
			dotProduct = -dotProduct;
		}

		// set up theta and sin theta
		float theta = acosf(dotProduct);
		float sinTheta = sinf(theta);

		// from formula
		float w1 = sinf(theta * (1 - time)) / sinTheta;
		float w2 = sinf(theta * time) / sinTheta;

		return Quaternion(
			q1.a * w1 + q2.a * w2,
			q1.b * w1 + q2.b * w2,
			q1.c * w1 + q2.c * w2,
			q1.d * w1 + q2.d * w2
		);

	}

	Quaternion quatMul(const Quaternion& q1, const Quaternion& q2)  // multiple two input quaternions together
	{
		return Quaternion(
			q1.d * q2.d - q1.a * q2.a - q1.b * q2.b - q1.c * q2.c,
			q1.d * q2.a + q1.a * q2.d + q1.b * q2.c - q1.c * q2.b,
			q1.d * q2.b - q1.a * q2.c + q1.b * q2.d + q1.c * q2.a,
			q1.d * q2.c + q1.a * q2.b - q1.b * q2.a + q1.c * q2.d
		);
	}

	// Build quaternion from axis-angle
	Quaternion FromAxisAngle(const Vec3& axis, float angleRadians)   // paramters are vec3 axis (0,1,0) for y-axis and angle in degrees
	{
		Vec3 v = axis.normalize();                // normalise the vector so unit quaternion or rotation is invalid
		float halfAngle = angleRadians * 0.5f;    // halve the radians angle
		float s = sin(halfAngle);                 // set up sin (radians angle / 2)
		float w = cos(halfAngle);                 // set up cos (radians angle / 2)
		return Quaternion(            
			v.x * s,          // vector x-component * sin half angle
			v.y * s,          // vector y-component * sin half angle
			v.z * s,          // vector z-component * sin half angle
			w);               // just cos angle
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class Colour
{
public:

	float r, g, b, a;

	// Default Constructor, black and fully transparent
	Colour() : r(0), g(0), b(0), a(0) {}

	// Constructor with float parameters (0.0f - 1.0f)
	Colour(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

	// Constructor from unsigned char (0 - 255)
	Colour(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255)
		: r(_r / 255.0f), g(_g / 255.0f), b(_b / 255.0f), a(_a / 255.0f) {}


	Colour operator+(const Colour& value) const   // adds each colour component by an input value
	{
		return { 
			r + value.r, 
			g + value.g, 
			b + value.b, 
			a + value.a 
		};
	}


	Colour operator-(const Colour& value) const   // subtracts each colour component by an input value
	{
		return { 
			r - value.r, 
			g - value.g, 
			b - value.b, 
			a - value.a 
		};
	}


	Colour operator*(const float value) const     // multiplies each colour component by an input value
	{
		return { 
			r * value, 
			g * value, 
			b * value, 
			a * value 
		};
	}


	Colour operator/(const float value) const    // divides each colour component by an input value
	{
		return { 
			r / value, 
			g / value, 
			b / value, 
			a / value 
		};
	}

	Colour RGBMult(float value) const           // scales RGB using input value without touching alpha
	{ 
		return { 
			r * value, 
			g * value, 
			b * value, 
			a 
		}; 
	}
};

/////////////////////////////////////////////////////////////////////////////////////////


class ShadingFrame
{
public:

	Vec3 N;   // normal, direction perpendicular to surface
	Vec3 T;   // tangent, one axis of surface tangent plane
	Vec3 B;   // bitangent, other axis of surface tangent plane

	// Construct from a normal given a vec3 input
	ShadingFrame(const Vec3& pVec)
	{
		BuildFromNormal(pVec);
	}

	// Default frame (z-up), normal points along z, tangent along x, bitangent along y
	ShadingFrame() : N(0, 0, 1), T(1, 0, 0), B(0, 1, 0) {}


	// Build orthonormal basis (T, B, N)
	void BuildFromNormal(const Vec3& pVec)
	{
		N = pVec.normalize();    // normalise the first vector, ensure N has unit length

		// Pick helper vector not parallel to N, fabs is absolute value of float, 
		// angle in radians
		// which axis is more perpendicular to N? choose either x or y
		Vec3 helper = (fabs(N.x) > cos(45 * 3.14 / 180.0f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f));

		// Gram–Schmidt: T = helper - projection(helper onto N)
		T = helper - N * helper.Dot(N);

		// Normalise T so it is unit length
		T = T.normalize();

		// B = cross product of N and T
		B = N.Cross(T);

		// Normalise B so it is unit length
		B = B.normalize();
	}


	Vec3 LocalToWorld(const Vec3& pVec) const      // tangent space vector to world vector
	{
		return T * pVec.x + B * pVec.y + N * pVec.z;    // scales T and B and N
	}

	Vec3 WorldToLocal(const Vec3& pVec) const
	{
		return Vec3(            // returns a vector in local co-ordinates
			pVec.Dot(T),  
			pVec.Dot(B),
			pVec.Dot(N)
		);
	}
};


/////////////////////////////////////////////////////////////////////////////////////////


class Triangle
{
public:
	Vec4 v0;
	Vec4 v1;
	Vec4 v2;

	Triangle(Vec4 a, Vec4 b, Vec4 c) : v0(a), v1(b), v2(c) {}


	// Cross product of two vectors
	float edgeFunction(const Vec4& v0, const Vec4& v1, const Vec4& p)
	{
		return (((p.x - v0.x) * (v1.y - v0.y)) - ((v1.x - v0.x) * (p.y - v0.y)));
	}

	// Find bounds of box surrounding triangle 
	void findBounds(Vec4& tr, Vec4& bl, GamesEngineeringBase::Window& canvas)
	{
		tr.x = min(max(max(v0.x, v1.x), v2.x), canvas.getWidth() - 1);
		tr.y = min(max(max(v0.y, v1.y), v2.y), canvas.getHeight() - 1);
		bl.x = max(min(min(v0.x, v1.x), v2.x), 0);
		bl.y = max(min(min(v0.y, v1.y), v2.y), 0);
	}

	// Barycentric co-ordinates for inside the triangle
	void barycentricCoordinates(const Vec4& p, float& alpha, float& beta, float& gamma)
	{
		alpha = edgeFunction(v1, v2, p);
		beta = edgeFunction(v2, v0, p);
		gamma = edgeFunction(v0, v1, p);

		float projArea = edgeFunction(v0, v1, v2);
		float area = 1.0f / projArea;

		alpha *= area;
		beta *= area;
		gamma *= area;
	}

};

// Template to nicely colour the triangle
template<typename t>
t simpleInterpolateAttribute(t a0, t a1, t a2, float alpha, float beta, float gamma)
{
	return (a0 * alpha) + (a1 * beta) + (a2 * gamma);
}

/////////////////////////////////////////////////////////////////////////////////////////