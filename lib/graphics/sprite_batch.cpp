#include "sprite_batch.h"

namespace gt {
	static const std::string VS = R"(#version 430 core
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec4 vColor;

uniform mat4 uProjView = mat4(1.0);

out DATA {
	vec4 color;
	vec4 position;
	vec2 uv;
} VS;

void main() {
	vec4 pos = vec4(vPosition, 0.0, 1.0);
	gl_Position = uProjView * pos;

	VS.color = vColor;
	VS.position = pos;
	VS.uv = vTexCoord;
}
)";

	static const std::string FS = R"(#version 430 core
out vec4 fragColor;

in DATA {
	vec4 color;
	vec4 position;
	vec2 uv;
} VS;

uniform sampler2D uTexture;

void main() {
	vec4 col = VS.color;
	col *= texture(uTexture, VS.uv);
	fragColor = col;
}
)";

	SpriteBatch::SpriteBatch(u32 width, u32 height) {
		VertexFormat fmt = VertexFormat(sizeof(Vertex))
			.add(2, DataType::TypeFloat)
			.add(2, DataType::TypeFloat)
			.add(4, DataType::TypeFloat);
		m_vao = VertexArray().create().bind();
		m_vbo = Buffer().create(Buffer::ArrayBuffer).bind();
		fmt.enable();
		m_ibo = Buffer().create(Buffer::ElementBuffer).bind();
		m_vao.unbind();

		m_defaultShader = Shader().create()
			.add(VS, Shader::VertexShader)
			.add(FS, Shader::FragmentShader)
			.link();
		m_currentShader = Shader(m_defaultShader.id());

		m_projection = ortho(0, width, height, 0, -1, 1);
		m_view = Matrix4();

		m_vertices.reserve(SpritesCount * 4);
		m_indices.reserve(SpritesCount * 6);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
	}

	SpriteBatch::~SpriteBatch() {
		m_defaultShader.destroy();
		m_vbo.destroy();
		m_ibo.destroy();
		m_vao.destroy();
	}

	void SpriteBatch::begin() {
		if (m_drawing) return;
		glDepthMask(false);
		m_currentShader.bind();
		setupMatrices();
		m_drawing = true;
	}

	void SpriteBatch::flush() {
		if (m_vertices.empty()) return;
		if (m_lastTexture.id()) m_lastTexture.bind(0);

		m_vbo.bind().update(m_vertices, Buffer::DynamicDraw);
		m_ibo.bind().update(m_indices, Buffer::DynamicDraw);

		m_vao.bind();
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
		m_vao.unbind();

		m_vertices.clear();
		m_indices.clear();
	}

	void SpriteBatch::end() {
		if (!m_drawing) return;
		if (!m_vertices.empty()) flush();
		m_lastTexture = Texture();
		m_drawing = false;
		glDepthMask(true);
		m_currentShader.unbind();
	}

	void SpriteBatch::setupMatrices() {
		m_currentShader.get("uProjView").set(m_projection * m_view, true);
		m_currentShader.get("uTexture").set(0);
	}

	void SpriteBatch::projectionMatrix(const Matrix4& v) {
		if (m_drawing) flush();
		m_projection = v;
		if (m_drawing) setupMatrices();
	}

	void SpriteBatch::viewMatrix(const Matrix4& v) {
		if (m_drawing) flush();
		m_view = v;
		if (m_drawing) setupMatrices();
	}

	void SpriteBatch::shader(const Shader& s) {
		if (m_drawing) {
			flush();
			m_currentShader.unbind();
		}
		m_currentShader = s.id() != 0 ? s : m_defaultShader;
		if (m_drawing) {
			m_currentShader.bind();
			setupMatrices();
		}
	}

	void SpriteBatch::switchTexture(const Texture& tex) {
		flush();
		m_lastTexture = tex;
	}

	void SpriteBatch::draw(const Texture& texture, Vector2 position, float rotation, Vector2 origin, Vector2 scale, Vector4 uv) {
		if (!m_drawing) flush();

		if (m_lastTexture.id() != texture.id()) {
			switchTexture(texture);
		} else if (m_vertices.size() / 4 >= SpritesCount) {
			flush();
		}

		const float ox = origin.x * m_lastTexture.width();
		const float oy = origin.y * m_lastTexture.height();
		const float wx = position.x + ox;
		const float wy = position.y + oy;
		float fx = -ox;
		float fy = -oy;
		float fx2 = m_lastTexture.width() - ox;
		float fy2 = m_lastTexture.height() - oy;

		fx *= scale.x;
		fy *= scale.y;
		fx2 *= scale.x;
		fy2 *= scale.y;

		const float p1x = fx;
		const float p1y = fy;
		const float p2x = fx;
		const float p2y = fy2;
		const float p3x = fx2;
		const float p3y = fy2;
		const float p4x = fx2;
		const float p4y = fy;

		float x1;
		float y1;
		float x2;
		float y2;
		float x3;
		float y3;
		float x4;
		float y4;

		// rotate
		if (rotation != 0) {
			const float cos = std::cos(rotation);
			const float sin = std::sin(rotation);

			x1 = cos * p1x - sin * p1y;
			y1 = sin * p1x + cos * p1y;

			x2 = cos * p2x - sin * p2y;
			y2 = sin * p2x + cos * p2y;

			x3 = cos * p3x - sin * p3y;
			y3 = sin * p3x + cos * p3y;

			x4 = x1 + (x3 - x2);
			y4 = y3 - (y2 - y1);
		} else {
			x1 = p1x;
			y1 = p1y;

			x2 = p2x;
			y2 = p2y;

			x3 = p3x;
			y3 = p3y;

			x4 = p4x;
			y4 = p4y;
		}

		x1 += wx;
		y1 += wy;
		x2 += wx;
		y2 += wy;
		x3 += wx;
		y3 += wy;
		x4 += wx;
		y4 += wy;

		float u1 = uv.x;
		float v1 = uv.y;
		float u2 = uv.x + uv.z;
		float v2 = uv.y + uv.w;

		u32 off = m_vertices.size();

		m_vertices.emplace_back(Vector2(x1, y1), Vector2(u1, v1), m_color);
		m_vertices.emplace_back(Vector2(x2, y2), Vector2(u1, v2), m_color);
		m_vertices.emplace_back(Vector2(x3, y3), Vector2(u2, v2), m_color);
		m_vertices.emplace_back(Vector2(x4, y4), Vector2(u2, v1), m_color);
		m_indices.insert(m_indices.end(), { off + 0, off + 1, off + 2, off + 0, off + 2, off + 3 });
	}

}