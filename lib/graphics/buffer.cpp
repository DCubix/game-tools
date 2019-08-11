#include "buffer.h"

#include <iostream>

namespace gt {
	void Buffer::destroy() {
		if (m_id) {
			glDeleteBuffers(1, &m_id);
			m_id = 0;
		}
	}

	Buffer& Buffer::create(BufferType type) {
		m_type = type;
		glGenBuffers(1, &m_id);
		return *this;
	}

	Buffer& Buffer::bind() {
		glBindBuffer(GLenum(m_type), m_id);
		return *this;
	}

	Buffer& Buffer::unbind() {
		glBindBuffer(GLenum(m_type), 0);
		return *this;
	}

	Buffer& Buffer::bindBase(u32 bindingPoint) {
		glBindBufferBase(GLenum(m_type), bindingPoint, m_id);
		return *this;
	}

	void Buffer::unmap() {
		glUnmapBuffer(GLenum(m_type));
	}

	VertexArray& VertexArray::create() {
		glGenVertexArrays(1, &m_id);
		return *this;
	}

	VertexArray& VertexArray::build() {
		for (auto&& buf : m_buffers) buf.bind();
		m_format.enable();
		return *this;
	}

	VertexArray& VertexArray::bind() {
		glBindVertexArray(m_id);
		return *this;
	}

	VertexArray& VertexArray::unbind() {
		glBindVertexArray(0);
		return *this;
	}

	VertexArray& VertexArray::add(const Buffer& buf) {
		m_buffers.push_back(buf);
		return *this;
	}

	VertexArray& VertexArray::format(VertexFormat format) {
		m_format = format;
		return *this;
	}
}