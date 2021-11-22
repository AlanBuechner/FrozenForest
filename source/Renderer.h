#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include "Math.h"
#include "Map.h"

#define SW (256)
#define SH (192)

#define PLAYER_SPRITE 0

namespace Renderer
{
	void Init();

	void BeginFrame();
	void EndFrame();

	void SetCamera(const Camera& cam);

	void DrawMesh(const Mesh& mesh, const Math::Vec3& position, uint8_t rotation);
	void DrawTile(const Tile& tile, const Math::Vec3& position);

	void DrawChunk(const Chunk& chunk, const Math::Vec3& pos);

	void DrawSprite(int x, int y, int id);
	void DrawAnimatedSprite(int x, int y, int id, uint16_t frame);
}

#endif