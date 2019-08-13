#ifndef SPRITE_BATCH_H
#define SPRITE_BATCH_H

#include "buffer.h"
#include "shader.h"
#include "texture.h"
#include "../math/math.hpp"
#include "../stl.hpp"

#define SafeDel(x) if (x) { delete x; x = nullptr; }

namespace gt {
	constexpr u32 SpritesCount = 30000;

	class SpriteBatch {
	public:
		SpriteBatch() = default;
		SpriteBatch(u32 width, u32 height);
		~SpriteBatch();

		void draw(
			const Texture& texture,
			Vector2 position,
			float rotation = 0.0f,
			Vector2 origin = Vector2(0.0f),
			Vector2 scale = Vector2(1.0f),
			Vector4 uv = Vector4(0, 0, 1, 1)
		);

		void begin();
		void flush();
		void end();

		const Matrix4& projectionMatrix() const { return m_projection; }
		const Matrix4& viewMatrix() const { return m_view; }

		void projectionMatrix(const Matrix4& v);
		void viewMatrix(const Matrix4& v);

		Shader& shader() { return m_currentShader; }
		void shader(const Shader& s);
		void resetShader() { shader(Shader(0)); }

		bool isDrawing() const { return m_drawing; }

	private:
		struct Vertex {
			Vector2 position;
			Vector2 texCoord;
			Vector4 color;

			Vertex() = default;
			Vertex(const Vector2& pos, const Vector2& uv, const Vector4& col)
				: position(pos), texCoord(uv), color(col) {}
		};

		std::vector<Vertex> m_vertices;
		std::vector<u32> m_indices;

		VertexArray m_vao;
		Buffer m_vbo, m_ibo;

		Matrix4 m_projection, m_view;

		Shader m_defaultShader, m_currentShader;
		Texture m_lastTexture{};

		Vector4 m_color{ 1.0f };

		bool m_drawing{ false };

		void setupMatrices();
		void switchTexture(const Texture& tex);
	};
}

#endif // SPRITE_BATCH_H