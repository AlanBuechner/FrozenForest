#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>
#include "stb_image\stb_image.h"

#define PACK_PIXEL(r, g, b, a) ((((a)<<15)&0b1000000000000000) | (((b)<<11)&0b0111100000000000) | (((g)<<6)&0b0000011111000000) | (((r)<<1)&0b0000000000111110) )

struct Texture
{
	std::string name;
	uint32_t count;
	stbi_uc* data;
};

std::vector<Texture> textures;

void IterateDirectory(std::filesystem::path path)
{
	for (auto& p : std::filesystem::directory_iterator(path))
	{
		auto& path = p.path();

		if (p.is_directory())
		{
			IterateDirectory(path);
		}
		else if (path.extension().string() == ".png")
		{

			Texture t;
			t.name = path.stem().string();
			int x, y;
			t.data = stbi_load(path.string().c_str(), &x, &y, nullptr, 4);
			t.count = x * y;

			textures.push_back(t);
		}
		else
		{
			std::cout << "could not read file \"" << path.string() << "\" not a json file" << std::endl;
		}

	}
}

int main(int argc, char** argv)
{
	IterateDirectory("Textures");

	for (auto& t : textures)
	{
		uint16_t* buffer = new uint16_t[t.count];
		for (uint32_t i = 0; i < t.count; i++)
		{
			uint16_t r = ((float)t.data[i*4+0]/255.0f)*31;
			uint16_t g = ((float)t.data[i*4+1]/255.0f)*31;
			uint16_t b = ((float)t.data[i*4+2]/255.0f)*31;
			uint16_t a = ((float)t.data[i*4+3]/255.0f);

			buffer[i] = PACK_PIXEL(r, g, b, a);
		}

		std::ofstream out("data/"+t.name+".bin", std::ios::binary);
		out.write((const char*)buffer, t.count*2);
		out.close();

		delete[] buffer;
	}

	return 0;
}