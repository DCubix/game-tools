#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "vector.hpp"

namespace gt {

	template <size_t N>
	struct Mat {
		static_assert(N >= 2 && N <= 4, "Invalid matrix size.");
		Vec<N> m[N];
	};

	template <>
	struct Mat<2> {
		union {
			Vec<2> m[2];
			struct { float m00, m01, m10, m11; };
		};

		inline Mat(float m00, float m01, float m10, float m11)
			: m00(m00), m10(m10), m11(m11), m01(m01)
		{}
		inline Mat() : Mat(1, 0, 0, 1) {}

		inline const Vec<2>& operator [](unsigned int i) const { return m[i]; }
		inline Vec<2>& operator [](unsigned int i) { return m[i]; }
	};

	template <>
	struct Mat<3> {
		union {
			Vec<3> m[3];
			struct {
				float m00, m01, m02;
				float m10, m11, m12;
				float m20, m21, m22;
			};
		};

		inline Mat(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22
		) : m00(m00), m01(m01), m02(m02),
			m10(m10), m11(m11), m12(m12),
			m20(m20), m21(m21), m22(m22)
		{}
		inline Mat() : Mat(1, 0, 0, 0, 1, 0, 0, 0, 1) {}

		inline const Vec<3>& operator [](unsigned int i) const { return m[i]; }
		inline Vec<3>& operator [](unsigned int i) { return m[i]; }
	};

	template <>
	struct Mat<4> {
		union {
			Vec<4> m[4];
			struct {
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
		};

		inline Mat(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		) : m00(m00), m01(m01), m02(m02), m03(m03),
			m10(m10), m11(m11), m12(m12), m13(m13),
			m20(m20), m21(m21), m22(m22), m23(m23),
			m30(m30), m31(m31), m32(m32), m33(m33)
		{}
		inline Mat() : Mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}
		inline Mat(const Vec<4>& r0, const Vec<4>& r1, const Vec<4>& r2, const Vec<4>& r3) : Mat() {
			m[0] = r0;
			m[1] = r1;
			m[2] = r2;
			m[3] = r3;
		}
		inline Mat(const Mat<4>& a) : Mat() {
			for (size_t j = 0; j < 4; j++) {
				for (size_t i = 0; i < 4; i++) {
					m[j][i] = a[j][i];
				}
			}
		}

		inline const Vec<4>& operator [](unsigned int i) const { return m[i]; }
		inline Vec<4>& operator [](unsigned int i) { return m[i]; }
	};

	template <size_t N> inline Mat<N> operator *(const Mat<N>& a, float b) {
		Mat<N> ret;
		for (size_t i = 0; i < N; i++) ret.m[i] = a[i] * b;
		return ret;
	}

	template <size_t N> inline Mat<N> operator *(const Mat<N>& a, const Mat<N>& b) {
		Mat<N> ret;
		for (size_t r = 0; r < N; r++) {
			for (size_t c = 0; c < N; c++) {
				ret[r][c] = 0.0f;
				for (size_t k = 0; k < N; k++) {
					ret[r][c] += a[r][k] * b[k][c];
				}
			}
		}
		return ret;
	}

	template <size_t N> inline Vec<N> operator *(const Mat<N>& a, const Vec<N>& b) {
		Vec<N> ret;
		for (size_t i = 0; i < N; i++) {
			ret[i] = dot(a[i], b);
		}
		return ret;
	}

	template <size_t N> inline Mat<N> transpose(const Mat<N>& a) {
		Mat<N> ret;
		for (size_t r = 0; r < N; r++) {
			for (size_t c = 0; c < N; c++) {
				ret[r][c] = a[c][r];
			}
		}
		return ret;
	}

