#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include "Math.h"
#include "Map.h"

namespace Renderer
{
	void Init();

	void BeginFrame();
	void EndFrame();

	void SetCamera(const Camera& cam);

	void DrawMesh(const Mesh& mesh, const Math::Vec3& position, uint8_t rotation);
	void DrawTile(const Tile& tile, const Math::Vec3& position);

	void DrawChunk(const Chunk& chunk, const Math::Vec3& pos);

}

#endif