#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


template <typename T>
struct vector2
{
	T x, y;

	//! trivial ctor
	vector2<T>() {}

	//! setting ctor
	vector2<T>(const T x0, const T y0): x(x0), y(y0) {}

	//! array indexing
	T &operator [](unsigned int i)
	{   return *(&x+i);   }

	//! array indexing
	const T &operator [](unsigned int i) const
	{	return *(&x+i);   }

	//! function call operator
	void operator ()(const T x0, const T y0)
	{	x= x0; y= y0;	}

	//! test for equality
	bool operator==(const vector2<T> &v)
	{	return (x==v.x && y==v.y);	}

	//! test for inequality
	bool operator!=(const vector2<T> &v)
	{	return (x!=v.x || y!=v.y);	}

	//! set to value
	const vector2<T> &operator =(const vector2<T> &v)
	{
		x= v.x; y= v.y;
		return *this;
	}

	//! negation
	const vector2<T> operator -(void) const
	{	return vector2<T>(-x, -y);	}

	//! addition
	const vector2<T> operator +(const vector2<T> &v) const
	{	return vector2<T>(x+v.x, y+v.y);	}

	//! subtraction
	const vector2<T> operator -(const vector2<T> &v) const
	{   return vector2<T>(x-v.x, y-v.y);	}

	//! uniform scaling
	const vector2<T> operator *(const T num) const
	{
		vector2<T> temp(*this);
		return temp*=num;
	}

	//! uniform scaling
	const vector2<T> operator /(const T num) const
	{
		vector2<T> temp(*this);
		return temp/=num;
	}

	//! addition
	const vector2<T> &operator +=(const vector2<T> &v)
	{
		x+=v.x;	y+=v.y;
		return *this;
	}

	//! subtraction
	const vector2<T> &operator -=(const vector2<T> &v)
	{
		x-=v.x;	y-=v.y;
		return *this;
	}

	//! uniform scaling
	const vector2<T> &operator *=(const T num)
	{
		x*=num;	y*=num;
		return *this;
	}

	//! uniform scaling
	const vector2<T> &operator /=(const T num)
	{
		x/=num;	y/=num;
		return *this;
	}

	//! dot product
	T operator *(const vector2<T> &v) const
	{	return x*v.x + y*v.y;	}
};


// macro to make creating the ctors for derived vectors easier
#define VECTOR2_CTORS(name, type)   \
	/* trivial ctor */				\
	name() {}						\
	/* down casting ctor */			\
	name(const vector2<type> &v): vector2<type>(v.x, v.y) {}	\
	/* make x,y combination into a vector */					\
	name(type x0, type y0): vector2<type>(x0, y0) {}



struct vector2i: public vector2<int>
{
	VECTOR2_CTORS(vector2i, int)
};


struct vector2ui: public vector2<unsigned int>
{
	VECTOR2_CTORS(vector2ui, unsigned int)
};


struct vector2f: public vector2<float>
{
	VECTOR2_CTORS(vector2f, float)

	//! gets the length of this vector squared
	float length_squared() const
	{	return (float)(*this * *this);   }

	//! gets the length of this vector
	float length() const
	{	return (float)sqrt(*this * *this);   }

	//! normalizes this vector
	void normalize()
	{	*this/=length();	}

	//! returns the normalized vector
	vector2f normalized() const
	{   return  *this/length();  }

	//! reflects this vector about n
	void reflect(const vector2f &n)
	{
		vector2f orig(*this);
		project(n);
		*this= *this*2 - orig;
	}

	int dir() const
	{
	    if (abs(x) > abs(y)) {
	        if (x > 0) return 1;
	        if (x < 0) return 3;
	    } else
	    if (abs(y) > abs(x)) {
	        if (y > 0) return 2;
	        if (y < 0) return 0;
	    }
	    return -1;
    }


	//! projects this vector onto v
	void project(const vector2f &v)
	{	*this= v * (*this * v)/(v*v);	}

	//! returns this vector projected onto v
	vector2f projected(const vector2f &v)
	{   return v * (*this * v)/(v*v);	}

	//! computes the angle between 2 arbitrary vectors
	static inline float angle(const vector2f &v1, const vector2f &v2)
	{   return acosf((v1*v2) / (v1.length()*v2.length()));  }

	//! computes the angle between 2 normalized arbitrary vectors
	static inline float angle_normalized(const vector2f &v1, const vector2f &v2)
	{   return acosf(v1*v2);  }
};
