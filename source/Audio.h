#ifndef AUDIO_H
#define AUDIO_H

#include <nds.h>
#include <stdio.h>
#include <maxmod9.h>

namespace AudioStream
{
    mm_word Music(mm_word length, mm_addr dest, mm_stream_formats formats );
    void Init();
    void Update();
    void Play(int i);
}

#endif