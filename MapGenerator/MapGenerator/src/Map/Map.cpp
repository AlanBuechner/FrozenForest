#include "Map.h"
#include "../Json.h"
#include "rapidjson.h"

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
		size += 2*sizeof(bool); // quad bool
		size += sizeof(uint16_t); // nuber of vertices
		size += sizeof(uint16_t); // nuber of indices

		size += mesh.vertices.size() * sizeof(Vert);
		size += mesh.indices.size() * sizeof(uint16_t);
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
		
	}

}

Map LoadMap(const rapidjson::Value& value)
{
	Map map;

	// read map into data structure (chunk section)
	if (value.HasMember("Chunks") && value["Chunks"].IsArray())
	{
		auto chunks = value["Chunks"].GetArray(); 
		for (int c = 0; c < chunks.Size(); c++)
		{
			Chunk chunk; 
			JSON_READ(chunks[c]["x"], chunk.x);
			JSON_READ(chunks[c]["y"], chunk.y);

			// add in error checking for layers, should always be only 3 layers 
			auto layers = chunks[c]["Layers"].GetArray(); 
			if (layers.Size() == 3)
			{
				for (int l = 0; l < layers.Size(); l++)
				{
					auto tiles = chunks[c]["Layers"].GetArray(); 
					for (int t = 0; t < layers[l].Size(); t++)
					{
						JSON_READ(tiles[t], chunk.layers[l].tiles[t / CHUNK_SIZE][t % CHUNK_SIZE]); 
					}

					auto height = layers[l]["Height"].GetArray(); 
					for (int h = 0; h < height.Size(); h++)
					{
						JSON_READ(height[h], chunk.layers[l].height[h / CHUNK_SIZE][h % CHUNK_SIZE]); 
					}
				}
			}
		}
	}

	// stuff for Tiles 
	if (value.HasMember("Tile"))
	{
		Tile tile; 
		JSON_READ(value, tile.meshId); 
		JSON_READ(value, tile.rotation); 
	}

	// mesh 
	if (value.HasMember("Mesh"))
	{
		Mesh mesh; 
		JSON_READ(value, mesh.quads); 

		for (int v = 0; v < mesh.vertices.size(); v++)
		{
		//		for all of the positions (x, y, and z) 
		//		for all of the uvs (x and y)
		}
		//JSON_READ(value, mesh.vertices); 
		 
		// not entirely sure on this one 
		//JSON_READ(value, mesh.indices); 
	}

	// these don't exist yet so do later 
	// textures 

	// interactable items 

	if (value.HasMember("Chunks") && value["Chunks"].IsArray())
	{
		auto chunks = value["Chunks"].GetArray();
		for (int c = 0; c < chunks.Size(); c++)
		{
			Chunk chunk;
			JSON_READ(chunks[c]["x"], chunk.x);
			JSON_READ(chunks[c]["y"], chunk.y);

			auto layers = chunks[c]["Layers"].GetArray();
			for (int l = 0; l < layers.Size(); l++)
			{
				auto tiles = layers[l]["Tiles"].GetArray();
				for (int t = 0; t < tiles.Size(); t++)
					JSON_READ(tiles[t], chunk.layers[l].tiles[t / CHUNK_SIZE][t % CHUNK_SIZE]);

				auto height = layers[l]["Height"].GetArray();
				for (int h = 0; h < height.Size(); h++)
					JSON_READ(height[h], chunk.layers[l].height[h / CHUNK_SIZE][h % CHUNK_SIZE]);
				
			}
		}
	}

	// tiles


	// meshes


	// textuers


	// interactable items


	// sort any data that needs to be sorted 



	
	return map;
}
