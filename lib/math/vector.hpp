#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>

#define VEC_OP(op) \
Vec<N> ret; \
for (size_t i = 0; i < N; i++) { ret.data[i] = a.data[i] op b.data[i]; } \
return ret;

#define VEC_OP_S(op) \
Vec<N> ret; \
for (size_t i = 0; i < N; i++) { ret.data[i] = a.data[i] op b; } \
return ret;

#define VEC_AOP(op) \
for (size_t i = 0; i < N; i++) { a.data[i] op##= b.data[i]; } \
return a;

#define VEC_AOP_S(op) \
for (size_t i = 0; i < N; i++) { a.data[i] op##= b; } \
return a;

namespace gt {

	template <size_t N>
	struct Vec {
		static_assert(N >= 2 && N <= 4, "Invalid vector size.");
		float data[N];
	};

	// Specializations
	template <>
	struct Vec<2> {
		union {
			float data[2];
			struct { float x, y; };
		};

		inline Vec(float x, float y) : x(x), y(y) {}
		inline Vec(float v) : Vec(v, v) {}
		inline Vec() : Vec(0.0f) {}

		inline const float& operator [](unsigned int i) const { return data[i]; }
		inline float& operator [](unsigned int i) { return data[i]; }
	};

	template <>
	struct Vec<3> {
		union {
			float data[3];
			struct { float x, y, z; };
			Vec<2> xy;
		};

		inline Vec(float x, float y, float z) : x(x), y(y), z(z) {}
		inline Vec(float v) : Vec(v, v, v) {}
		inline Vec() : Vec(0.0f) {}

		inline const float& operator [](unsigned int i) const { return data[i]; }
		inline float& operator [](unsigned int i) { return data[i]; }
	};

	template <>
	struct Vec<4> {
		union {
			float data[4];
			struct { float x, y, z, w; };
			Vec<2> xy;
			Vec<3> xyz;
			Vec<3> rgb;
		};

		inline Vec(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		inline Vec(float v) : Vec(v, v, v, v) {}
		inline Vec() : Vec(0.0f) {}

		inline const float& operator [](unsigned int i) const { return data[i]; }
		inline float& operator [](unsigned int i) { return data[i]; }
	};

	// Operators
	template <size_t N> inline Vec<N> operator +(const Vec<N>& a, const Vec<N>& b) { VEC_OP(+) }
	template <size_t N> inline Vec<N> operator -(const Vec<N>& a, const Vec<N>& b) { VEC_OP(-) }
	template <size_t N> inline Vec<N> operator *(const Vec<N>& a, const Vec<N>& b) { VEC_OP(*) }
	template <size_t N> inline Vec<N> operator *(const Vec<N>& a, float b) { VEC_OP_S(*) }
	template <size_t N> inline Vec<N> operator /(const Vec<N>& a, float b) { VEC_OP_S(/) }

	template <size_t N> inline Vec<N>& operator +=(Vec<N>& a, const Vec<N>& b) { VEC_AOP(+) }
	template <size_t N> inline Vec<N>& operator -=(Vec<N>& a, const Vec<N>& b) { VEC_AOP(-) }
	template <size_t N> inline Vec<N>& operator *=(Vec<N>& a, const Vec<N>& b) { VEC_AOP(*) }
	template <size_t N> inline Vec<N>& operator *=(Vec<N>& a, float b) { VEC_AOP_S(*) }
	template <size_t N> inline Vec<N>& operator /=(Vec<N>& a, float b) { VEC_AOP_S(/) }

	template <size_t N>
	inline float dot(const Vec<N>& a, const Vec<N>& b) {
		float sum = 0.0f;
		for (size_t i = 0; i < N; i++) sum += a[i] * b[i];
		return sum;
	}

	template <size_t N>
	inline float lengthSqr(const Vec<N>& a) { return dot(a, a); }

	template <size_t N>
	inline float length(const Vec<N>& a) { return std::sqrt(lengthSqr(a)); }

	template <size_t N>
	inline Vec<N> normalize(const Vec<N>& a) { return a / length(a); }

	inline Vec<3> cross(const Vec<3>& a, const Vec<3>& b) {
		Vec<3> ret;
		ret.data[0] = a.data[1] * b.data[2] - a.data[2] * b.data[1];
		ret.data[1] = a.data[2] * b.data[0] - a.data[0] * b.data[2];
		ret.data[2] = a.data[0] * b.data[1] - a.data[1] * b.data[0];
		return ret;
	}

	template <size_t N>
	inline Vec<N> lerp(const Vec<N>& a, const Vec<N>& b, float t) {
		Vec<N> ret;
		for (size_t i = 0; i < N; i++) ret.data[i] = (1.0f - t) * a.data[i] + b.data[i] * t;
		return ret;
	}

	using Vector2 = Vec<2>;
	using Vector3 = Vec<3>;
	using Vector4 = Vec<4>;
}

#endif // VECTOR_HPP