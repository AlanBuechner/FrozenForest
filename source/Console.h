#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdio.h>
#include <string>
#include <stdlib.h>


namespace Console
{
    void Init();
    void Clear();
    void SetPrintString(const std::string& str);
    void Update();
    bool IsPrinting();

    bool IsScreenClear();
}


#endif