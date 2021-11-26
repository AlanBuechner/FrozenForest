#include "Map.h"
#include <stdio.h>
#include <algorithm>

Mesh GetMesh(uint32_t meshID)
{
	Mesh* meshLocation = (Mesh*)GetMeshLocation(meshID);
	Mesh mesh = *meshLocation;

	mesh.verts = (Vertex*)((size_t)meshLocation + 6u);
	mesh.indices = (uint16_t*)((size_t)meshLocation + 6u + (mesh.vertCount*sizeof(Vertex)));
	return mesh;
}

uint32_t GetCurrentChunk(uint32_t x, uint32_t y)
{
	uint32_t extent = GetChunkXExtent() + 1;

	if(x < 0 || -y < 0 || x >= extent)
		return UINT32_MAX;

	uint32_t i = -y * extent + x;
	
	uint32_t lowi = 0, highi = GetNumberOfChunks()-1;

	while (lowi <= highi)
	{
		// get the mid chunk
		uint32_t midi = (lowi + highi)/2;
		const Chunk& mid = GetChunk(midi);

		// calculate index for low high and mid
		uint32_t mi = (mid.y * extent) + mid.x;

		if(i == mi)
			return midi;

		if(i < mi)
			highi = midi - 1;

		if(i > mi)
			lowi = midi + 1;
	}

	return UINT32_MAX; // could not find the chunk
	
}