	template <size_t N> inline float determinant(const Mat<N>& a) {
		if constexpr (N == 2) {
			return a.m[0][0] * a.m[1][1] - a.m[1][0] * a.m[0][1];
		} else if constexpr (N == 3) {
			return a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) -
					a.m[1][0] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]) +
					a.m[2][0] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
		} else if constexpr (N == 4) {
			auto&& m = a.m;
			return m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
					m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
					m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1] * m[1][2] * m[2][3] * m[3][0] -
					m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
					m[0][3] * m[1][0] * m[2][2] * m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1] -
					m[0][2] * m[1][0] * m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1] +
					m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
					m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
					m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1] * m[2][3] * m[3][2] -
					m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1] * m[1][2] * m[2][0] * m[3][3] +
					m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][0] * m[1][2] * m[2][1] * m[3][3] -
					m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3];
		}
		return 0.0f;
	}

	template <size_t N> inline Mat<N> inverse(const Mat<N>& a) {
		auto&& m = a.m;
		if constexpr (N == 2) {
			float dinv = 1.0f / determinant(a);
			return Mat<2>(
				 m[1][1] * dinv,
				-m[0][1] * dinv,
				-m[1][0] * dinv,
				 m[0][0] * dinv
			);
		} else if constexpr (N == 3) {
			float dinv = 1.0f / determinant(a);
			Mat<N> inv;
			inv.m[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * dinv;
			inv.m[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * dinv;
			inv.m[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * dinv;
			inv.m[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * dinv;
			inv.m[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * dinv;
			inv.m[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * dinv;
			inv.m[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * dinv;
			inv.m[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * dinv;
			inv.m[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * dinv;

			return inv;
		} else if constexpr (N == 4) {
			float c00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			float c02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			float c03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

			float c04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			float c06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			float c07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

			float c08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			float c10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
			float c11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

			float c12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
			float c14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
			float c15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

			float c16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
			float c18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
			float c19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

			float c20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
			float c22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
			float c23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

			Vec<4> f0(c00, c00, c02, c03);
			Vec<4> f1(c04, c04, c06, c07);
			Vec<4> f2(c08, c08, c10, c11);
			Vec<4> f3(c12, c12, c14, c15);
			Vec<4> f4(c16, c16, c18, c19);
			Vec<4> f5(c20, c20, c22, c23);

			Vec<4> Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
			Vec<4> Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
			Vec<4> Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
			Vec<4> Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

			Vec<4> i0 = (Vec1 * f0 - Vec2 * f1 + Vec3 * f2);
			Vec<4> i1 = (Vec0 * f0 - Vec2 * f3 + Vec3 * f4);
			Vec<4> i2 = (Vec0 * f1 - Vec1 * f3 + Vec3 * f5);
			Vec<4> i3 = (Vec0 * f2 - Vec1 * f4 + Vec2 * f5);

			Vec<4> sA(+1, -1, +1, -1);
			Vec<4> sB(-1, +1, -1, +1);
			Mat<4> inv(i0 * sA, i1 * sB, i2 * sA, i3 * sB);

			Vec<4> r0(inv[0][0], inv[1][0], inv[2][0], inv[3][0]);
			Vec<4> d0 = (m[0] * r0);
			float d1 = (d0.x + d0.y) + (d0.z + d0.w);
			float dinv = 1.0f / d1;

			return inv * dinv;
		}
	}

	inline Mat<2> rotation(float angle) {
		const float c = std::cos(angle), s = std::sin(angle);
		return Mat<2>(c, -s, s, c);
	}

	inline Mat<3> axisAngle(const Vec<3>& axis, float angle) {
		const float s = std::sin(angle),
					c = std::cos(angle),
					t = 1.0f - c;
		const Vec<3> ax = normalize(axis);
		float x = ax.x, y = ax.y, z = ax.z;
		return Mat<3>(
			t * x * x + c, t * x * y - z * s, t * x * z + y * s,
			t * x * y + z * s, t * y * y + c, t * y * z + x * s,
			t * x * z - y * s, t * y * z + x * s, t * z * z + c
		);
	}

	inline Mat<3> rotation(const Vec<3>& forward, const Vec<3>& up, const Vec<3>& right) {
		const Vec<3>& f = forward, u = up, r = right;
		return Mat<3>(
			r.x, r.y, r.z,
			u.x, u.y, u.z,
			f.x, f.y, f.z
		);
	}

	inline Mat<3> rotation(const Vec<3>& forward, const Vec<3>& up) {
		const Vec<3> f = normalize(forward);
		Vec<3> r = normalize(up);
		r = cross(r, f);

		const Vec<3> u = cross(f, r);
		return rotation(f, u, r);
	}

	inline Mat<2> scale(const Vec<2>& sc) {
		return Mat<2>(sc.x, 0, 0, sc.y);
	}

	inline Mat<3> scale(const Vec<3>& sc) {
		return Mat<3>(
			sc.x, 0, 0,
			0, sc.y, 0,
			0, 0, sc.z
		);
	}

	inline Mat<4> translation(const Vec<3>& sc) {
		return Mat<4>(
			1, 0, 0, sc.x,
			0, 1, 0, sc.y,
			0, 0, 1, sc.z,
			0, 0, 0, 1
		);
	}

	inline Mat<4> ortho(float left, float right, float bottom, float top, float near, float far) {
		const float w = right - left;
		const float h = top - bottom;
		const float d = far - near;
		return Mat<4>(
			2.0f / w, 0.0f, 0.0f, -((right + left) / w),
			0.0f, 2.0f / h, 0.0f, -((top + bottom) / h),
			0.0f, 0.0f, -2.0f / d, -((far + near) / d),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	inline Mat<4> perspective(float fov, float aspect, float near, float far) {
		const float thf = std::tan(fov / 2.0f);
		return Mat<4>(
			1.0f / (aspect * thf), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / thf, 0.0f, 0.0f,
			0.0f, 0.0f, -((far + near) / (far - near)), -((2.0f * far * near) / (far - near)),
			0.0f, 0.0f, 1.0f, 0.0f
		);
	}

	using Matrix2 = Mat<2>;
	using Matrix3 = Mat<3>;
	using Matrix4 = Mat<4>;
}

#endif // MATRIX_HPP