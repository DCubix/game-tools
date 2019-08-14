#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <map>
#include <string>

#include "../stl.hpp"
#include "../glad/glad.h"

namespace gt {
	class Buffer {
	public:
		enum BufferType {
			ArrayBuffer = GL_ARRAY_BUFFER,
			ElementBuffer = GL_ELEMENT_ARRAY_BUFFER,
			UniformBuffer = GL_UNIFORM_BUFFER,
			ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER
		};

		enum BufferUsage {
			StaticDraw = GL_STATIC_DRAW,
			StaticCopy = GL_STATIC_COPY,
			StaticRead = GL_STATIC_READ,
			DynamicDraw = GL_DYNAMIC_DRAW,
			DynamicCopy = GL_DYNAMIC_COPY,
			DynamicRead = GL_DYNAMIC_READ,
			StreamDraw = GL_STREAM_DRAW,
			StreamCopy = GL_STREAM_COPY,
			StreamRead = GL_STREAM_READ,
		};

		enum BufferAccess {
			AccessReadOnly = GL_READ_ONLY,
			AccessWriteOnly = GL_WRITE_ONLY,
			AccessReadWrite = GL_READ_WRITE
		};

		Buffer() = default;
		~Buffer() = default;

		Buffer& create(BufferType type);
		void destroy();

		Buffer& bind();
		Buffer& unbind();

		Buffer& bindBase(u32 bindingPoint);

		template <typename DataType>
		inline Buffer& update(const std::vector<DataType>& data, BufferUsage usage = StaticDraw, i32 offset = 0) {
			const size_t size = sizeof(DataType) * data.size();
			if (size > m_size) {
				glBufferData(GLenum(m_type), size, data.data(), GLenum(usage));
				m_size = size;
				m_usage = usage;
			} else {
				glBufferSubData(GLenum(m_type), offset, size, data.data());
			}
			return *this;
		}

		template <typename DataType>
		inline DataType* map(BufferAccess access = AccessReadWrite) {
			return (DataType*) glMapBuffer(GLenum(m_type), GLenum(access));
		}

		void unmap();

		GLuint id() const { return m_id; }
		BufferType type() const { return m_type; }
		BufferUsage usage() const { return m_usage; }
		u32 size() const { return m_size; }

	private:
		GLuint m_id{ 0 };
		BufferType m_type;
		BufferUsage m_usage;
		u32 m_size{ 0 };
	};

	enum DataType {
		TypeByte = GL_BYTE,
		TypeUByte = GL_UNSIGNED_BYTE,
		TypeShort = GL_SHORT,
		TypeUShort = GL_UNSIGNED_SHORT,
		TypeInt = GL_INT,
		TypeUInt = GL_UNSIGNED_INT,
		TypeFloat = GL_FLOAT,
		TypeHalfFloat = GL_HALF_FLOAT,
		TypeFixed = GL_FIXED
	};

	class VertexFormat {
	public:
		struct Field {
			DataType type;
			u8 size;
			bool normalized;
		};

		using FieldList = std::vector<Field>;

		VertexFormat() = default;
		VertexFormat(size_t stride) : m_stride(stride) {}
		~VertexFormat() = default;

		inline VertexFormat& add(u8 size, DataType type, bool normalized = false) {
			Field field;
			field.type = type;
			field.size = size;
			field.normalized = normalized;
			m_fields.push_back(field);
			return *this;
		}

		inline void enable() {
			u32 off = 0, i = 0;
			for (auto&& field : m_fields) {
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, field.size, field.type, field.normalized, m_stride, reinterpret_cast<void*>(off));
				i++;
				off += getDataTypeSize(field.type) * field.size;
			}
		}

		inline void disable() {
			for (u32 i = 0; i < m_fields.size(); i++) {
				glDisableVertexAttribArray(i);
			}
		}

	private:
		size_t m_stride;
		FieldList m_fields;

		inline size_t getDataTypeSize(DataType type) {
			switch (type) {
				case DataType::TypeUByte:
				case DataType::TypeByte: return 1;
				case DataType::TypeUShort:
				case DataType::TypeShort: return 2;
				case DataType::TypeUInt:
				case DataType::TypeInt: return 4;
				case DataType::TypeFloat: return 4;
				case DataType::TypeHalfFloat: return 2;
				case DataType::TypeFixed: return 4;
				default: return 1;
			}
		}
	};

	class VertexArray {
	public:
		VertexArray() = default;
		~VertexArray() = default;

		VertexArray& create();
		void destroy();
		VertexArray& bind();
		VertexArray& unbind();

	private:
		GLuint m_id;
	};
}

#endif // BUFFER_H