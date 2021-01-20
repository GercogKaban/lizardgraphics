#pragma once
//#include "../LizardScript/LizardScript/metagen_def.h"
//#define VECTOR_PARTIAL_ACCESS 1
//#pragma pack(push, 1)

template <typename T>
struct vect2_data
{
	T x, y;

	T& operator[](size_t index)
	{
		return ((T*)this)[index];
	}

	T& operator[](size_t index) const
	{
		return ((T*)this)[index];
	}

	template <typename X>
	static vect2_data from(const X& value)
	{
		vect2_data data;
		data.x = value.x;
		data.y = value.y;
		return data;
	}

	template <typename X>
	X to()
	{
		X x;
		x.x = x;
		x.y = y;
	}
};

template <typename T>
struct vect3_data
{
	T x, y, z;

	T& operator[](size_t index)
	{
		return ((T*)this)[index];
	}

	T& operator[](size_t index) const
	{
		return ((T*)this)[index];
	}

	template <typename X>
	static vect3_data from(const X& value)
	{
		vect3_data data;
		data.x = value.x;
		data.y = value.y;
		data.z = value.z;
		return data;
	}

	template <typename X>
	X to()
	{
		X x;
		x.x = x;
		x.y = y;
		x.z = z;
	}
};

template <typename T>
struct vect4_data
{
	T x, y, z, w;

	T& operator[](size_t index)
	{
		return ((T*)this)[index];
	}

	T& operator[](size_t index) const
	{
		return ((T*)this)[index];
	}

	template <typename X>
	static vect4_data from(const X& value)
	{
		vect4_data data;
		data.x = value.x;
		data.y = value.y;
		data.z = value.z;
		data.w = value.w;
		return data;
	}

	template <typename X>
	X to()
	{
		X x;
		x.x = x;
		x.y = y;
		x.z = z;
		x.w = w;
	}
};

template <typename T, size_t size>
struct vect_custom_data
{
	T containers[size];
};

struct color_data
{
	unsigned char a, r, g, b;
};

template <typename T, size_t X, size_t Y>
struct partial_elements_data_2
{
private:
	size_t getIndex(size_t index) const
	{
		return index ? Y : X;
	}

public:
	T& operator[](size_t index)
	{
		return ((T*)this)[getIndex(index)];
	}

	const T& operator[](size_t index) const
	{
		return ((const T*)this)[getIndex(index)];
	}
};

template <typename T, size_t X, size_t Y, size_t Z>
struct partial_elements_data_3
{
private:
	size_t getIndex(size_t index) const
	{
		return index == 2 ? Z : index == 1 ? Y : X;
	}

public:
	T& operator[](size_t index)
	{
		return ((T*)this)[getIndex(index)];
	}

	const T& operator[](size_t index) const
	{
		return ((const T*)this)[getIndex(index)];
	}
};

template <typename T, size_t X, size_t Y, size_t Z, size_t W>
struct partial_elements_data_4
{
private:
	size_t getIndex(size_t index) const
	{
		return index == 3 ? W : index == 2 ? Z : index == 1 ? Y : X;
	}

public:
	T& operator[](size_t index)
	{
		return ((T*)this)[getIndex(index)];
	}

	const T& operator[](size_t index) const
	{
		return ((const T*)this)[getIndex(index)];
	}
};

template <typename T, size_t csize, typename DATA>
struct vect_prototype2 : public DATA
{
    /*NOT_METADATA*/ //using val_type = T;

	//static const size_t csize;

	size_t size() const
	{
		return size;
	}

    public:

	//================================================================================================
	//	CONSTRUCTORS
	//================================================================================================

	vect_prototype2()
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] = 0;
	}

	vect_prototype2(const T& each)
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] = each;
	}

	vect_prototype2(const T& a, const T& b)
	{
		(*this)[0] = a;
		(*this)[1] = b;
	}

	vect_prototype2(const T& a, const T& b, const T& c)
	{
		(*this)[0] = a;
		(*this)[1] = b;
		(*this)[2] = c;
	}

	vect_prototype2(const T& a, const T& b, const T& c, const T& d)
	{
		(*this)[0] = a;
		(*this)[1] = b;
		(*this)[2] = c;
		(*this)[3] = d;
	}

	template <typename X>
	static vect_prototype2 from(const X& object)
	{
		vect_prototype2 r;
		*(DATA*)&r = DATA::from<X>(object);
		return r;
	}

	//================================================================================================
	//	PARTIAL ACCESS
	//================================================================================================

	//vect_prototype2<T, csize, two_elements_data<T, 0, 1>>& xy() { return *reinterpret_cast<two_elements_data<T, 0, 1>*>(this); }


