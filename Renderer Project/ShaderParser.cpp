#include "ShaderParser.hpp"

#include <fstream>
#include <sstream>
#include <print>

namespace cyber {

std::string ShaderFileAsString(const std::filesystem::path& shaderPath) {
	if (!std::filesystem::exists(shaderPath)) {
		std::println(stderr, "Shader file at {} doesn't exist", shaderPath.string());
		return "";
	}

	std::ifstream file{ shaderPath };

	if (!file.is_open()) {
		std::println(stderr, "shader file at {} invalid!", shaderPath.string());
		return "";
	}

	std::stringstream buf;
	buf << file.rdbuf();
	return buf.str();
}

GLuint ShaderCompile(GLenum type, const std::string& shader) {
	GLuint shaderId = glCreateShader(type);

	auto src = shader.c_str();
	glShaderSource(shaderId, 1, &src, nullptr);
	glCompileShader(shaderId);

	int ok;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);

	if (!ok) {
		char errInfo[512]{};
		glGetShaderInfoLog(shaderId, 512, nullptr, errInfo);

		std::string_view typeName;
		switch (type) {
		case GL_VERTEX_SHADER:
			typeName = "Vertex";
			break;
		case GL_FRAGMENT_SHADER:
			typeName = "Fragment";
			break;
		case GL_COMPUTE_SHADER:
			typeName = "Compute";
			break;
		default:
			typeName = "Unknown";
			break;
		}

		std::println(stderr, "{} shader failed compilation: {}",
					 typeName, errInfo);
	}

	return shaderId;
}

void ShaderLink(GLuint shaderProgram, std::initializer_list<GLuint> shaders) {
	for (GLuint shader : shaders) {
		glAttachShader(shaderProgram, shader);
	}

	glLinkProgram(shaderProgram);

	int ok;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);

	if (!ok) {
		char errInfo[512]{};
		glGetProgramInfoLog(shaderProgram, 512, NULL, errInfo);
		std::println(stderr, "Shader program failed linkage: {}", errInfo);
	}
}

} // namespace cyber
