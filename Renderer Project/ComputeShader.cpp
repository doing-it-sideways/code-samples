#include "ComputeShader.hpp"
#include "ShaderParser.hpp"

namespace cyber {

ComputeShader::ComputeShader(const std::filesystem::path& compPath)
	: Shader()
{
	GLuint compShader = ShaderCompile(GL_COMPUTE_SHADER, ShaderFileAsString(compPath));

	programID = glCreateProgram();
	ShaderLink(programID, { compShader });

	glGetProgramiv(programID, GL_COMPUTE_WORK_GROUP_SIZE, &workGroupSize.x);

	glDeleteShader(compShader);
}

ComputeShader::~ComputeShader() {
	glDeleteProgram(programID);
}

} // namespace cyber
