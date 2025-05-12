/*****************************************************************************
  An abstraction for shaders in OpenGL.
  Only supports vertex and fragment shaders currently.

  Author(s): Evan O'Bryant
  Copyright © 2024-2025 Evan O'Bryant.    
*****************************************************************************/

#pragma once

#include <filesystem>
#include <string_view>
#include <print>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "ConceptExtensions.hpp"
#include "ClassConstructorMacros.hpp"

class Shader {
public:
	// Parses the two shader files and attempts to pass the data to OpenGL to convert into a valid
	// shader program.
	Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);
	~Shader();

	DEFAULT_COPY(Shader, delete);
	DEFAULT_MOVE(Shader, default);

	inline void Use() const { glUseProgram(programID); }

	// Sets a shader variable with the provided values of the same type.
	// Returns itself so a user can call .Set(...).Set(...).Set(...) etc
	template <cyber::arithmetic... Ts> requires cyber::all_same<Ts...>
	const Shader& Set(std::string_view valName, Ts... vals) const;

	template <glm::length_t L, cyber::arithmetic T, glm::qualifier Q>
	const Shader& Set(std::string_view vecName, const glm::vec<L, T, Q>& vec) const;

	template <glm::length_t C, glm::length_t R, cyber::arithmetic T, glm::qualifier Q>
	const Shader& Set(std::string_view matName, const glm::mat<C, R, T, Q>& mat, bool transpose = false) const;

	// More clear helper function for when user wants to set a sampler value.
	inline const Shader& SetSampler(std::string_view samplerName, int val) const { Set<int>(samplerName, val); return *this; }

	// Returns the location of a uniform variable in the shader.
	GLint GetUniformLocation(std::string_view uniformName) const;

private:
	GLuint programID = -1;
};

#include "Shader_Templates.cxx"
