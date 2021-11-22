#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include <stb_image.h>

/*
header:
	uint32_t chunk section offset
	uint32_t tile section offset
	uint32_t mesh section offset

// all chunks are a 10x10 regen of tiles
// all chuncks are ordered by there y position then there x position
chunk section:
	uint32_t number of chunks
	chunks[]:
		- 1
			uint16_t x, y
			uint32_t tiles[10,10] // index into the tiles array
			uint32_t collisionMap[10][10]

tile section:
	uint32_t number of tiles
	tile[]:
		- 1
			uint32_t mesh id
			uint8_t data // rotation

mesh section:
	uint32_t number of meshes
	meshLookup[] // offset from file base for each mesh
	mesh[]:
		- 1
			bool quads
			uint32_t vertex count
			uint32_t index count
			float[] vertices
			uint32_t[] indices


*/

using byte_t = unsigned char;


#define CHUNK_SIZE 9

struct Header
{
	uint32_t chunkSectionOffset;
	uint32_t tileSectionOffset;
	uint32_t meshSectionOffset;
	uint32_t textureSectionOffset;
};

struct Layer
{
	uint32_t tiles[CHUNK_SIZE][CHUNK_SIZE];
	uint32_t height[CHUNK_SIZE][CHUNK_SIZE];
};

struct Chunk
{
	uint16_t x, y;
	Layer layers[3];
	uint32_t collisonMask[CHUNK_SIZE][CHUNK_SIZE];
};

struct ChunkSection
{
	std::vector<Chunk> chunks;
};

struct Tile
{
	uint32_t meshId;
	uint8_t rotation; 
};

struct TileSection
{
	std::vector<Tile> tiles;
};

struct Vec3 
{
	float x, y, z;
};

struct Vec2
{
	float x, y;
};

struct Vertex
{
	Vec3 position;
	Vec2 uv;
};

struct Mesh
{
	bool quads;
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
};

struct MeshSection
{
	std::vector<Mesh> meshes;
};


struct Map
{
	Header header;
	ChunkSection chunkSection;
	TileSection tileSection;
	MeshSection meshSection;

};

size_t HeaderAllocationSize(Map& map);
size_t ChunkAllocationSize(Map& map);
size_t TileAllocationSize(Map& map);
size_t MeshAllocationSize(Map& map);

size_t MapAllocationSize(Map& map);

void CompileMap(Map& map, byte_t** data, size_t* size);