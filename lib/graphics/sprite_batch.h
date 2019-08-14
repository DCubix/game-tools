#ifndef SPRITE_BATCH_H
#define SPRITE_BATCH_H

#include "buffer.h"
#include "shader.h"
#include "texture.h"
#include "../math/math.hpp"
#include "../stl.hpp"

namespace gt {
	inline static const std::string SBVertexShader = R"(#version 430 core
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec4 vColor;
layout (location = 3) in vec3 vTangent;

uniform mat4 uProjView = mat4(1.0);

out DATA {
	vec4 color;
	vec4 position;
	vec2 uv;
	mat3 tbn;
} VS;

void main() {
	vec4 pos = vec4(vPosition, 0.0, 1.0);
	gl_Position = uProjView * pos;

	VS.color = vColor;
	VS.position = pos;
	VS.uv = vTexCoord;

	const vec3 N = vec3(0.0, 0.0, 1.0);
	vec3 T = normalize(vTangent - dot(vTangent, N) * N);
	vec3 B = cross(T, N);
	VS.tbn = mat3(T, B, N);
}
)";

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

		const Vector4& color() const { return m_color; }
		void color(const Vector4& col) { m_color = col; }

		GLenum blendSrcFunc() const { return m_srcFuncColor; }
		GLenum blendDstFunc() const { return m_dstFuncColor; }
		GLenum blendSrcFuncAlpha() const { return m_srcFuncAlpha; }
		GLenum blendDstFuncAlpha() const { return m_dstFuncAlpha; }

		void blendFunctionSeparate(GLenum src, GLenum dst, GLenum srcAlpha, GLenum dstAlpha);
		void blendFunction(GLenum src, GLenum dst);

		void enableBlending();
		void disableBlending();

		bool isDrawing() const { return m_drawing; }

	private:
		struct Vertex {
			Vector2 position;
			Vector2 texCoord;
			Vector4 color;
			Vector3 tangent;

			Vertex() = default;
			Vertex(const Vector2& pos, const Vector2& uv, const Vector4& col, const Vector3& tan)
				: position(pos), texCoord(uv), color(col), tangent(tan) {}
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

		bool m_blending{ false };
		GLenum m_srcFuncColor, m_dstFuncColor, m_srcFuncAlpha, m_dstFuncAlpha;

		void setupMatrices();
		void switchTexture(const Texture& tex);
	};
}

#endif // SPRITE_BATCH_H