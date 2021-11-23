#include <iostream>
#include <fstream>
#include <filesystem>
#include "Map\Map.h"

#include <stb_image.h>


int main(int argc, char** argv)
{
	
	std::cout << std::filesystem::current_path() << std::endl;

	Map map;
	
	Mesh mesh;
	mesh.quads = true;
	mesh.vertices = std::vector<Vertex>{
		{ {-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f} },
		{ { 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f} },
		{ { 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f} },
		{ {-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f} },
	};

	mesh.indices = std::vector<uint16_t> {
		0,1,2,3
	};

	map.meshSection.meshes.push_back(mesh);

	map.tileSection.tiles.push_back({0, 0});
	map.tileSection.tiles.push_back({0, 1});
	map.tileSection.tiles.push_back({0, 2});
	map.tileSection.tiles.push_back({0, 3});

	Chunk chunk;
	chunk.x = 0;
	chunk.y = 0;

	for (uint8_t x = 0; x < CHUNK_SIZE; x++)
		for (uint8_t y = 0; y < CHUNK_SIZE; y++)
			chunk.layers[0].tiles[x][y] = ((x+(y%2)) % 2);

	for (uint8_t x = 0; x < CHUNK_SIZE; x++)
		for (uint8_t y = 0; y < CHUNK_SIZE; y++)
			chunk.layers[0].height[x][y] = 0;

	/*chunk.layers[0].height[0][0] = 1;
	chunk.layers[0].height[3][7] = 1;
	chunk.layers[0].height[5][3] = 1;
	chunk.layers[0].height[1][6] = 1;
	chunk.layers[0].height[9][6] = 1;
	chunk.layers[0].height[5][4] = 1;*/

	for (uint8_t c = 1; c < 3; c++)
		for (uint8_t x = 0; x < CHUNK_SIZE; x++)
			for (uint8_t y = 0; y < CHUNK_SIZE; y++)
				chunk.layers[c].tiles[x][y] = 0;

	for (uint8_t x = 0; x < CHUNK_SIZE; x++)
		for (uint8_t y = 0; y < CHUNK_SIZE; y++)
			chunk.collisonMask[x][y] = 0xcccccccc;

	map.chunkSection.chunks.push_back(chunk);

	for (uint8_t x = 0; x < CHUNK_SIZE; x++)
		for (uint8_t y = 0; y < CHUNK_SIZE; y++)
			chunk.layers[0].tiles[x][y] = ((x + (y % 2)+1) % 2);

	map.chunkSection.chunks.push_back(chunk);

	// load in file from rapidjson 
	rapidjson::Document document;
	bool success = json::Load("test.txt", document);
	//assert(success); 
	//LoadMap(document); 


	size_t size = 0;
	byte_t* data = nullptr;
	CompileMap(map, &data, &size);

	std::ofstream out("data/map.bin", std::ios::binary);

	out.write((const char*)data, size);

	out.close();

	return 0;
}