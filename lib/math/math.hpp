#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

namespace gt {
	constexpr float Pi = 3.141592654f;
	constexpr float HalfPi = Pi / 2.0f;
	constexpr float Tau = Pi * 2.0f;
	constexpr float Epsilon = 1e-5f;
	constexpr float E = 2.718281828459045235360287f;

	// Ref: https://stackoverflow.com/a/253874
	inline bool almostEqual(float a, float b, float epsilon = E) {
		return std::fabs(a - b) <= ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
	}
}

#include "vector.hpp"
#include "matrix.hpp"

#endif // MATH_HPP