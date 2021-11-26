#define ARM9
#include <nds.h>
#include "Console.h"
#include <string.h>

#include "Time.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include "Input.h"

namespace Console
{
	std::string print;
	float nextPrintTime;
	float currentTime;
	uint32_t currentIndex = 0;
	void Init()
	{
		consoleDemoInit();
	}

	void Clear()
	{
		consoleClear();
	}

	bool IsPrinting()
	{
		return !print.empty();
	}

	void SetPrintString(const std::string& str)
	{
		print = str;
		nextPrintTime = 0;
		currentIndex = 0;
		currentTime = 0;
	}

	void Update()
	{

		if (IsPrinting())
		{
			if (currentIndex <= print.size())
			{
				currentTime += DELTA_TIME;
				if (currentTime >= nextPrintTime)
				{
					iprintf("%c", print[currentIndex]);
					nextPrintTime += 0.25f;
						currentIndex++;
					if (currentIndex == print.size())
						print = "";
				}
			}

			if (Input::IsKeyPressed(Key::A))
			{
				Clear();
				iprintf("%s", print.c_str());
				print = "";
			}
		}
	}
}