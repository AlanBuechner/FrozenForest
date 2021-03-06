#include <iostream>
#include <fstream>
#include <filesystem>

#include "Map\Map.h"

void IterateDirectory(Map& map, std::filesystem::path path)
{
	for (auto& p : std::filesystem::directory_iterator(path))
	{
		auto& path = p.path();

		if (p.is_directory())
		{
			IterateDirectory(map, path);
		}
		else if(path.extension().string() == ".json")
		{
			rapidjson::Document document;
			if (json::Load(path.string(), document))
				LoadMap(document, map);
			else
				std::cout << "could not load file \"" << path.string() << "\" not a valid josn file" << std::endl;
		}
		else
		{
			std::cout << "could not read file \"" << path.string() << "\" not a json file" << std::endl;
		}

	}
}


int main(int argc, char** argv)
{
	
	std::cout << std::filesystem::current_path() << std::endl;

	Map map;

	// load in file from rapidjson 
	IterateDirectory(map, "Map/");

	OptimizeMap(map);

	size_t size = 0;
	byte_t* data = nullptr;
	CompileMap(map, &data, &size);

	std::ofstream out("data/map.bin", std::ios::binary);
	out.write((const char*)data, size);
	out.close();

	return 0;
}