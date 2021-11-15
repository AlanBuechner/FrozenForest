#include "Map.h"
#include <stdio.h>

Texture GetTexture(uint32_t textureID)
{
	Texture* textureLocation = GetTextureLocation(textureID);
	Texture texture = *textureLocation;
	texture.data = (byte_t*)(textureLocation + (2 * sizeof(uint32_t)));
	return texture;
}

Mesh GetMesh(uint32_t meshID)
{
	Mesh* meshLocation = (Mesh*)GetMeshLocation(meshID);
	Mesh mesh = *meshLocation;

	mesh.verts = (Vertex*)((size_t)meshLocation + 6u);
	mesh.indices = (uint16_t*)((size_t)meshLocation + 6u + (mesh.vertCount*sizeof(Vertex)));
	return mesh;
}