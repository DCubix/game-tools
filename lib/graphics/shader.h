#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

#include "../math/math.hpp"
#include "../glad/glad.h"
#include "../stl.hpp"

namespace gt {
	class Shader {
	public:
		enum ShaderType {
			VertexShader = GL_VERTEX_SHADER,
			FragmentShader = GL_FRAGMENT_SHADER,
			GeometryShader = GL_GEOMETRY_SHADER,
			ComputeShader = GL_COMPUTE_SHADER
		};

		enum ProgramInterface {
			ShaderStorageBlock,
			UniformBufferBlock
		};

		struct Uniform {
			u32 loc;
			void set(i32 v);
			void set(float v);
			void set(const Vector2& v);
			void set(const Vector3& v);
			void set(const Vector4& v);
			void set(const Matrix2& m, bool transpose = false);
			void set(const Matrix3& m, bool transpose = false);
			void set(const Matrix4& m, bool transpose = false);
		};

		using ShaderMap = std::unordered_map<ShaderType, GLuint>;
		using ValueMap = std::unordered_map<std::string, GLuint>;

		Shader() = default;
		~Shader() = default;

		Shader& create();
		void destroy();

		Shader& bind();
		Shader& unbind();

		Shader& add(const std::string& source, ShaderType type);
		Shader& link();

		i32 getBlockIndex(ProgramInterface interface, const std::string& name);
		i32 getUniformIndex(const std::string& name);
		i32 getAttributeIndex(const std::string& name);

		void uniformBlockBinding(u32 blockIndex, u32 binding);
		void storageBlockBinding(u32 blockIndex, u32 binding);

		Uniform get(const std::string& name);

		GLuint id() const { return m_id; }

	private:
		GLuint m_id{ 0 };
		ShaderMap m_subShaders;
		ValueMap m_attributes, m_uniforms, m_blockIndices;
	};
}

#endif // SHADER_H