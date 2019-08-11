#include <iostream>
#include <cassert>

#include "game_window.h"
#include "graphics/sprite_batch.h"
#include "math/math.hpp"

using namespace gt;

#include "stb/stb_image.h"

class Game : public GameAdapter {
public:
	void create(GameWindow& gw) {
		sb = std::unique_ptr<SpriteBatch>(new SpriteBatch(gw.width(), gw.height()));

		i32 w, h, comp;
		u8* data = stbi_load("bricks.png", &w, &h, &comp, 4);
		if (data) {
			tex.create(TextureType::Texture2D, Format::RGBA, w, h).bind()
				.filter(TextureFilter::Nearest, TextureFilter::Nearest)
				.wrapMode(TextureWrap::Repeat, TextureWrap::Repeat)
				.update(data, DataType::TypeUByte)
				.generateMipmaps();
			stbi_image_free(data);
		}
	}

	void render(GameWindow& gw) {
		sb->begin();
		sb->draw(tex, Vector2(0.0f, 0.0f));
		sb->end();
	}

	void update(GameWindow& gw, float dt) {
		float fps = 1000.0f / gw.msPerFrame();
		gw.title("Game Test | " + std::to_string(gw.msPerFrame()) + " ms/frame | " + std::to_string(fps) + " FPS");

		if (gw.keyPressed(SDLK_a)) {

		}
	}

	std::unique_ptr<SpriteBatch> sb;
	Texture tex;
};

int main(int argc, char** argv) {
	return GameWindow(new Game(), 640, 480).run();
}
