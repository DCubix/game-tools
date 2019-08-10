#include <iostream>

#include "game_window.h"
#include "math/vector.hpp"
#include "math/matrix.hpp"

class Game : public gt::GameAdapter {
public:

};

int main(int argc, char** argv) {
	gt::Vector2 dir{ 1.0f, 0.0f };
	gt::Matrix2 mat = gt::rotation(gt::Pi);
	dir = gt::normalize(mat * dir);
	std::cout << dir.x << ", " << dir.y << std::endl;

	return gt::GameWindow(new Game(), 640, 480).run();
}
