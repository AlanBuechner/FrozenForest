#include "Map.h"
#include <stdio.h>

Mesh GetMesh(uint32_t meshID)
{
	Mesh* meshLocation = (Mesh*)GetMeshLocation(meshID);
	Mesh mesh = *meshLocation;

	mesh.verts = (Vertex*)((size_t)meshLocation + 6u);
	mesh.indices = (uint16_t*)((size_t)meshLocation + 6u + (mesh.vertCount*sizeof(Vertex)));
	return mesh;
}