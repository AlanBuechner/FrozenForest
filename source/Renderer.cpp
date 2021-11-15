#define ARM9
#include <nds.h>
#include "Renderer.h"

namespace Renderer
{
	void Init()
	{
		//set mode 0, enable BG0 and set it to 3D
		videoSetMode(MODE_0_3D);

		// initialize gl
		glInit();
	
		// enable antialiasing
		glEnable(GL_ANTIALIAS);
	
		// setup the rear plane
		glClearColor(0,0,0,31); // BG must be opaque for AA to work
		glClearPolyID(63); // BG must have a unique polygon ID for AA to work
		glClearDepth(0x7FFF);

		//this should work the same as the normal gl call
		glViewport(0,0,255,191);
	}

	void BeginFrame()
	{
		while (REG_DISPCAPCNT & DCAP_ENABLE);
	}

	void EndFrame()
	{
		glFlush(0);
		swiWaitForVBlank();
	}

	void SetCamera(const Camera& cam)
	{
		//any floating point gl call is being converted to fixed prior to being implemented
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(cam.fov, 256.0 / 192.0, cam.near, cam.far);
	
		const Math::Vec3& pos = cam.Position;
		Math::Vec3 lookAt = {
			pos.x + cam.Dir.x,
			pos.y + cam.Dir.y,
			pos.z + cam.Dir.z,
		};

		gluLookAt(	
			pos.x, pos.y, pos.z,				//camera possition
			lookAt.x, lookAt.y, lookAt.z,		//look at
			0.0, 1.0, 0.0						//up
		);		
	}

	void DrawMesh(const Mesh& mesh, const Math::Vec3& position, uint8_t rotation)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef32(floattof32(position.x), floattof32(position.y), floattof32(position.z));
				
		glRotateX(0.0f);
		glRotateY(90.0f * rotation);

		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

		glBegin(mesh.quds? GL_QUADS : GL_TRIANGLES);

		for(uint16_t i = 0; i < mesh.indexCount; i++)
		{
			Vertex vert = mesh.verts[mesh.indices[i]];

			glColor3b(255,0,0);
			GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
			glVertex3v16(vert.position.x, vert.position.y, vert.position.z);
		}

		glEnd();

		glPopMatrix(1);
	}

	void DrawTile(const Tile& tile, const Math::Vec3& position)
	{
		DrawMesh(GetMesh(tile.meshId), position, tile.rotation & 0b11);
	}

	void DrawChunk(const Chunk& chunk, const Math::Vec3& pos)
	{
		Math::Vec3 p = { (chunk.x*CHUNK_SIZE) - pos.x, pos.y, (chunk.y*CHUNK_SIZE)-pos.z };
		for(uint8_t c = 0; c < 3; c++)
		{
			for(uint8_t x = 0; x < CHUNK_SIZE; x++)
			{
				for(uint8_t y = 0; y < CHUNK_SIZE; y++)
				{
					uint32_t tileIndex = chunk.layers[c].tiles[x][y];
					uint32_t height = chunk.layers[c].height[x][y];
					if(tileIndex != 0)
						DrawTile(GetTile(tileIndex-1), {p.x+x, height-p.y, p.z+y-CHUNK_SIZE+1});
				}
			}
		}
	}

}