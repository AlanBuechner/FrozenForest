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

	bool clearScreen = false;
	bool screenClear = true;

	void Init()
	{
		consoleDemoInit();
	}

	void Clear()
	{
		clearScreen = true;
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
		clearScreen = false;
		screenClear = false;
	}

	void Update()
	{

		if(clearScreen)
		{
			consoleClear();
			screenClear = true;
		}

		if (IsPrinting())
		{
			if (currentIndex <= print.size())
			{
				currentTime += DELTA_TIME;
				if (currentTime >= nextPrintTime)
				{
					iprintf("%c", print[currentIndex]);
					nextPrintTime += 0.1f;
						currentIndex++;
					if (currentIndex == print.size())
						print = "";
				}
			}

			if (Input::IsKeyPressed(Key::A))
			{
				consoleClear();
				iprintf("%s", print.c_str());
				print = "";
			}
		}
		else if(Input::IsKeyPressed(Key::A))
		{
			Clear();
		}
	}

	bool IsScreenClear()
	{
		return screenClear;
	}
}