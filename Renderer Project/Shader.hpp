#pragma once

#include <filesystem>
#include <string_view>
#include <print>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "ConceptExtensions.hpp"
#include "ClassConstructorMacros.hpp"

namespace cyber {

class Shader {
public:
	Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);
	~Shader();

	DEFAULT_COPY(Shader, delete);
	DEFAULT_MOVE(Shader, default);

	inline void Use() const { glUseProgram(programID); }

	template <cyber::arithmetic... UniformVals> requires cyber::all_same<UniformVals...>
	const Shader& Set(std::string_view valName, UniformVals... vals) const;

	template <glm::length_t L, cyber::arithmetic UniformVal, glm::qualifier Q>
	const Shader& Set(std::string_view vecName, const glm::vec<L, UniformVal, Q>& vec) const;

	template <glm::length_t C, glm::length_t R, cyber::arithmetic UniformVal, glm::qualifier Q>
	const Shader& Set(std::string_view matName, const glm::mat<C, R, UniformVal, Q>& mat, bool transpose = false) const;

	inline const Shader& SetSampler(std::string_view samplerName, int val) const { Set<int>(samplerName, val); return *this; }

	GLint GetUniformLocation(std::string_view uniformName) const;

protected:
	Shader() = default;

protected:
	GLuint programID = -1;
};

} // namespace cyber

#include "Shader_Templates.cxx"
