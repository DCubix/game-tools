#include <iostream>
#include <cassert>

#include "game_window.h"
#include "math/math.hpp"

class Game : public gt::GameAdapter {
public:

};

void test_mat_rotation() {
	// Rotates a Vector2 by 90°
	gt::Vector2 dir{ 1.0f, 0.0f };
	gt::Matrix2 mat = gt::rotation(gt::HalfPi);
	dir = gt::normalize(mat * dir);

	assert(gt::almostEqual(dir.x, 0.0f) && "X is not 0.0f");
	assert(gt::almostEqual(dir.y, 1.0f) && "Y is not 1.0f");
}

void test_mat_multiplication() {
	// Rotates a Vector3 on the Z axis by 45° and scales it by 2
	gt::Vector3 val{ 1.0f, 0.0f, 0.0f };
	gt::Matrix3 R = gt::axisAngle(gt::Vector3(0.0f, 0.0f, 1.0f), gt::HalfPi / 2.0f);
	gt::Matrix3 S = gt::scale(gt::Vector3(2.0f));
	gt::Matrix3 T = R * S;

	val = T * val;

	assert(gt::almostEqual(val.x, 1.0f) && "X is not 1.0f");
	assert(gt::almostEqual(val.y, 1.0f) && "Y is not 1.0f");
}

int main(int argc, char** argv) {
	test_mat_rotation();
	test_mat_multiplication();

	return gt::GameWindow(new Game(), 640, 480).run();
}
