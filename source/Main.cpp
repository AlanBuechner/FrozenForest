#include "Renderer.h"
#include "Console.h"
#include "Input.h"
#include "Time.h"
#include "Camera.h"
#include "Map.h"
#include "Entity.h"

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
	Renderer::Init();
	Console::Init();

	Camera cam;
	cam.Position = Math::Vec3{0,7,1};
	cam.Dir = -cam.Position;
	cam.fov = 75.0f;
	Renderer::SetCamera(cam);

	Entity player;
	player.position = Math::Vec3{4, 0, -4};

	while(1)
	{
		// update
		Console::Update();
		Input::Update();

		if(Input::IsKeyPressed(Key::A) && player.dir == Math::Vec3{0,0,0})
		{
			Math::Vec3 dir{(float)(-(player.state-2)%2), 0, (float)((player.state-1)%2)};
			Math::Vec3 tile = player.position + dir;
		}
		else if(!Console::IsPrinting())
			player.update();

		// render
		Renderer::BeginFrame();

		// 3d renderering
		Renderer::DrawChunk(GetChunk(player.currentChunk), player.position);
		for(uint8_t i = 0; i < 8; i++)
			if(player.surroundingChunks[i] != UINT32_MAX)
				Renderer::DrawChunk(GetChunk(player.surroundingChunks[i]), player.position);

		// 2d renderering
		Renderer::DrawAnimatedSprite(SW/2-16, SH/2-30, PLAYER_SPRITE, player.state*3 + player.frame);

		Renderer::EndFrame();
	}

	return 0;
}//end main


