#pragma once

#include <filesystem>
#include <glm/vec3.hpp>

#include "Shader.hpp"

namespace cyber {

class ComputeShader : public Shader {
public:
	explicit ComputeShader(const std::filesystem::path& compPath);
	~ComputeShader();

	DEFAULT_COPY(ComputeShader, delete);
	DEFAULT_MOVE(ComputeShader, default);

	inline auto& GetWorkGroupSize() const { return workGroupSize; }

private:
	glm::ivec3 workGroupSize{};
};

} // namespace cyber
