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
uniform sampler2D uNormals;

uniform vec3 uLightPos = vec3(320.0, 240.0, 2.0);
uniform float uRadius = 200.0;

void main() {
	vec4 A = texture(uTexture, VS.uv);
	vec3 N = texture(uNormals, VS.uv).xyz * 2.0 - 1.0;

	vec3 vec = (uLightPos - gl_FragCoord.xyz);
	vec3 L = normalize(vec);
	vec3 aL = vec / uRadius;
	float att = 1.0 - dot(aL, aL);

	float nl = clamp(dot(N, L), 0.0, 1.0) * att;

	fragColor = A * vec4(vec3(nl), A.a);
}
)";

float rnd() {
	return float(rand()) / float(RAND_MAX + 1.0f);
}

class Game : public GameAdapter {
public:
	void create(GameWindow& gw) {
		sb = std::unique_ptr<SpriteBatch>(new SpriteBatch(gw.width(), gw.height()));

		i32 w, h, comp;
		u8* data = stbi_load("color.png", &w, &h, &comp, 4);
		if (data) {
			col.create(TextureType::Texture2D, Format::RGBA, w, h).bind()
				.filter(TextureFilter::Linear, TextureFilter::LinearMipMapLinear)
				.wrapMode(TextureWrap::Repeat, TextureWrap::Repeat)
				.update(data, DataType::TypeUByte)
				.generateMipmaps();
			stbi_image_free(data);
		}

		data = stbi_load("normals.png", &w, &h, &comp, 4);
		if (data) {
			norm.create(TextureType::Texture2D, Format::RGBA, w, h).bind()
				.filter(TextureFilter::Linear, TextureFilter::LinearMipMapLinear)
				.wrapMode(TextureWrap::Repeat, TextureWrap::Repeat)
				.update(data, DataType::TypeUByte)
				.generateMipmaps();
			stbi_image_free(data);
		}

		carShader.create().bind()
			.add(SBVertexShader, Shader::VertexShader)
			.add(FS, Shader::FragmentShader)
			.link();
		sb->shader(carShader);

		srand(int(gw.currentTime() * 1000.0));
	}

	float lerpf(float a, float b, float t) {
		return (1.0f - t) * a + b * t;
	}

	void render(GameWindow& gw) {
		glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sb->shader(carShader);
		sb->begin();
		sb->enableBlending();
		sb->blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		carShader.get("uLightPos").set(Vector3(gw.mousePosition().x, gw.height() - gw.mousePosition().y, 10.0));
		carShader.get("uNormals").set(1);
		norm.bind(1);

		sb->draw(col, Vector2(10.0f));
		sb->end();

		sb->resetShader();
		sb->begin();
		sb->enableBlending();
		sb->blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		sb->draw(col, Vector2(10.0f, 256.0f), 0.0f, Vector2(0.0f), Vector2(0.25f));
		sb->draw(norm, Vector2(138.0f, 256.0f), 0.0f, Vector2(0.0f), Vector2(0.25f));
		sb->end();
	}

	void update(GameWindow& gw, float dt) {
		float fps = 1000.0f / gw.msPerFrame();
		gw.title(
			"Car | " + std::to_string(gw.msPerFrame()) +
			" ms/frame | " + std::to_string(fps) + " FPS"
		);
	}

	void destroy(GameWindow& gw) {
		sb.reset();
	}

	std::unique_ptr<SpriteBatch> sb;
	Texture norm, col;
	Shader carShader;
};

int main(int argc, char** argv) {
	return GameWindow(new Game(), 640, 480).run();
}
