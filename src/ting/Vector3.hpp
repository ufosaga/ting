/* The MIT License:

Copyright (c) 2008 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// ting 0.3
// Homepage: http://code.google.com/p/ting
// Author: Ivan Gagis <igagis@gmail.com>

// File description:
//	3d and 2d vector math classes

#ifndef M_Vector3_hpp
#define M_Vector3_hpp

#include <cmath>
#include <cstdlib>

#include "types.hpp"
#include "math.hpp"
#include "Exc.hpp"

namespace ting{

//  -== forward declarations ==-
template <typename T> class Vector2;
template <typename T> class Vector3;
template <typename T> class Matrix4;
template <typename T> class Quaternion;

template <class T> inline const char* ParseNum(const char* str, T& out_Res);

template <> inline const char* ParseNum<int>(const char* str, int& out_Res){
	char buf[32];
	const char *p = str;
	ting::uint i = 0;
	
	//TODO: allow '-' character in front of number
	while(*p >= 0x30 && *p <= 0x39 && i < sizeof(buf)){
		buf[i] = *p;
		++i;
		++p;
		ASSERT(i < sizeof(buf))
	}
	buf[i] = 0;//null terminate
	
	if(i == 0)
		return 0;
	
	out_Res = atoi(buf);
	return p;
};

template <> inline const char* ParseNum<float>(const char* str, float& out_Res){
	char buf[32];
	const char *p = str;
	ting::uint i = 0;
	
	//TODO: allow one '.' character in the number
	while(*p >= 0x30 && *p <= 0x39 && i < sizeof(buf)){
		buf[i] = *p;
		++i;
		++p;
		ASSERT(i < sizeof(buf))
	}
	buf[i] = 0;//null terminate
	
	if(i == 0)
		return 0;
	
	out_Res = atof(buf);
	return p;
};

//===============================
//
//
//      Vector2 class
//
//
//===============================
template <class T> class Vector2{
	friend class Vector3<T>;

	STATIC_ASSERT(sizeof(Vector2) == sizeof(T) * 2)
public:
	T x, y;

	//default constructor
	inline Vector2(){};
	
	Vector2(T vecX, T vecY){
		this->x = vecX;
		this->y = vecY;
	};

	//copy constructr will be generated by compiler

	//create Vector2 from Vector3
	Vector2(const Vector3<T>& vec);

	inline T& operator[](ting::uint i){
		ASSERT(i < 2)
		ASSERT( &((&this->x)[0]) == &this->x)
		ASSERT( &((&this->x)[1]) == &this->y)
		return (&this->x)[i];
	};

	inline const T& operator[](ting::uint i)const{
		ASSERT(i < 2)
		ASSERT( &((&this->x)[0]) == &this->x)
		ASSERT( &((&this->x)[1]) == &this->y)
		return (&this->x)[i];
	};

	inline Vector2& operator=(const Vector2& vec){
		this->x = vec.x;
		this->y = vec.y;
		return (*this);
	};
	
	inline Vector2& operator=(const Vector3<T>& vec);

	inline Vector2 operator+(const Vector3<T>& vec)const;

	inline Vector2& operator+=(const Vector2& vec){
		this->x += vec.x;
		this->y += vec.y;
		return (*this);
	};

	inline Vector2 operator+(const Vector2& vec)const{
		return (Vector2(*this) += vec);
	};

	inline Vector2& operator-=(const Vector2& vec){
		this->x -= vec.x;
		this->y -= vec.y;
		return (*this);
	};

	inline Vector2 operator-(const Vector2& vec)const{
		return (Vector2(*this) -= vec);
	};

	inline Vector2 operator-(const Vector3<T>& vec)const;
	
	//unary minus
	inline Vector2 operator-()const{
		return Vector2(-this->x, -this->y);
	};

	inline Vector2& operator*=(T num){
		this->x *= num;
		this->y *= num;
		return (*this);
	};

	inline Vector2 operator*(T num)const{
		return (Vector2(*this) *= num);
	};

	//operator num * vec
	inline friend Vector2 operator*(T num, const Vector2& vec){
		return vec * num;
	};

	inline Vector2& operator/=(T num){
		ASSERT(num != 0)
		this->x /= num;
		this->y /= num;
		return (*this);
	};

	inline Vector2 operator/(T num)const{
		ASSERT(num != 0)
		return (Vector2(*this) /= num);
	};

	//dot product
	inline T operator*(const Vector2& vec)const{
		return (this->x * vec.x + this->y * vec.y);
	};

	inline bool operator==(const Vector2& vec)const{
		return this->x == vec.x && this->y == vec.y;
	};

	inline bool operator!=(const Vector2& vec)const{
		return !this->operator==(vec);
	};

	inline bool IsZero()const{
		return (this->x == 0 && this->y == 0);
	};

	inline Vector2& Negate(){
		//NOTE: this is faster than // (*this) = -(*this);
		this->x = -this->x;
		this->y = -this->y;
		return (*this);
	};

	inline T MagPow2(){
		return Pow2(this->x) + Pow2(this->y);
	};

	inline T Magnitude(){
		return T( sqrt(this->MagPow2()) );
	};

	inline Vector2& Normalize(){
		ASSERT(this->Magnitude() != 0)
		return (*this) /= this->Magnitude();
	};

	inline Vector2& Scale(T value){
		return (*this) *= value;
	};

	inline Vector2& SetToZero(){
		this->x = 0;
		this->y = 0;
		return (*this);
	};

	//Angle is passed in radians
	Vector2& Rotate(T angle){
		T cosa = T(::cos(angle));
		T sina = T(::sin(angle));
		T tmp = this->x * cosa - this->y * sina;
		this->y = this->y * cosa + this->x * sina;
		this->x = tmp;
		return (*this);
	};

	Vector2 Rotation(T angle)const{
		return Vector2(*this).Rotate(angle);
	};

	//Parse string of form "xxx, yyy" where xxx and yyy are positive decimal numbers
	Vector2& ParseXY(const char* str) throw(Exc){
		ASSERT(str)

		const char *p = str;

		//search the first number
		//TODO: allow first '-' character
		while(*p < 0x30 || *p > 0x39){
			if(*p == 0)
				throw Exc("CResManHGE::ParseXY(): no number found");
			++p;
		}
		
		p = ParseNum<T>(p, this->x);
		if(!p)
			throw Exc("CResManHGE::ParseXY(): input string should start with dight");

		//search next number
		//TODO: allow first '-' character
		while(*p < 0x30 || *p > 0x39){
			if(*p == 0)
				throw Exc("CResManHGE::ParseXY(): second number not found");
			++p;
		}

		p = ParseNum<T>(p, this->y);
		if(!p)
			throw Exc("CResManHGE::ParseXY(): second number parsing failed");

		return *this;
	};
	
#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Vector2<T>& vec){
		s<<"("<<vec.x<<", "<<vec.y<<")";
		return s;
	};
#endif
};//~class


//===============================
//
//
//      Vector3 class
//
//
//===============================
template <class T> class Vector3{
	friend class Vector2<T>;
	friend class Matrix4<T>;

	STATIC_ASSERT(sizeof(Vector3) == sizeof(T) * 3)
public:
	T x, y, z;
	
	inline Vector3(){};//default constructor
	
	Vector3(T vecX, T vecY, T vecZ){
		this->x = vecX;
		this->y = vecY;
		this->z = vecZ;
	};

	//copy constructor will be generated by compiler

	Vector3(T num){
		this->operator=(num);
	};

	Vector3(const Vector2<T>& vec){
		this->x = vec.x;
		this->y = vec.y;
		this->z = 0;
	};

	inline T& operator[](ting::uint i){
		ASSERT(i < 3)
		ASSERT( &((&this->x)[0]) == &this->x)
		ASSERT( &((&this->x)[1]) == &this->y)
		ASSERT( &((&this->x)[2]) == &this->z)
		return (&this->x)[i];
	};

	inline const T& operator[](ting::uint i)const{
		ASSERT(i < 3)
		ASSERT( &((&this->x)[0]) == &this->x)
		ASSERT( &((&this->x)[1]) == &this->y)
		ASSERT( &((&this->x)[2]) == &this->z)
		return (&this->x)[i];
	};

	inline Vector3& operator=(const Vector3& vec){
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
		return *this;
	};

	inline Vector3& operator=(const Vector2<T>& vec);

	inline Vector3& operator=(T num){
		this->x = num;
		this->y = num;
		this->z = num;
		return (*this);
	};

	inline Vector3& operator+=(const Vector2<T>& vec);

	inline Vector3& operator+=(const Vector3& vec){
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		return (*this);
	};

	inline Vector3 operator+(const Vector3& vec)const{
		return (Vector3(*this) += vec);
	};

	inline Vector3& operator-=(const Vector3& vec){
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		return *this;
	};

	inline Vector3 operator-(const Vector3& vec)const{
		return (Vector3(*this) -= vec);
	};

	//unary minus
	inline Vector3 operator-()const{
		return Vector3(*this).Negate();
	};

	inline Vector3& operator*=(T num){
		this->x *= num;
		this->y *= num;
		this->z *= num;
		return (*this);
	};

	inline Vector3 operator*(T num)const{
		return (Vector3(*this) *= num);
	};

	inline friend Vector3 operator*(T num, const Vector3& vec){
		return vec * num;
	};

	inline Vector3& operator/=(T num){
		ASSERT(num != 0)
		this->x /= num;
		this->y /= num;
		this->z /= num;
		return (*this);
	};

	inline Vector3 operator/(T num){
		ASSERT(num != 0)
		return (Vector3(*this) /= num);
	};

	//Dot product
	inline T operator*(const Vector3& vec)const{
		return this->x * vec.x +
					this->y * vec.y +
					this->z * vec.z;
	};

	//Cross product
	inline Vector3 operator%(const Vector3& vec)const{
		return Vector3(this->y * vec.z - this->z * vec.y,
					this->z * vec.x - this->x * vec.z,
					this->x * vec.y - this->y * vec.x
				);
	};

	inline bool IsZero()const{
		return (this->x == 0 && this->y == 0 && this->z == 0);
	};

	inline Vector3& Negate(){
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		return (*this);
	};

	//power 2 of the magnitude
	inline T MagPow2()const{
		return Pow2(this->x) + Pow2(this->y) + Pow2(this->z);
	};

	inline T Magnitude()const{
		return (T)sqrt(this->MagPow2());
	};

	inline Vector3& Normalize(){
		ASSERT(this->Magnitude() != 0)
		(*this) /= this->Magnitude();
		return (*this);
	};
	
	inline Vector3& SetToZero(){
		this->x = 0;
		this->y = 0;
		this->z = 0;
		return (*this);
	};

	inline Vector3& ProjectOnto(const Vector3& vec){
		ASSERT(this->MagPow2() != 0)
		(*this) = vec * (vec * (*this)) / vec.MagPow2();
		return (*this);
	};

	//rotate this vector with unit quaternion which represents a rotation
	inline Vector3<T>& Rotate(const Quaternion<T>& q);//see implemenation below

#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Vector3<T>& vec){
		s<<"("<<vec.x<<", "<<vec.y<<", "<<vec.z<<")";
		return s;
	};
#endif
};


//===============================
//
//
//      Matrix4 class
//
//
//===============================
template <typename T> class Matrix4{
	//OpenGL compatible matrix elements array in case T = float/double
	T m[ 4 * 4 ]; //matrix components 0-3 1st column, 4-7 2nd column, 8-11 3rd column, 12-15 4th column
  public:
	inline Matrix4(){};//Default constructor.

	//copy constructor must be trivial.
	//Let's allow compiler to make it for us.
	//Matrix4(const Matrix4& matr){};

	//~Matrix4(){};//destructor

	inline T* operator[](u32 i){
		ASSERT(i < 4)
		return &m[i*4];
	};

	//Multiply by Vector3 (M * V). i.e. transform vector with transformation matrix
	Vector3<T> operator*(const Vector3<T>& vec){
		return Vector3<T>(
					this->m[0] * vec[0] + this->m[4] * vec[1] + this->m[8] * vec[2] + this->m[12],
					this->m[1] * vec[0] + this->m[5] * vec[1] + this->m[9] * vec[2] + this->m[13],
					this->m[2] * vec[0] + this->m[6] * vec[1] + this->m[10] * vec[2] + this->m[14]
				);
	};

	inline Matrix4& operator=(const Matrix4& matr){
		memcpy(this->m, matr.m, sizeof(this->m));
		return (*this);
	};

	//Transpose matrix
	Matrix4& Transpose(){
		Exchange(this->m[1], this->m[4]);
		Exchange(this->m[2], this->m[8]);
		Exchange(this->m[6], this->m[9]);
		Exchange(this->m[3], this->m[12]);
		Exchange(this->m[7], this->m[13]);
		Exchange(this->m[11], this->m[14]);
		return (*this);
	};

	//Multipply by matrix from the right m  = m * M
	Matrix4& RightMultMatrix(const Matrix4 &M){
		//TODO: rewrite to use Matrix4 instead of T tmpM[16]
		T tmpM[16];
		for(ting::uint i = 0; i < 4; ++i){
			tmpM[4*i]  =m[0]*M.m[4*i]+m[4]*M.m[4*i+1]+m[8]*M.m[4*i+2]+ m[12]*M.m[4*i+3];
			tmpM[4*i+1]=m[1]*M.m[4*i]+m[5]*M.m[4*i+1]+m[9]*M.m[4*i+2]+ m[13]*M.m[4*i+3];
			tmpM[4*i+2]=m[2]*M.m[4*i]+m[6]*M.m[4*i+1]+m[10]*M.m[4*i+2]+m[14]*M.m[4*i+3];
			tmpM[4*i+3]=m[3]*M.m[4*i]+m[7]*M.m[4*i+1]+m[11]*M.m[4*i+2]+m[15]*M.m[4*i+3];
		}
		memcpy(this->m, tmpM, sizeof(this->m));
		//*this=tmp;
		return (*this);
	};

	//Multiply by matrix from the left m = M * m
	Matrix4& LeftMultMatrix(const Matrix4& M){
		//TODO: rewrite to use Matrix4 instead of T tmpM[16]
		T tmpM[16];
		for(ting::uint i = 0; i < 4; ++i){
			tmpM[4*i]  =m[4*i]*M.m[0]+m[4*i+1]*M.m[4]+m[4*i+2]*M.m[8]+ m[4*i+3]*M.m[12];
			tmpM[4*i+1]=m[4*i]*M.m[1]+m[4*i+1]*M.m[5]+m[4*i+2]*M.m[9]+ m[4*i+3]*M.m[13];
			tmpM[4*i+2]=m[4*i]*M.m[2]+m[4*i+1]*M.m[6]+m[4*i+2]*M.m[10]+m[4*i+3]*M.m[14];
			tmpM[4*i+3]=m[4*i]*M.m[3]+m[4*i+1]*M.m[7]+m[4*i+2]*M.m[11]+m[4*i+3]*M.m[15];
		}
		memcpy(this->m, tmpM, sizeof(this->m));
		return (*this);
	};

	Matrix4& Identity(){
		this->m[0] = 1;    this->m[4] = 0;    this->m[8] = 0;    this->m[12] = 0;
		this->m[1] = 0;    this->m[5] = 1;    this->m[9] = 0;    this->m[13] = 0;
		this->m[2] = 0;    this->m[6] = 0;    this->m[10] = 1;   this->m[14] = 0;
		this->m[3] = 0;    this->m[7] = 0;    this->m[11] = 0;   this->m[15] = 1;
		return (*this);
	};

	//multiplies this matrix by Scale matrix from the left (M = S * M)
	Matrix4& Scale(const Vector3<T>& scale){
		//TODO:rewrite it using the *= operator
		
		//calculate first column
		this->m[0] = this->m[0] * scale[0];
		this->m[1] = this->m[1] * scale[1];
		this->m[2] = this->m[2] * scale[2];
		
		//calculate second column
		this->m[4] = this->m[4] * scale[0];
		this->m[5] = this->m[5] * scale[1];
		this->m[6] = this->m[6] * scale[2];
		
		//calculate third column
		this->m[8] = this->m[8] * scale[0];
		this->m[9] = this->m[9] * scale[1];
		this->m[10] = this->m[10] * scale[2];
		
		//calculate fourth column
		this->m[12] = this->m[12] * scale[0];
		this->m[13] = this->m[13] * scale[1];
		this->m[14] = this->m[14] * scale[2];
		
		//NOTE: 4th row remains unchanged
		return (*this);
	};

	//multiplies this matrix by Translation matrix from the left (M = T * M)
	Matrix4& Translate(const Vector3<T>& t){
		//calculate first column
		this->m[0] = this->m[0] + this->m[3] * t[0];
		this->m[1] = this->m[1] + this->m[3] * t[1];
		this->m[2] = this->m[2] + this->m[3] * t[2];

		//calculate second column
		this->m[4] = this->m[4] + this->m[7] * t[0];
		this->m[5] = this->m[5] + this->m[7] * t[1];
		this->m[6] = this->m[6] + this->m[7] * t[2];

		//calculate third column
		this->m[8] = this->m[8] + this->m[11] * t[0];
		this->m[9] = this->m[9] + this->m[11] * t[1];
		this->m[10] = this->m[10] + this->m[11] * t[2];

		//calculate fourth column
		this->m[12] = this->m[12] + this->m[15] * t[0];
		this->m[13] = this->m[13] + this->m[15] * t[1];
		this->m[14] = this->m[14] + this->m[15] * t[2];

		//note: 4th row remain unchanged
		return (*this);
	};

	//multiplies this matrix by Rotation matrix from the left, rotation given by quaternion.
	inline Matrix4& Rotate(const Quaternion<T>& q);//implementation see below
};


//===============================
//
//
//      Quaternion class
//
//
//===============================
template <typename T> class Quaternion{
public:
	T x,y,z,w; //Quaternion components
	
	// This is our constructor that allows us to initialize our data upon creating an instance
	Quaternion(T qx, T qy, T qz, T qw) :
			x(qx),
			y(qy),
			z(qz),
			w(qw)
	{};

	//this constructor creates unit quaternion of a rotation around the axis by |axis| radians
	Quaternion(const Vector3<T>& axis){
		T mag = axis.Magnitude();//magnitude is a rotation angle
		if(mag != 0){
			Vector3<T> a = axis;
			a /= mag;//normalize axis
			this->InitRot( a.x, a.y, a.Z(), mag);
		}else
			this->Identity();
	};

	// A default constructor
	inline Quaternion(){};

	//"complex conjugate of" operator
	inline Quaternion operator!()const{
		return Quaternion(-this->x, -this->y, -this->z, this->w);
	};

	inline Quaternion operator+(const Quaternion& q)const{
		return Quaternion(this->x + q.x, this->y + q.y, this->z + q.z, this->w + q.w);
	};

	inline Quaternion& operator+=(const Quaternion& q){
		this->x += q.x;
		this->y += q.y;
		this->z += q.z;
		this->w += q.w;
		return (*this);
	};

	inline Quaternion& operator=(const Quaternion& q){
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
		return (*this);
	};

	inline Quaternion& operator*=(T s){
		this->x *= s;
		this->y *= s;
		this->z *= s;
		this->w *= s;
		return (*this);
	};

	inline Quaternion operator*(T s)const{
		return (Quaternion(*this) *= s);
	};

	inline Quaternion& operator/=(T s){
		this->x /= s;
		this->y /= s;
		this->z /= s;
		this->w /= s;
		return (*this);
	};

	inline Quaternion operator/(T s)const{
		return (Quaternion(*this) /= s);
	};

	//dot product of quaternions
	inline T operator*(const Quaternion& q)const{
		return this->x * q.x + this->y * q.y + this->z * q.z + this->w * q.w;
	};

	//multiplication of quaternions
	Quaternion& operator%=(const Quaternion& q){
		T a = (this->w + this->x) * (q.w + q.x);
		T b = (this->z - this->y) * (q.y - q.z);
		T c = (this->x - this->w) * (q.y + q.z);
		T d = (this->y + this->z) * (q.x - q.w);
		T e = (this->x + this->z) * (q.x + q.y);
		T f = (this->x - this->z) * (q.x - q.y);
		T g = (this->w + this->y) * (q.w - q.z);
		T h = (this->w - this->y) * (q.w + q.z);

		this->x = a - (e + f + g + h) * 0.5f;
		this->y = -c + (e - f + g - h) * 0.5f;
		this->z = -d + (e - f - g + h) * 0.5f;
		this->w = b + (-e - f + g + h) * 0.5f;
		return (*this);
	};

	//multiplication of quaternions
	inline Quaternion operator%(const Quaternion& q)const{
		return (Quaternion(*this) %= q);
	};

	inline Quaternion& Identity(){
		this->x = T(0);
		this->y = T(0);
		this->z = T(0);
		this->w = T(1);
		return *this;
	};

	//Complex conjugate
	inline Quaternion& Conjugate(){
		*this = !(*this);
		return *this;
	};

	inline Quaternion& Negate(){
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		this->w = -this->w;
		return *this;
	};

	//returns the magnitude^2 of this quaternion
	inline T MagPow2()const{
		return (*this) * (*this);
	};

	inline T Magnitude()const{
		return T( sqrt(this->MagPow2()) );
	};

	inline Quaternion& Normalize(){
		return (*this) /= Magnitude();
	};

	//Initialize this with rotation unit quaternion from axis (normalized) and an angle
	inline void InitRot(T xx, T yy, T zz, T angle){
		T sina2 = T( sin(angle / 2) );
		this->w = T( cos(angle / 2) );
		this->x = xx * sina2;
		this->y = yy * sina2;
		this->z = zz * sina2;
	};

	//multiply this quaternion by unit rotation quaternion
	//from the left
	Quaternion& Rotate(Vector3<T> axis, T angle){
		Quaternion r;
		r.InitRot(axis.x, axis.y, axis.Z(), angle);
		return (*this) = r % (*this);
	};

	//create 4x4 OpenGL like rotation matrix from this quaternion
	//ARGS: m - matrix to fill
	//RETURNS: return a reference to m
	//TODO: move this functionality to  Matrix4::InitRot(const Quaternion& q)
	Matrix4<T>& CreateMatrix4(Matrix4<T>& m)const{
		// After about 300 trees murdered and 20 packs of chalk depleted, the
		// mathematicians came up with these equations for a quaternion to matrix converion:
		//   /  1-(2y^2+2z^2)   2xy+2zw         2xz-2yw         0   \T
		// M=|  2xy-2zw         1-(2x^2+2z^2)   2zy+2xw         0   |
		//   |  2xz+2yw         2yz-2xw         1-(2x^2+2y^2)   0   |
		//   \  0               0               0               1   /

		//First column
		m.m[0] = T(1) - T(2) * ( Pow2(this->y) + Pow2(this->z) );
		m.m[1] = T(2) * (this->x * this->y + this->z * this->w);
		m.m[2] = T(2) * (this->x * this->z - this->y * this->w);
		m.m[3] = T(0);

		//Second column
		m.m[4] = T(2) * (this->x * this->y - this->z * this->w);
		m.m[5] = T(1) - T(2) * ( Pow2(this->x) + Pow2(this->z) );
		m.m[6] = T(2) * (this->z * this->y + this->x * this->w);
		m.m[7] = T(0);

		//Third column
		m.m[8] = T(2) * (this->x * this->z + this->y * this->w);
		m.m[9] = T(2) * (this->y * this->z - this->x * this->w);
		m.m[10] = T(1) - T(2) * ( Pow2(this->x) + Pow2(this->y) );
		m.m[11] = T(0);

		//Fourth column
		m.m[12] = T(0);
		m.m[13] = T(0);
		m.m[14] = T(0);
		m.m[15] = T(1);
		return m;
	}

	//--||--||--
	Matrix4<T> ToMatrix4()const{
		Matrix4<T> m;
		this->CreateMatrix4(m);
		return m;
	};

	//TODO: make ToMatrix4 a template
	Matrix4<float> ToMatrix4f()const{
		Matrix4<float> m;
		Quaternion<float> qq((float)this->x,(float)this->y,(float)this->z,(float)this->w);
		qq.CreateMatrix4(m);
		return m;
	};

	//Spherical linear interpolation.
	//This quaternion = SLERP(q1,q2,t), t from [0;1].
	//SLERP(q1,q2,t) = q1*sin((1-t)*alpha)/sin(alpha)+q2*sin(t*alpha)/sin(alpha),
	//where cos(alpha) = (q1,q2) (dot product of normalized quaternions q1 and q2).
	//It is assumed that quaternions are normalized!
	void Slerp(const Quaternion& q1, const Quaternion& q2, T t){
		//Since quaternions are normalized the cosine of the angle alpha
		//between quaternions are equal to their dot product.
		T cosalpha = q1 * q2;

		//If the dot product is less than 0, the angle alpha between quaternions
		//is greater than 90 degrees. Then we negate second quaternion to make alpha
		//be less than 90 degrees. It is possible since normalized quaternions
		//q and -q represent the same rotation!
		if(cosalpha < T(0)){
			//Negate the second quaternion and the result of the dot product (i.e. cos(alpha))
			q2.Negate();
			cosalpha = -cosalpha;
		}

		//interpolation done by the following general formula:
		//RESULT=q1*sc1(t)+q2*sc2(t).
		//Where sc1,sc2 called interpolation scales.
		T sc1, sc2;//Define variables for scales for interpolation

		//Check if the angle alpha between the 2 quaternions is big enough
		//to make SLERP. If alpha is small then we do a simple linear
		//interpolation between quaternions instead of SLERP!
		//It is also used to avoid divide by zero since sin(0)=0 !
		//We made threshold for cos(alpha)>0.9f (if cos(alpha)==1 then alpha=0).
		if(cosalpha > T(0.9f)){
			//Get the angle alpha between the 2 quaternions, and then store the sin(alpha)
			T alpha = T(acos(cosalpha));
			T sinalpha = T(sin(alpha));

			//Calculate the scales for q1 and q2, according to the angle and it's sine value
			sc1 = T( sin((1 - t) * alpha) / sinalpha );
			sc2 = T( sin(t * alpha) / sinalpha );
		}else{
			sc1 = (1 - t);
			sc2 = t;
		}

		// Calculate the x, y, z and w values for the interpolated quaternion.
		(*this) = q1 * sc1 + q2 * sc2;
	};

#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Quaternion<T>& quat){
		s<<"("<<quat.x<<", "<<quat.y<<", "<<quat.z<<", "<<quat.w<<")";
		return s;
	};
#endif  
};

//template <> class Vector2<uint>{
//	uint v[2]; //Vector components
//  public:
//	inline Vector2(){};//default constructor
//
//	Vector2(uint x, uint y){
//		this->v[0] = x;
//		this->v[1] = y;
//	};
//
//	inline uint& X(){
//		return this->v[0];
//	};
//
//	inline const uint& X()const{
//		return this->v[0];
//	};
//
//	inline uint& Y(){
//		return this->v[1];
//	};
//
//	inline const uint& Y()const{
//		return this->v[1];
//	};
//
//	inline uint& operator[](uint i){
//		ASSERT(i < 2)
//		return this->v[i];
//	};
//
//	inline const uint& operator[](uint i)const{
//		ASSERT(i < 2)
//		return this->v[i];
//	};
//
//	inline bool operator==(const Vector2& vec)const{
//		return this->v[0] == vec.v[0] && this->v[1] == vec.v[1];
//	};
//
//	inline Vector2& operator=(const Vector2<uint>& vec){
//		this->v[0] = vec.v[0];
//		this->v[1] = vec.v[1];
//		return (*this);
//	};
//
////    inline Vector2& operator=(const Vector3<T>& vec);
////
////    inline Vector2 operator+(const Vector2& vec)const{return Vector2( vec.v[0]+v[0], vec.v[1]+v[1] );};
////    inline Vector2 operator+(const Vector3<T>& vec)const;
////    inline Vector2& operator+=(const Vector2& vec){v[0]+=vec.v[0];v[1]+=vec.v[1];return (*this);};
////    inline Vector2 operator-(const Vector2& vec){return Vector2( v[0]-vec.v[0], v[1]-vec.v[1] );};
////    inline Vector2 operator-(const Vector3<T>& vec)const;
////    inline Vector2& operator-=(const Vector2& vec){v[0]-=vec.v[0];v[1]-=vec.v[1];return (*this);};
////
////    inline Vector2 operator-()const{return Vector2(-v[0],-v[1]);};//unary minus
////
////    inline Vector2 operator*(T num)const{return Vector2(v[0]*num, v[1]*num);}
////    inline Vector2& operator*=(T num){v[0]*=num; v[1]*=num; return (*this);};
////    inline Vector2 operator/(T num)const{return Vector2(v[0]/num, v[1]/num);};
////    inline Vector2& operator/=(T num){v[0]/=num; v[1]/=num; return (*this);};
////
////    inline friend Vector2 operator*(T num, const Vector2& vec){return Vector2(vec.v[0]*num, vec.v[1]*num);};
////
////    //dot product
////    inline T operator*(const Vector2& vec)const{return (v[0]*vec.v[0]+v[1]*vec.v[1]);};
////
//    inline bool IsZero()const{
//		return (this->v[0] == 0 && this->v[1] == 0);
//	};
////    inline T MagPow2(){return (v[0]*v[0]+v[1]*v[1]);};
////    inline T Magnitude(){return T( sqrt(MagPow2()) );};
////    inline Vector2& Normalize(){(*this)/=Magnitude();return (*this);};
////    inline Vector2& Scale(T value){(*this)*=value; return (*this);};
//
//    inline Vector2& SetToZero(){
//		this->v[0] = 0;
//		this->v[1] = 0;
//		return (*this);
//	};
//
//#ifdef DEBUG
//	friend std::ostream& operator<<(std::ostream& s, const Vector2<uint>& vec){
//		s<<"("<<vec.X()<<", "<<vec.Y()<<")";
//		return s;
//	};
//#endif
//};//~class

//===============================================
//
//       inline functions implementation
//
//===============================================

template <class T> inline Vector2<T>::Vector2(const Vector3<T>& vec){
	this->operator=(vec);
};

template <class T> inline Vector2<T>& Vector2<T>::operator=(const Vector3<T>& vec){
	this->x = vec.x;
	this->y = vec.y;
	return (*this);
};

template <class T> inline Vector3<T>& Vector3<T>::operator=(const Vector2<T>& vec){
	this->x = vec.x;
	this->y = vec.y;
	return (*this);
};

template <class T> inline Vector3<T>& Vector3<T>::operator+=(const Vector2<T>& vec){
	this->x += vec.x;
	this->y += vec.y;
	return (*this);
};

template <class T> inline Vector2<T> Vector2<T>::operator+(const Vector3<T>& vec)const{
	return Vector2<T>(
				this->x + vec.x,
				this->y + vec.y
			);
};

template <class T> inline Vector2<T> Vector2<T>::operator-(const Vector3<T>& vec)const{
	return Vector2<T>(
				this->x - vec.x,
				this->y - vec.y
			);
};

template <class T> inline Matrix4<T>& Matrix4<T>::Rotate(const Quaternion<T>& q){
	Matrix4<T> rm;
	q.CreateMatrix4(rm);
	this->LeftMultMatrix(rm);
	return (*this);
};

template <class T> inline Vector3<T>& Vector3<T>::Rotate(const Quaternion<T>& q){
	*this = q.ToMatrix4() * (*this);
	return *this;
};


template <class T> class Rectangle2{
	Vector2<T> lb; //Left-Bottom
	Vector2<T> rt; //Right-Top
public:
	
	inline Rectangle2(){};
	
	Rectangle2(T left, T top, T right, T bottom) :
			lb(left, bottom),
			rt(right, top)
	{};

	Rectangle2(Vector2<T> leftBottom, Vector2<T> rightTop) :
			lb(leftBottom),
			rt(rightTop)
	{};
	
	inline void Set(T left, T top, T right, T bottom){
		this->lb = Vector2<T>(left, bottom);
		this->rt = Vector2<T>(right, top);
	};
	
	inline Vector2<T> Center()const{
		return (this->lb + this->rt) / 2;
	};

	inline void SetCenter(const Vector2<T>& vec){
		Vector2<T> offset = vec - Center();
		this->operator +=(offset);
	};

	bool IsIn(const Vector2<T>& vec)const{
		if(this->Left() <= this->Right()){
			if(this->Bottom() <= this->Top()){
				return vec.x <= this->Right() &&
							vec.x >= this->Left() &&
							vec.y >= this->Bottom() &&
							vec.y <= this->Top();
			}else{
				return vec.x <= this->Right() &&
							vec.x >= this->Left() &&
							vec.y <= this->Bottom() &&
							vec.y >= this->Top();
			}
		}else{
			if(this->Bottom() <= this->Top()){
				return vec.x >= this->Right() &&
							vec.x <= this->Left() &&
							vec.y >= this->Bottom() &&
							vec.y <= this->Top();
			}else{
				return vec.x >= this->Right() &&
							vec.x <= this->Left() &&
							vec.y <= this->Bottom() &&
							vec.y >= this->Top();
			}
		}
	};
	
	inline Vector2<T> Extent()const{
		return this->Size()/2;
	};

	inline Vector2<T> Size()const{
		return this->rt - this->lb;
	};
	
	inline Rectangle2& operator=(const Rectangle2<T>&  rect){
		this->rt = rect.rt;
		this->lb = rect.lb;
		return *this;
	};

	inline Rectangle2& operator+=(const Vector2<T>& vec){
		this->rt += vec;
		this->lb += vec;
		return (*this);
	};

	inline Rectangle2 operator*(T num){
		return Rectangle(this->lb * num, this->rt * num);
	};

	inline T& LeftBottom(){
		//TODO: return min out of this->lb.x and this->rt.x
		return this->lb;
	};

	inline const T& LeftBottom()const{
		//TODO: return min out of this->lb.x and this->rt.x
		return this->lb;
	};

	inline T& RightTop(){
		//TODO: return min out of this->lb.x and this->rt.x
		return this->rt;
	};

	inline const T& RightTop()const{
		//TODO: return min out of this->lb.x and this->rt.x
		return this->rt;
	};

	inline T& Left(){
		//TODO: return min out of this->lb.x and this->rt.x
		return this->lb.x;
	};

	inline const T& Left()const{
		return this->lb.x;
	};

	inline T& Top(){
		return this->rt.y;
	};

	inline const T& Top()const{
		return this->rt.y;
	};

	inline T& Right(){
		return this->rt.x;
	};

	inline const T& Right()const{
		return this->rt.x;
	};

	inline T& Bottom(){
		return this->lb.y;
	};

	inline const T& Bottom()const{
		return this->lb.y;
	};
	
	inline T Width()const{
		return this->Right() - this->Left();
	};
	
	inline T Height()const{
		return this->Top() - this->Bottom();
	};
};

//
//
// Some convenient typedefs
//
//

typedef Vector2<int> Vec2i;
typedef Vector2<ting::uint> Vec2ui;
typedef Vector2<float> Vec2f;
typedef Vector2<double> Vec2d;

typedef Vector3<float> Vec3f;
typedef Vector3<double> Vec3d;

typedef Matrix4<float> Matr4f;
typedef Matrix4<double> Matr4d;

typedef Quaternion<float> Quatf;
typedef Quaternion<double> Quatd;

typedef Rectangle2<float> Rect2f;
typedef Rectangle2<double> Rect2d;
typedef Rectangle2<int> Rect2i;

}//~namespace
#endif//~once