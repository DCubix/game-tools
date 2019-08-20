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
	rim = smoothstep(0.3, 1.0, rim);

	vec3 col = (1.0 - rim) * VS.color.rgb;
	col = clamp(col * n.rgb + col, 0.0, 1.0);
	fragColor = vec4(col * VS.color.a, 1.0) * n.a;
}
)";

float rnd() {
	return float(rand()) / float(RAND_MAX + 1.0f);
}

struct Object {
	Vector2 pos, vel, acel;
	Vector3 color;
	float life, maxLife, size, dim;
	int frame;
};

constexpr float circleScale = 0.2f;
constexpr float massFactor = 0.02f;

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

		srand(int(gw.currentTime() * 1000.0));
	}

	float lerpf(float a, float b, float t) {
		return (1.0f - t) * a + b * t;
	}

	void render(GameWindow& gw) {
		glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const float tw = 1.0f / 15;

		sb->begin();
		sb->enableBlending();
		sb->blendFunction(GL_ONE, GL_ONE);
		for (auto&& g : objects) {
			float d = g.dim;
			float tx = (g.frame % 15) * tw;

			sb->color(Vector4(g.color.x * d, g.color.y * d, g.color.z * d, 1.0f));
			sb->draw(tex, g.pos, 0.0f, Vector2(0.5f), Vector2(circleScale * g.size), Vector4(tx, 0.0f, tw, 1.0f));
		}
		sb->end();
	}

	void update(GameWindow& gw, float dt) {
		float fps = 1000.0f / gw.msPerFrame();
		gw.title(
			"Bubbles | " + std::to_string(gw.msPerFrame()) +
			" ms/frame | " + std::to_string(fps) + " FPS | " +
			std::to_string(objects.size()) + " objects"
		);

		if (gw.mouseHeld(1) && objects.size() < SpritesCount) {
			for (u32 i = 0; i < 10; i++) {
				Object g{};
				float a = rnd() * Tau;
				float f = 200.0f + rnd() * 300.0f;
				g.frame = 0;
				g.pos = gw.mousePosition();
				g.maxLife = 10.0f + rnd() * 20.0f;
				g.size = 1.0f + rnd() * 2.0f;
				g.life = g.maxLife;
				g.color.x = 0.4f + rnd() * 0.6f;
				g.color.y = 0.4f + rnd() * 0.6f;
				g.color.z = 0.4f + rnd() * 0.6f;
				g.acel = Vector2(f * std::cos(a), f * std::sin(a));
				objects.push_back(g);
			}
		}

		if (gw.mousePressed(3)) {
			for (size_t i = 0; i < objects.size(); i++) {
				auto& g = objects[i];
				Vector2 v = (gw.mousePosition() - g.pos);
				float gR = (g.size * circleScale) * tex.height() * 0.5f;
				if (length(v) < gR) {
					g.life = 0.2f;
				}
			}
		}

		if (gw.mouseHeld(2)) {
			for (size_t i = 0; i < objects.size(); i++) {
				auto&& g = objects[i];
				Vector2 v = (gw.mousePosition() - g.pos);
				if (length(v) < 80.0f) {
					g.vel -= normalize(v) * 20.0f;
				}
			}
		}

		std::vector<std::pair<Object*, Object*>> colliding;
		std::vector<size_t> rem;
		for (size_t i = 0; i < objects.size(); i++) {
			auto&& g = objects[i];

			g.vel += g.acel * dt;
			g.pos += g.vel * dt;

			g.dim = 0.2f + (std::sin(g.life * 2.0f) * 0.5f + 0.5f) * 0.8f;

			if (g.pos.x >= gw.width() || g.pos.x <= 0.0f) {
				g.vel.x *= -1.0f;
			}
			if (g.pos.y >= gw.height() || g.pos.y <= 0.0f) {
				g.vel.y *= -1.0f;
			}

			for (size_t j = 0; j < objects.size(); j++) {
				if (i == j) continue;

				auto& h = objects[j];
				float gR = (g.size * circleScale) * tex.height() * 0.5f;
				float hR = (h.size * circleScale) * tex.height() * 0.5f;
				float r = hR + gR;
				Vector2 vec = g.pos - h.pos;
				float d = lengthSqr(vec);
				if (std::abs(d) <= r * r) {
					float dist = std::sqrt(d);
					float overlap = (dist - gR - hR) * 0.5f;
					g.pos -= (vec / dist) * overlap;
					h.pos += (vec / dist) * overlap;

					colliding.push_back({ &g, &h });
				}
			}

			if (g.life <= 0.2f) {
				float t = 1.0f - (g.life / 0.2f);
				g.frame = int(std::floor(t * 15.0f));
			}
			if (g.life <= 0.0f) {
				rem.push_back(i);
			}

			g.acel *= 0.8f;
			g.vel *= 0.99f;
			g.life -= dt;
		}

		for (auto [a, b] : colliding) {
			Object& g = *a;
			Object& h = *b;

			float massG = g.size * massFactor;
			float massH = h.size * massFactor;

			Vector2 vec = h.pos - g.pos;
			float dist = length(vec);

			Vector2 n = vec / dist;
			Vector2 t = Vector2(-n.y, n.x);

			float dptG = dot(g.vel, t);
			float dptH = dot(h.vel, t);

			float dpnG = dot(g.vel, n);
			float dpnH = dot(h.vel, n);

			float m1 = (dpnG * (massG - massH) + 2.0f * massH * dpnH) / (massG + massH);
			float m2 = (dpnH * (massH - massG) + 2.0f * massG * dpnG) / (massG + massH);

			g.vel = t * dptG + n * m1;
			h.vel = t * dptH + n * m2;
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
