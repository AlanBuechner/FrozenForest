#ifndef MAP_H
#define MAP_H

#include "map_bin.h"
#include <vector>

#define CHUNK_SIZE 9

typedef short int v16;
typedef short t16;
using byte_t = unsigned char;

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

struct Tile
{
	uint32_t meshId;
	uint8_t rotation;
};

struct Vec3 
{
	v16 x, y, z;
};

struct Vec2
{
	t16 x, y;
};

struct Vertex
{
	Vec3 position;
	Vec2 uv;
};

struct Mesh
{
    bool quds = false;

    uint16_t vertCount;
    uint16_t indexCount;
    Vertex* verts;
    uint16_t* indices;
};

struct Texture
{
	int width, height;
	byte_t* data;
};

inline const Header& GetHeader() { return *(const Header*)map_bin; }

inline const void* GetChunkSection() { return (void*)((size_t)map_bin + (size_t)GetHeader().chunkSectionOffset); }
inline const void* GetTileSection() { return (void*)((size_t)map_bin + (size_t)GetHeader().tileSectionOffset); }
inline const void* GetMeshSection() { return (void*)((size_t)map_bin + (size_t)GetHeader().meshSectionOffset); }
inline const uint32_t* GetMeshLookupSection() { return (uint32_t*)((size_t)map_bin + (size_t)GetHeader().meshSectionOffset + sizeof(uint32_t)); }
inline const void* GetTextureSection() { return (void*)((size_t)map_bin + (size_t)GetHeader().textureSectionOffset); }
inline const uint32_t* GetTextureLookupSection() { return(uint32_t*)((size_t)map_bin + (size_t)GetHeader().textureSectionOffset + sizeof(uint32_t)); }

inline size_t MapAllicationSize() { return map_bin_size; }

inline uint32_t GetNumberOfTextures() { return *(uint32_t*)GetTextureSection(); }
inline Texture* GetTextureLocation(uint32_t textureID) { return (Texture*)((size_t)map_bin + (size_t)GetTextureLookupSection()[textureID]); }
Texture GetTexture(uint32_t textureID);

inline uint32_t GetNumberOfMeshes() { return *(uint32_t*)GetMeshSection(); }
inline Mesh* GetMeshLocation(uint32_t meshID) { return (Mesh*)((size_t)map_bin + (size_t)GetMeshLookupSection()[meshID]); }
Mesh GetMesh(uint32_t meshID);

inline uint32_t GetNumberOfTiles() { return *(uint32_t*)GetTileSection(); }
inline const Tile& GetTile(uint32_t tileID) { return *(Tile*)((size_t)GetTileSection() + sizeof(uint32_t) + (tileID*sizeof(Tile))); }

inline uint32_t GetNumberOfChunks() { return *(uint32_t*)GetChunkSection(); }
inline uint32_t GetChunkXExtent() { return *(uint32_t*)((size_t)GetChunkSection() + sizeof(uint32_t)); }
inline const Chunk& GetChunk(uint32_t chunkID) { return *(Chunk*)((size_t)GetChunkSection() + (2*sizeof(uint32_t)) + (chunkID*sizeof(Chunk))); }

#endif