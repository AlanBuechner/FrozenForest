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

	Console::SetPrintString("hello world!");

	while(1)
	{
		// update
		Console::Update();
		Input::Update();
		player.update();

		// render
		Renderer::BeginFrame();

		// 3d renderering
		Renderer::DrawChunk(GetChunk(0), player.position);

		// 2d renderering
		Renderer::DrawAnimatedSprite(SW/2-16, SH/2-30, PLAYER_SPRITE, player.state*3 + player.frame);

		Renderer::EndFrame();
	}

	return 0;
}//end main


