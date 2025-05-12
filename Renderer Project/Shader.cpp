/*****************************************************************************
  Implementation of shader abstraction.

  Author(s): Evan O'Bryant
  Copyright © 2024-2025 Evan O'Bryant.    
*****************************************************************************/

#include "Shader.hpp"
#include "ShaderParser.hpp"

// Initially, I didn't have a shader class, so I just call the functions I had previously made in
// the shader parser.
Shader::Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath) {
	GLuint vertShader = ShaderCompile(GL_VERTEX_SHADER, ShaderFileAsString(vertPath));
	GLuint fragShader = ShaderCompile(GL_FRAGMENT_SHADER, ShaderFileAsString(fragPath));

	programID = glCreateProgram();
	ShaderLink(programID, vertShader, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

Shader::~Shader() {
	glDeleteProgram(programID);
}

GLint Shader::GetUniformLocation(std::string_view uniformName) const {
	GLint loc = glGetUniformLocation(programID, uniformName.data());
	
	if (loc == -1)
		std::println(stderr, "location of {} not found", uniformName);

	return loc;
}
