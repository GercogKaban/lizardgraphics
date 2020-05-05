#pragma once
//#include "../LizardScript/LizardScript/metagen_def.h"

#pragma pack(push, 1)

template <typename T>
struct vect2_data
{
	T x, y;
};

template <typename T>
struct vect3_data
{
	T x, y, z;
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

template <typename T, size_t S, typename DATA>
struct /*lsclass*/ vect_prototype2 : public DATA
{
    /*NOT_METADATA*/ using val_type = T;

	static const size_t csize;

	size_t size() const
	{
		return size;
	}

    public:

	vect_prototype2()
	{
		for (size_t i = 0; i < csize; i++)
			(*this)[i] = 0;
	}

	//vect_prototype2(const vect_prototype2& vect)
	//{
	//	for (size_t i = 0; i < csize; i++)
	//		(*this)[i] = vect[i];
	//}

	//vect_prototype2(vect_prototype2&& vect)
	//{
	//	for (size_t i = 0; i < csize; i++)
	//		(*this)[i] = vect[i];
	//}

	vect_prototype2(T a, T b)
	{
		(*this)[0] = a;
		(*this)[1] = b;
	}

	vect_prototype2(T a, T b, T c)
	{
		(*this)[0] = a;
		(*this)[1] = b;
		(*this)[2] = c;
	}

	T& operator[](size_t index) const
	{
		return ((T*)this)[index];
	}

	vect_prototype2 operator*(T val) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] * val;
		return ret;
	}

	vect_prototype2 operator*(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] * v[i];
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

	vect_prototype2 operator/(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] / v[i];
		return ret;
	}

	friend vect_prototype2 operator/(T val, const vect_prototype2& v)
	{
		return v / val;
	}

	vect_prototype2 operator-() const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = -(*this)[i];
		return ret;
	}

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

	vect_prototype2 operator+(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] + v[i];
		return ret;
	}
	vect_prototype2 operator-(vect_prototype2&& v) const { return this->operator-(v); }

	bool operator>(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] <= v[i])
				return false;
		return true;
	}
	bool operator>(vect_prototype2&& v) const { return this->operator>(v); }

	bool operator<(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] >= v[i])
				return false;
		return true;
	}
	bool operator<(vect_prototype2&& v) const { return this->operator<(v); }

	bool operator==(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] != v[i])
				return false;
		return true;
	}
	bool operator==(vect_prototype2&& v) const { return this->operator==(v); }

	bool operator!=(const vect_prototype2& v) const
	{
		for (size_t i = 0; i < csize; i++)
			if ((*this)[i] != v[i])
				return true;
		return false;
	}
	bool operator!=(vect_prototype2&& v) const { return this->operator!=(v); }

	vect_prototype2 operator-(const vect_prototype2& v) const
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] - v[i];
		return ret;
	}
	vect_prototype2 operator+(vect_prototype2&& v) const { return this->operator+(v); }

	vect_prototype2 operator+=(const vect_prototype2& v)
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] += v[i];
		return ret;
	}

	vect_prototype2 operator-=(const vect_prototype2& v)
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] -= v[i];
		return ret;
	}

	vect_prototype2 operator*=(float val)
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] *= val;
		return ret;
	}

	vect_prototype2 operator/=(float val)
	{
		vect_prototype2 ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (*this)[i] /= val;
		return ret;
	}

	template <typename TO>
	TO cut() const
	{
		//вычисляем, сколько элементов вектора нужно скопировать
		size_t maxSize = csize;
		if (TO::csize < maxSize)
			maxSize = TO::csize;

		TO ret;
		for (size_t i = 0; i < maxSize; i++)
			ret[i] = (TO::val_type)(*this)[i];
		return ret;
	}


	template <typename TO, size_t TOS, typename TODATA>
	operator vect_prototype2<TO, TOS, TODATA>()
	{
		vect_prototype2<TO, TOS, TODATA> ret;
		for (size_t i = 0; i < csize; i++)
			ret[i] = (TO)((*this)[i]);
		return ret;
	}

	T* begin() { return &(*this)[0]; }
	T* end() { return &(*this)[csize]; }
};

template <typename T, size_t S, typename DATA>
const size_t vect_prototype2<T, S, DATA>::csize = S;

using fvect2 = vect_prototype2<float, 2, vect2_data<float>>;
using fvect3 = vect_prototype2<float, 3, vect3_data<float>>;
using ivect2 = vect_prototype2<int, 2, vect2_data<int>>;
using ivect3 = vect_prototype2<int, 3, vect3_data<int>>;
using szvect2 = vect_prototype2<size_t, 2, vect2_data<size_t>>;
using szvect3 = vect_prototype2<size_t, 3, vect3_data<size_t>>;
using color = vect_prototype2<unsigned char, 4, color_data>;

inline fvect2 fvect2fromAngle(float angle) 
{
	return fvect2((float)cos(angle), (float)sin(angle));
}

inline float dist(fvect2 a, fvect2 b)
{
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

#pragma pack(pop)