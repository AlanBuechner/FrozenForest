#define ARM9
#include <nds.h>

#include "Input.h"

namespace Input
{

	void Update()
	{
		scanKeys();
	}

	bool IsKeyPressed(Key key)
	{
		u16 keys = keysDown();
		return (keys & key);
	}

	bool IsKeyDown(Key key)
	{
		u16 keys = keysHeld();
		return (keys & key);
	}
	
}