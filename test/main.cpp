#include <iostream>
#include <cassert>
#include <cstdlib>
#include <algorithm>

#include "game_window.h"
#include "graphics/sprite_batch.h"
#include "math/math.hpp"

using namespace gt;

#include "stb/stb_image.h"

inline static const std::string FS = R"(#version 430 core
out vec4 fragColor;

in DATA {
	vec4 color;
	vec4 position;
	vec2 uv;
	mat3 tbn;
} VS;

uniform sampler2D uTexture;

void main() {
	const vec3 V = vec3(0.0, 0.0, 1.0);
	const vec3 L = vec3(1.0, 1.0, -1.0);
	vec4 n = texture(uTexture, VS.uv);
	vec3 nrm = normalize(VS.tbn * (n.xyz * 2.0 - 1.0));

	float rim = clamp(dot(nrm, V), 0.0, 1.0);
	rim = smoothstep(0.4, 1.0, rim);

	vec3 col = (1.0 - rim) * VS.color.rgb;
	col = clamp(col * n.rgb, 0.0, 1.0);
	fragColor = vec4(col * VS.color.a, 1.0) * n.a;
}
)";

float rnd() {
	return float(rand()) / float(RAND_MAX + 1.0f);
}

struct Object {
	Vector2 pos, vel, acel;
	Vector3 color;
	float life, maxLife, size;
};

class Game : public GameAdapter {
public:
	void create(GameWindow& gw) {
		sb = std::unique_ptr<SpriteBatch>(new SpriteBatch(gw.width(), gw.height()));

		i32 w, h, comp;
		u8* data = stbi_load("ball.png", &w, &h, &comp, 4);
		if (data) {
			tex.create(TextureType::Texture2D, Format::RGBA, w, h).bind()
				.filter(TextureFilter::Linear, TextureFilter::LinearMipMapLinear)
				.wrapMode(TextureWrap::Repeat, TextureWrap::Repeat)
				.update(data, DataType::TypeUByte)
				.generateMipmaps();
			stbi_image_free(data);
		}

		objects.reserve(20000);

		normals.create().bind()
			.add(SBVertexShader, Shader::VertexShader)
			.add(FS, Shader::FragmentShader)
			.link();
		sb->shader(normals);
	}

	float lerpf(float a, float b, float t) {
		return (1.0f - t) * a + b * t;
	}

	void render(GameWindow& gw) {
		glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sb->begin();
		sb->enableBlending();
		sb->blendFunction(GL_ONE, GL_ONE);
		for (auto&& g : objects) {
			float t = g.life / g.maxLife;
			sb->color(Vector4(g.color.x, g.color.y, g.color.z, t));
			sb->draw(tex, g.pos, 0.0f, Vector2(0.5f), Vector2(0.25f * g.size));
		}
		sb->end();
	}

	void update(GameWindow& gw, float dt) {
		float fps = 1000.0f / gw.msPerFrame();
		gw.title(
			"Game Test | " + std::to_string(gw.msPerFrame()) +
			" ms/frame | " + std::to_string(fps) + " FPS | " +
			std::to_string(objects.size()) + " objects"
		);

		if (gw.mouseHeld(1) && objects.size() < SpritesCount) {
			Object g{};
			float a = rnd() * Tau;
			float f = 20.0f + rnd() * 50.0f;
			g.pos = gw.mousePosition();
			g.maxLife = 4.0f + rnd() * 6.0f;
			g.size = 1.0f + rnd() * 2.0f;
			g.life = g.maxLife;
			g.color.x = 0.2f + rnd();
			g.color.y = 0.2f + rnd();
			g.color.z = 0.2f + rnd();
			g.acel = Vector2(f * std::cos(a), f * std::sin(a));
			objects.push_back(g);
		}

		if (gw.mouseHeld(3)) {
			for (size_t i = 0; i < objects.size(); i++) {
				auto&& g = objects[i];
				Vector2 v = (gw.mousePosition() - g.pos);
				if (length(v) < 80.0f) {
					g.vel -= normalize(v) * 10.0f;
				}
			}
		}

		std::vector<size_t> rem;
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
			g.life -= dt;
			if (g.life <= 0.5f) {
				rem.push_back(i);
			}
		}

		std::sort(rem.begin(), rem.end());
		std::reverse(rem.begin(), rem.end());
		for (size_t i : rem) {
			objects.erase(objects.begin() + i);
		}
	}

	void destroy(GameWindow& gw) {
		sb.reset();
	}

	std::vector<Object> objects;
	std::unique_ptr<SpriteBatch> sb;
	Texture tex;
	Shader normals;
};

int main(int argc, char** argv) {
	return GameWindow(new Game(), 640, 480).run();
}
