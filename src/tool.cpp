#include "tool.h"
namespace easy3d {
	std::string read_ShaderFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			std::cout << "read " << filename << " erroe" << std::endl;
			return std::string();
		}
		auto size = file.tellg();
		std::string content;
		content.resize(size);
		file.seekg(0);
		file.read(content.data(), content.size());
		return content;
	}

}
