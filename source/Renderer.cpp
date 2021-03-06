#define ARM9
#include <nds.h>
#include "Renderer.h"

#include <iostream>

#include "player.h"

// textures
#include "snowtile_bin.h"
#include "bushtile_bin.h"
#include "bush_innercorner_bin.h"
#include "bushcorner_bin.h"
#include "bushside_bottom_bin.h"
#include "slopeside_bottom_bin.h"
#include "slopeturn_bin.h"
#include "sparkle_bin.h"


namespace Renderer
{
	constexpr int numTextures = 8;
	int textureIDs[numTextures];

	struct Sprite
	{
		uint16_t* gfx;
		SpriteSize size;

		unsigned short* pal;
		uint32_t palLen;
	};

	struct AnimatedSprite
	{
		uint16_t frames;
		uint16_t** gfx;
		SpriteSize size;

		unsigned short* pal;
		uint32_t palLen;
	};

	constexpr int numSprites = 1;
	int currentSprite = 0;
	Sprite sprites[numSprites];

	constexpr int numAnimatedSprites = 1;
	int currentAnimatedSprite = 0;
	AnimatedSprite animatedSprites[numAnimatedSprites];

	void MakeSprite(const unsigned int* tile, const unsigned short* pal, uint32_t tileSize, uint32_t palSize, SpriteSize size)
	{
		Sprite& sprite = sprites[currentSprite];
		//allocate some space for the sprite graphics	
		sprite.gfx = oamAllocateGfx(&oamMain, size, SpriteColorFormat_256Color);

		// copy the sprite to the hardware
		dmaCopy(tile, sprite.gfx, tileSize);
		dmaCopy(pal, SPRITE_PALETTE, palSize);

		sprite.size = size;

		sprite.pal = (unsigned short*)pal;
		sprite.palLen = palSize;

		currentSprite++;
	}

	void MakeAnimatedSprite(const unsigned int* tile, const unsigned short* pal, uint32_t tileSize, uint32_t palSize, SpriteSize size, int frames)
	{
		AnimatedSprite& sprite = animatedSprites[currentAnimatedSprite];
		sprite.frames = frames;
		sprite.gfx = new uint16_t*[frames];

		u8* tiles = (u8*)tile;
		for(int i = 0; i < frames; i++)
		{
			sprite.gfx[i] = oamAllocateGfx(&oamMain, size, SpriteColorFormat_256Color);
			dmaCopy(tiles, sprite.gfx[i], tileSize);
			tiles += tileSize;
		}

		sprite.size = size;
		sprite.pal = (unsigned short*)pal;
		sprite.palLen = palSize;

		currentAnimatedSprite++;
	}

	void BindSprite(const unsigned short* pal, uint32_t palLen)
	{
		dmaCopy(pal, SPRITE_PALETTE, palLen);
	}

	void Init()
	{
		//set mode 0, enable BG0 and set it to 3D
		videoSetMode(MODE_0_3D);

		// initialize gl
		glInit();
	
		// enable antialiasing
		//glEnable(GL_ANTIALIAS);
		//enable textures
		glEnable(GL_TEXTURE_2D);
	
		// setup the rear plane
		glClearColor(0,0,0,31); // BG must be opaque for AA to work
		glClearPolyID(63); // BG must have a unique polygon ID for AA to work
		glClearDepth(0x7FFF);

		//this should work the same as the normal gl call
		glViewport(0,0,255,191);

		// init 2d renderer
		//put bg 0 at a lower priority than the text background
		bgSetPriority(0, 1);
		oamInit(&oamMain, SpriteMapping_1D_128, false);

		// set vram backs
		vramSetBankA(VRAM_A_TEXTURE);
		vramSetBankB(VRAM_B_MAIN_SPRITE);

		// create sprites
		//MakeSprite(playerTiles, playerPal, 32*32, playerPalLen, SpriteSize_32x32);

		// create animated sprites
		MakeAnimatedSprite(playerTiles, playerPal, 32*32, playerPalLen, SpriteSize_32x32, 12);

		// create opengl textures
		glGenTextures(numTextures, textureIDs);
		glBindTexture(0, textureIDs[0]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)snowtile_bin);

