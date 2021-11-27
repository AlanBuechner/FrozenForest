#ifndef AUDIO_H
#define AUDIO_H
#include <nds.h>
#include <stdio.h>
#include <maxmod9.h>
#include "Audio.h"

namespace AudioStream
{
    constexpr int clipCount = 1;
    mm_stream music[clipCount];

    int sine;
    int lfo;

     enum adjustment
    {
	    sine_freq = 500,
	
	    lfo_freq = 3,
	
	    // LFO output shift amount
	    lfo_shift = 4,
	
	    // blue backdrop
	    bg_colour = 13 << 10,
	
	    // red cpu usage
	    cpu_colour = 31
    };



    mm_word Music(mm_word length, mm_addr dest, mm_stream_formats formats, int i)
    {
        s16 *target = (s16*)dest;

        int len = length;
        for( ; len; len--)
        {
            int sample = sinLerp(sine);

            *target++ = sample;

            *target++ = -sample;

            sine += sine_freq + (sinLerp(lfo) >> lfo_shift);
            lfo = (lfo + lfo_freq);

        }   
        return length;
    }

    mm_word Music2(mm_word length, mm_addr dest, mm_stream_formats formats )
    {
       return Music(length, dest, formats, 1);
    }

    mm_word Music1(mm_word length, mm_addr dest, mm_stream_formats formats )
    {
       return Music(length, dest, formats, 0);
    }

    void Init()
    {
        mm_ds_system dssys;
        dssys.mod_count = 0;
        dssys.samp_count = 0;
        dssys.mem_bank = 0;
        dssys.fifo_channel = FIFO_MAXMOD;
        mmInit(&dssys);

        music[0].sampling_rate = 25000;
        music[0].buffer_length = 1200;
        music[0].callback = AudioStream::Music1;
        music[0].format = MM_STREAM_16BIT_STEREO;
        music[0].timer = MM_TIMER0;
        //mmStreamOpen(&music[0]);

       
    }

    void Play(int i)
    {
        mmStreamOpen(&music[i]);
    }

    void Update()
    {

    }
}

#endif