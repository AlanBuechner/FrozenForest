#define ARM9
#include <nds.h>
#include "Console.h"

namespace Console
{
    void Init()
    {
		consoleDemoInit();
    }

    void Clear()
    {
        consoleClear();
    }
}