		glBindTexture(0, textureIDs[1]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)bushtile_bin);

		glBindTexture(0, textureIDs[2]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)bush_innercorner_bin);

		glBindTexture(0, textureIDs[3]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)bushcorner_bin);

		glBindTexture(0, textureIDs[4]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)bushside_bottom_bin);

		glBindTexture(0, textureIDs[5]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)slopeside_bottom_bin);

		glBindTexture(0, textureIDs[6]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)slopeturn_bin);

		glBindTexture(0, textureIDs[7]);
		glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_16 , TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)sparkle_bin);

	}

	void BeginFrame()
	{
		// wait for display capture to be enabled
		while (REG_DISPCAPCNT & DCAP_ENABLE);
	}

	void EndFrame()
	{
		// update the 3d renderer
		glFlush(0);
		//send the updates to the hardware
		oamUpdate(&oamMain);
		// wait for thev-blank
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

	void DrawMesh(const Mesh& mesh, const Math::Vec3& position, uint8_t rotation, uint16_t textureId)
	{
		if(position.z > 6 || position.z < -7)
			return;
		
		if(position.x > 8.5f || position.x < -8.5f)
			return;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef32(floattof32(position.x), floattof32(position.y/2), floattof32(position.z));
				
		glRotateX(0.0f);
		glRotateY(90.0f * rotation);

		//glMaterialf(GL_AMBIENT, RGB15(16,16,16));
		//glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
		//glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
		glMaterialf(GL_EMISSION, RGB15(16,16,16));

		//ds uses a table for shinyness..this generates a half-ass one
		//glMaterialShinyness();

		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

		if(textureId >= numTextures)
			glBindTexture(0, textureIDs[0]);
		else
			glBindTexture(0, textureIDs[textureId]);

		glBegin(mesh.quds? GL_QUADS : GL_TRIANGLES);
		glNormal(NORMAL_PACK(0,inttov10(1),0));

		for(uint16_t i = 0; i < mesh.indexCount; i++)
		{
			Vertex vert = mesh.verts[mesh.indices[i]];

			GFX_TEX_COORD = (TEXTURE_PACK(vert.uv.x, vert.uv.y));
			glVertex3v16(vert.position.x, vert.position.y, vert.position.z);
		}

		glEnd();

		glPopMatrix(1);
	}

	void DrawTile(const Tile& tile, const Math::Vec3& position)
	{
		DrawMesh(GetMesh(tile.meshId), position, tile.rotation & 0b11, tile.textureId);
	}

	void DrawChunk(const Chunk& chunk, const Math::Vec3& pos)
	{
		Math::Vec3 p = { (chunk.x*CHUNK_SIZE) - pos.x, pos.y, -(chunk.y*CHUNK_SIZE)-pos.z };
		for(uint8_t c = 0; c < 3; c++)
		{
			for(uint8_t x = 0; x < CHUNK_SIZE; x++)
			{
				for(uint8_t y = 0; y < CHUNK_SIZE; y++)
				{
					uint32_t tileIndex = chunk.layers[c].tiles[y][x];
					uint32_t height = chunk.layers[c].height[y][x];
					if(tileIndex != 0)
						DrawTile(GetTile(tileIndex-1), {p.x+x, height-p.y, p.z+y-CHUNK_SIZE+1});
				}
			}
		}
	}

	void DrawSprite(int x, int y, int id)
	{
		if(id >= numSprites)
			return;

		Sprite& sprite = sprites[id];

		BindSprite(sprite.pal, sprite.palLen);

		oamSet(&oamMain, 			//main graphics engine context
			0,						//oam index (0 to 127)  
			x, y,					//x and y pixel location of the sprite		
			0,						//priority, lower renders last (on top)
			0,						//palette index if multiple palettes or the alpha value if bmp sprite
			sprite.size,
			SpriteColorFormat_256Color,
			sprite.gfx,				//pointer to the loaded graphics
			-1,						//sprite rotation data  
			false,					//double the size when rotating?
			false,					//hide the sprite?
			false, false, 			//vflip, hflip
			false					//apply mosaic
			);
	}

	void DrawAnimatedSprite(int x, int y, int id, uint16_t frame)
	{
		if(id >= numAnimatedSprites)
			return;

		AnimatedSprite& sprite = animatedSprites[id];

		BindSprite(sprite.pal, sprite.palLen);

		oamSet(&oamMain, 			//main graphics engine context
			0,						//oam index (0 to 127)  
			x, y,					//x and y pixel location of the sprite		
			0,						//priority, lower renders last (on top)
			0,						//palette index if multiple palettes or the alpha value if bmp sprite	
			sprite.size,
			SpriteColorFormat_256Color,
			sprite.gfx[frame],		//pointer to the loaded graphics
			-1,						//sprite rotation data  
			false,					//double the size when rotating?
			false,					//hide the sprite?
			false, false, 			//vflip, hflip
			false					//apply mosaic
			);
	}

}