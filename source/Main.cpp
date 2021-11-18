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
	cam.Position = {0,7,1};
	cam.Dir = -cam.Position;
	cam.fov = 75.0f;
	Renderer::SetCamera(cam);

	Entity player;

	iprintf("%u\n", (size_t)GetHeader().textureSectionOffset);
	iprintf("%ld\n", GetNumberOfTextures());
	Texture texture = GetTexture(0);
	iprintf("%d, %d\n", texture.width, texture.height);

	while(1)
	{
		// update
		Input::Update();
		player.update();
		

		// render
		Renderer::BeginFrame();
		Renderer::DrawChunk(GetChunk(0), player.position);
		//Renderer::DrawChunk(GetChunk(0), { -9,0,-9 });
		//Renderer::DrawChunk(GetChunk(0), { -9,0,0 });
		//Renderer::DrawChunk(GetChunk(0), { -9,0,9 });
		//Renderer::DrawChunk(GetChunk(0), { 0,0,-9 });
		//Renderer::DrawChunk(GetChunk(0), { 0,0,0 });
		//Renderer::DrawChunk(GetChunk(0), { 0,0,9 });
		//Renderer::DrawChunk(GetChunk(0), { 9,0,-9 });
		//Renderer::DrawChunk(GetChunk(0), { 9,0,0 });
		//Renderer::DrawChunk(GetChunk(0), { 9,0,9 });
		Renderer::EndFrame();
	}

	return 0;
}//end main
