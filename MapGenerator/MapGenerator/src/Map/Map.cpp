#include "Map.h"
#include "../Json.h"
#include "rapidjson.h"

#include <algorithm>

typedef short int v16;
#define floattov16(n)        ((v16)((n) * (1 << 12))) /*!< \brief convert float to v16 */

typedef short t16;
#define floattot16(n)        ((t16)((n) * (1 << 4)))


struct Vec3v16
{
	v16 x, y, z;
};

struct Vec2t16
{
	t16 x, y;
};

struct Vert
{
	Vec3v16 position;
	Vec2t16 uv;
};

template <typename T>
T swap_endian(T u)
{
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

size_t HeaderAllocationSize(Map& map)
{
	return sizeof(map.header);
}

size_t ChunkAllocationSize(Map& map)
{
	size_t size = 0;
	size += sizeof(uint32_t); // number of chunks
	size += sizeof(uint32_t); // xExtent
	size += map.chunkSection.chunks.size() * sizeof(Chunk); // chunk list
	return size;
}

size_t TileAllocationSize(Map& map)
{
	size_t size = 0;
	size += sizeof(uint32_t);
	size += map.tileSection.tiles.size() * sizeof(Tile);
	return size;
}

size_t MeshAllocationSize(Map& map)
{
	size_t size = 0;
	size += sizeof(uint32_t);
	size += map.meshSection.meshes.size() * sizeof(uint32_t);
	for (auto& mesh : map.meshSection.meshes)
	{
		size_t meshSize = 0;
		meshSize += 2*sizeof(bool); // quad bool
		meshSize += sizeof(uint16_t); // nuber of vertices
		meshSize += sizeof(uint16_t); // nuber of indices

		meshSize += mesh.vertices.size() * sizeof(Vert);
		meshSize += mesh.indices.size() * sizeof(uint16_t);

		meshSize += meshSize % 4;

		size += meshSize;
	}
	return size;
}

size_t MapAllocationSize(Map& map)
{
	size_t size = 0;
	size += HeaderAllocationSize(map); // header

	// chunk section
	size += ChunkAllocationSize(map);

	// tile section
	size += TileAllocationSize(map);

	// mesh section
	size += MeshAllocationSize(map);

	return size;
}

void CompileMap(Map& map, byte_t** data, size_t* size)
{
	// find offet of chunk section
	size_t offset = HeaderAllocationSize(map);
	map.header.chunkSectionOffset = offset;

	// find offset of tile section
	offset += ChunkAllocationSize(map);
	map.header.tileSectionOffset = offset;

	// find offet of mesh section
	offset += TileAllocationSize(map);
	map.header.meshSectionOffset = offset;

	// find size of map allocation and alocate memory
	offset += MeshAllocationSize(map);
	*data = (byte_t*)malloc(offset);
	*size = offset;
	byte_t* d = *data;

	// write map header
	*(Header*)(d) = map.header;

	// write chunk section
	*(uint32_t*)(d + map.header.chunkSectionOffset) = map.chunkSection.chunks.size(); // write nuber of chunks
	uint32_t& xExtent = *(uint32_t*)(d + map.header.chunkSectionOffset + sizeof(uint32_t)) = 0;
	for (uint32_t i = 0; i < map.chunkSection.chunks.size(); i++)
	{
		// find offset of this chunk
		uint32_t offset = map.header.chunkSectionOffset + (2*sizeof(uint32_t)) + (i * sizeof(Chunk));
		*(Chunk*)(d + offset) = map.chunkSection.chunks[i]; // write chunk

		if (map.chunkSection.chunks[i].x > xExtent)
			xExtent = map.chunkSection.chunks[i].x;
	}

	// write tile section
	*(uint32_t*)(d + map.header.tileSectionOffset) = map.tileSection.tiles.size(); // write nuber of tiles
	for (uint32_t i = 0; i < map.tileSection.tiles.size(); i++)
	{
		uint32_t offset = map.header.tileSectionOffset + sizeof(uint32_t) + (i * sizeof(Tile));
		*(Tile*)(d + offset) = map.tileSection.tiles[i];
	}

	// write mesh section
	*(uint32_t*)(d + map.header.meshSectionOffset) = map.meshSection.meshes.size(); // number of meshes
	uint32_t* meshLookup = (uint32_t*)(d + map.header.meshSectionOffset + sizeof(uint32_t));
	uint32_t meshOffset = map.header.meshSectionOffset + sizeof(uint32_t) + (map.meshSection.meshes.size() * sizeof(uint32_t));
	for (uint32_t i = 0; i < map.meshSection.meshes.size(); i++)
	{
		// curent mesh
		Mesh& mesh = map.meshSection.meshes[i];

		meshLookup[i] = meshOffset;

		// write if it is quads
		*(bool*)(d + meshOffset) = mesh.quads;
		meshOffset += 2*sizeof(bool);

		// write nuber of verts and indices
		*(uint16_t*)(d + meshOffset) = (uint16_t)mesh.vertices.size();
		meshOffset += sizeof(uint16_t);
		*(uint16_t*)(d + meshOffset) = (uint16_t)mesh.indices.size();
		meshOffset += sizeof(uint16_t);

		// write vertices
		for (uint16_t v = 0; v < mesh.vertices.size(); v++)
		{
			Vert vert;
			vert.position.x = floattov16(mesh.vertices[v].position.x);
			vert.position.y = floattov16(mesh.vertices[v].position.y);
			vert.position.z = floattov16(mesh.vertices[v].position.z);

			vert.uv.x = floattot16(mesh.vertices[v].uv.x);
			vert.uv.y = floattot16(mesh.vertices[v].uv.y);

			*(Vert*)(d + meshOffset + (v * sizeof(Vert))) = vert;

		}
		meshOffset += mesh.vertices.size() * sizeof(Vert);

		// write indices
		for (uint16_t j = 0; j < mesh.indices.size(); j++)
			*(uint16_t*)(d + meshOffset + (j * sizeof(uint16_t))) = mesh.indices[j];
		meshOffset += mesh.indices.size() * sizeof(uint16_t);

		meshOffset += meshOffset % 4;
		
	}

}

void LoadMap(const rapidjson::Value& value, Map& map)
{
	// read map into data structure
	// chunks
	if (value.HasMember("Chunks") && value["Chunks"].IsArray())
	{
		auto chunks = value["Chunks"].GetArray(); 
		for (uint32_t c = 0; c < chunks.Size(); c++)
		{
			Chunk chunk; 
			json::Get(chunks[c], "x", chunk.x);
			json::Get(chunks[c], "y", chunk.y);

			// add in error checking for layers, should always be only 3 layers 
			auto layers = chunks[c]["Layers"].GetArray(); 
			if (layers.Size() == 3)
			{
				for (uint32_t l = 0; l < layers.Size(); l++)
				{
					auto tiles = layers[l]["Tiles"].GetArray();
					if(tiles.Size() == 0)
						for (uint32_t t = 0; t < CHUNK_SIZE * CHUNK_SIZE; t++)
							chunk.layers[l].tiles[t / CHUNK_SIZE][t % CHUNK_SIZE] = 0;
					else
					for (uint32_t t = 0; t < tiles.Size(); t++)
						chunk.layers[l].tiles[t / CHUNK_SIZE][t % CHUNK_SIZE] = tiles[t].GetInt(); 

					auto height = layers[l]["Height"].GetArray(); 
					if(height.Size() == 0)
						for (uint32_t h = 0; h < CHUNK_SIZE * CHUNK_SIZE; h++)
							chunk.layers[l].height[h / CHUNK_SIZE][h % CHUNK_SIZE] = 0;
					else
					for (uint32_t h = 0; h < height.Size(); h++)
						chunk.layers[l].height[h / CHUNK_SIZE][h % CHUNK_SIZE] = height[h].GetInt(); 
				}
			}

			if (chunks[c].HasMember("Collision") && chunks[c]["Collision"].IsArray())
			{
				auto collision = chunks[c]["Collision"].GetArray();
				if (collision.Size() == 0)
					for (uint32_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
						chunk.collisonMask[i / CHUNK_SIZE][i % CHUNK_SIZE] = 0;
				else
					for (uint32_t i = 0; i < collision.Size(); i++)
						chunk.collisonMask[i / CHUNK_SIZE][i % CHUNK_SIZE] = collision[i].GetInt();

			}

			map.chunkSection.chunks.push_back(chunk);
		}
	}

	// tiles
	if (value.HasMember("Tiles") && value["Tiles"].IsArray())
	{
		auto tiles = value["Tiles"].GetArray();
		for (uint32_t t = 0; t < tiles.Size(); t++)
		{
			Tile tile;
			json::Get(tiles[t], "MeshID", tile.meshId);
			json::Get(tiles[t], "Texture", tile.textureId);
			json::Get(tiles[t], "Rotation", tile.rotation);

			map.tileSection.tiles.push_back(tile);
		}
	}

	// mesh 
	if (value.HasMember("Meshes") && value["Meshes"].IsArray())
	{
		auto meshes = value["Meshes"].GetArray();
		for (uint32_t m = 0; m < meshes.Size(); m++)
		{

			Mesh mesh;
			json::Get(meshes[m], "Quads", mesh.quads);

			auto verts = meshes[m]["Vertices"].GetArray();
			mesh.vertices.resize(verts.Size() / 5);
			for (uint32_t v = 0; v < verts.Size(); v++)
				((float*)mesh.vertices.data())[v] = verts[v].GetFloat();
			
			auto indices = meshes[m]["Indices"].GetArray();
			mesh.indices.resize(indices.Size());
			for (uint32_t i = 0; i < indices.Size(); i++)
				mesh.indices[i] = indices[i].GetInt();

			map.meshSection.meshes.push_back(mesh);
		}
	}
}

void OptimizeMap(Map& map)
{
	uint32_t xExtent = 0;
	for (auto& c : map.chunkSection.chunks)
	{
		if (c.x > xExtent)
			xExtent = c.x;
	}

	std::sort(map.chunkSection.chunks.begin(), map.chunkSection.chunks.end(), [&xExtent](Chunk a, Chunk b)
		{
			int ai = a.y * xExtent + a.x;
			int bi = b.y * xExtent + b.x;
			return ai < bi;
		}
	);

}
