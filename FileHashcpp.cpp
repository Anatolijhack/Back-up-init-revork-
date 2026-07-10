#include "FileHash.h"
#include <fstream>
namespace fs = std::filesystem;
std::size_t hashFile(const fs::path& path)
{
	std::ifstream file(path,std::ios::binary);

	std::size_t hash = 0;
	char buffer[4096];
	while (file.read(buffer, sizeof(buffer)) || file.gcount())
	{
		for (size_t i = 0; i < file.gcount(); i++)
		{
			hash ^= std::hash<char>{}(buffer[i]);
		}
	}
	return hash;
}
bool isModified(const fs::path& a, const fs::path& b)
{
	if (!fs::exists(b))
	{
		return true;
	}
	return hashFile(a) != hashFile(b);
}