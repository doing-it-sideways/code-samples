/*****************************************************************************
  Abstraction for reading in shader data from a file, compiling, and linking shaders.

  Author(s): Evan O'Bryant
  Copyright © 2024-2025 Evan O'Bryant.    
*****************************************************************************/

#pragma once

#include <string>
#include <filesystem>

#include <glad/gl.h>

// Converts file data into a string.
std::string ShaderFileAsString(const std::filesystem::path& shaderPath);

// Compiles a shader (given with a string) and outputs the id for the compiled shader.
GLuint ShaderCompile(GLenum type, const std::string& shader);

// Links a vertex shader and a fragment shader together, storing it in a shader program.
void ShaderLink(GLuint shaderProgram, GLuint vertexShader, GLuint fragShader);