#if VECTOR_PARTIAL_ACCESS
#include "vectors_ext.inl"
#endif

	//================================================================================================
	//	VECTOR & SCALAR
	//================================================================================================

	vect_prototype2 operator+(T v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] + v;
		return ret;
	}

	vect_prototype2 operator-(T v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] - v;
		return ret;
	}

	vect_prototype2 operator*(T val) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] * val;
		return ret;
	}

	friend vect_prototype2 operator*(T val, const vect_prototype2& v)
	{
		return v * val;
	}

	vect_prototype2 operator/(T val) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] / val;
		return ret;
	}


	//================================================================================================
	//	arithmetics
	//================================================================================================

	vect_prototype2 operator*(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] * v[i];
		return ret;
	}

	vect_prototype2 operator/(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] / v[i];
		return ret;
	}

	vect_prototype2 operator+() const
	{
		return *this;
	}

	vect_prototype2 operator-() const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = -(*this)[i];
		return ret;
	}

	vect_prototype2 operator+(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] + v[i];
		return ret;
	}

	vect_prototype2 operator-(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] - v[i];
		return ret;
	}

	//================================================================================================
	//	comparators (condition must be true for each element index)
	//================================================================================================

	bool operator>(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] <= v[i])
				return false;
		return true;
	}

	bool operator<(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] >= v[i])
				return false;
		return true;
	}

	bool operator>=(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] < v[i])
				return false;
		return true;
	}

	bool operator<=(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] > v[i])
				return false;
		return true;
	}

	bool operator==(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] != v[i])
				return false;
		return true;
	}

	bool operator!=(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] != v[i])
				return true;
		return false;
	}

	//================================================================================================
	//	arithmetics & assignment
	//================================================================================================

	vect_prototype2& operator+=(const vect_prototype2& v)
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] += v[i];
		return *this;
	}

	vect_prototype2& operator-=(const vect_prototype2& v)
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] -= v[i];
		return *this;
	}

	vect_prototype2 operator*=(float val)
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] *= val;
		return *this;
	}

	vect_prototype2 operator/=(float val)
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] /= val;
		return *this;
	}

	//================================================================================================
	//	converters
	//================================================================================================

	//operator bool()
	//{
	//	return *this != 0.0f;
	//}

	template <size_t TOS>
	operator vect_prototype2<T, TOS, DATA>&()
	{
		return *reinterpret_cast<vect_prototype2<T, TOS, DATA>*>(&ret);
	}

	template <typename TO, size_t TOS, typename TODATA>
	operator vect_prototype2<TO, TOS, TODATA>() const
	{
		size_t maxSize = csize;
		if (TOS < maxSize)
			maxSize = TOS;

		vect_prototype2<TO, TOS, TODATA> ret;
		for (size_t i = 0; i < maxSize; i++)
			ret[i] = (TO)((*this)[i]);
		return ret;
	}

	//================================================================================================
	//	iterators
	//================================================================================================

	T* begin() { return &(*this)[0]; }
	T* end() { return &(*this)[csize]; }
	const T* begin() const { return &(*this)[0]; }
	const T* end() const { return &(*this)[csize]; }

	//================================================================================================
	//	misc
	//================================================================================================

	T sum() const
	{
		T s = T();
		for (size_t i = 0; i < csize; i++)
			s += (*this)[i];
		return s;
	}

	T product() const
	{
		T s = T(1);
		for (size_t i = 0; i < csize; i++)
			s *= (*this)[i];
		return s;
	}

	T scalar_product(const vect_prototype2& other) const
	{
		T s = T(0);
		for (size_t i = 0; i < csize; i++)
			s += (*this)[i] * other[i];
		return s;
	}

	T length() const
	{
		T len = T();
		for (size_t i = 0; i < csize; i++)
			len += (*this)[i] * (*this)[i];
		len = sqrt(len);
		return len;
	}

	T length2() const
	{
		T len = T();
		for (size_t i = 0; i < csize; i++)
			len += (*this)[i] * (*this)[i];
		return len;
	}

	T dist(const vect_prototype2& v) const
	{
		T len = T();
		for (size_t i = 0; i < csize; i++)
			len += ((*this)[i] - v[i]) * ((*this)[i] - v[i]);
		len = sqrt(len);
		return len;
	}

	T dist2(const vect_prototype2& v) const
	{
		T len = T();
		for (size_t i = 0; i < csize; i++)
			len += ((*this)[i] - v[i]) * ((*this)[i] - v[i]);
		return len;
	}

	friend T dist(const vect_prototype2& t, const vect_prototype2& v)
	{
		return t.dist(v);
	}

	friend T dist2(const vect_prototype2& t, const vect_prototype2& v)
	{
		return t.dist2(v);
	}

	vect_prototype2 normalize() const
	{
		vect_prototype2 ret;
		T len = length();
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] / len;
		return ret;
	}
};

//template <typename T, size_t S, typename DATA>
//const size_t vect_prototype2<T, S, DATA>::csize = S;

using fvect2 = vect_prototype2<float, 2, vect2_data<float>>;
using fvect3 = vect_prototype2<float, 3, vect3_data<float>>;
using fvect4 = vect_prototype2<float, 4, vect4_data<float>>;
using ivect2 = vect_prototype2<int, 2, vect2_data<int>>;
using ivect3 = vect_prototype2<int, 3, vect3_data<int>>;
using ivect4 = vect_prototype2<int, 4, vect4_data<int>>;
using szvect2 = vect_prototype2<size_t, 2, vect2_data<size_t>>;
using szvect3 = vect_prototype2<size_t, 3, vect3_data<size_t>>;
using szvect4 = vect_prototype2<size_t, 4, vect4_data<size_t>>;
using dvect2 = vect_prototype2<double, 2, vect2_data<double>>;
using dvect3 = vect_prototype2<double, 3, vect3_data<double>>;
using dvect4 = vect_prototype2<double, 4, vect4_data<double>>;
using color = vect_prototype2<unsigned char, 4, color_data>;

inline fvect2 fvect2fromAngle(float angle) 
{
	return fvect2((float)cos(angle), (float)sin(angle));
}

//inline float dist(fvect2 a, fvect2 b)
//{
//	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
//}

inline float angleTo(fvect2 from, fvect2 to)
{
	float r = dist(from, to);
	fvect2 delta = to - from;
	fvect2 cs = delta / r;
	cs.x = acosf(cs.x);
	if (cs.y < 0) cs.x = -cs.x;
	return cs.x;
}

inline fvect2 sinCosTo(fvect2 from, fvect2 to)
{
	float r = dist(from, to);
	fvect2 delta = to - from;
	fvect2 cs = delta / r;
	return cs;
}

inline float angleFromSinCos(fvect2 cs)
{
	cs.x = acosf(cs.x);
	if (cs.y < 0) cs.x = -cs.x;
	return cs.x;
}

#pragma pack(pop)