#include <iostream>
#include <cassert>
#include <cstdlib>

#include "game_window.h"
#include "graphics/sprite_batch.h"
#include "math/math.hpp"

using namespace gt;

#include "stb/stb_image.h"

float rnd() {
	return float(rand()) / float(RAND_MAX + 1.0f);
}

struct Goomba {
	Vector2 pos, vel, acel;
};

class Game : public GameAdapter {
public:
	void create(GameWindow& gw) {
		sb = std::unique_ptr<SpriteBatch>(new SpriteBatch(gw.width(), gw.height()));

		i32 w, h, comp;
		u8* data = stbi_load("diego.png", &w, &h, &comp, 4);
		if (data) {
			tex.create(TextureType::Texture2D, Format::RGBA, w, h).bind()
				.filter(TextureFilter::Nearest, TextureFilter::Nearest)
				.wrapMode(TextureWrap::Repeat, TextureWrap::Repeat)
				.update(data, DataType::TypeUByte)
				.generateMipmaps();
			stbi_image_free(data);
		}

		objects.reserve(20000);
	}

	void render(GameWindow& gw) {
		glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sb->begin();
		for (auto&& g : objects) {
			sb->draw(tex, g.pos, 0.0f, Vector2(0.5f));
		}
		sb->end();
	}

	void update(GameWindow& gw, float dt) {
		float fps = 1000.0f / gw.msPerFrame();
		gw.title(
			"Game Test | " + std::to_string(gw.msPerFrame()) +
			" ms/frame | " + std::to_string(fps) + " FPS | " +
			std::to_string(objects.size()) + " objects");

		if (gw.keyHeld(SDLK_a) && objects.size() < SpritesCount) {
			for (u32 i = 0; i < 100; i++) {
				Goomba g{};
				float a = rnd() * Tau;
				float f = 20.0f + rnd() * 50.0f;
				g.pos = Vector2(gw.width() / 2.0f, gw.height() / 2.0f);
				g.acel = Vector2(f * std::cos(a), f * std::sin(a));
				objects.push_back(g);
			}
		}

		if (gw.mouseHeld(1)) {
			for (size_t i = 0; i < objects.size(); i++) {
				auto&& g = objects[i];
				Vector2 v = (gw.mousePosition() - g.pos);
				if (length(v) < 80.0f) {
					g.vel -= normalize(v) * 10.0f;
				}
			}
		}

		for (size_t i = 0; i < objects.size(); i++) {
			auto&& g = objects[i];
			g.vel += g.acel;
			g.pos += g.vel * dt;
			if (g.pos.x >= gw.width() || g.pos.x <= 0.0f) {
				g.vel.x *= -1.0f;
			}
			if (g.pos.y >= gw.height() || g.pos.y <= 0.0f) {
				g.vel.y *= -1.0f;
			}
			g.acel *= 0.5f;
			g.vel *= 0.99f;
		}
	}

	std::vector<Goomba> objects;
	std::unique_ptr<SpriteBatch> sb;
	Texture tex;
};

int main(int argc, char** argv) {
	return GameWindow(new Game(), 640, 480).run();
}
