/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2010 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "alu.h"

static void SetSpeakerArrangement(const char *name, ALfp SpeakerAngle[MAXCHANNELS],
                                  Channel Speaker2Chan[MAXCHANNELS], ALint chans)
{
    char layout_str[256];
    char *confkey, *next;
    char *sep, *end;
    Channel val;
    int i;

    if(!ConfigValueExists(NULL, name))
        name = "layout";

    strncpy(layout_str, GetConfigValue(NULL, name, ""), sizeof(layout_str));
    layout_str[sizeof(layout_str)-1] = 0;

    if(!layout_str[0])
        return;

    next = confkey = layout_str;
    while(next && *next)
    {
        confkey = next;
        next = strchr(confkey, ',');
        if(next)
        {
            *next = 0;
            do {
                next++;
            } while(isspace(*next) || *next == ',');
        }

        sep = strchr(confkey, '=');
        if(!sep || confkey == sep)
            continue;

        end = sep - 1;
        while(isspace(*end) && end != confkey)
            end--;
        *(++end) = 0;

        if(strcmp(confkey, "fl") == 0 || strcmp(confkey, "front-left") == 0)
            val = FRONT_LEFT;
        else if(strcmp(confkey, "fr") == 0 || strcmp(confkey, "front-right") == 0)
            val = FRONT_RIGHT;
        else if(strcmp(confkey, "fc") == 0 || strcmp(confkey, "front-center") == 0)
            val = FRONT_CENTER;
        else if(strcmp(confkey, "bl") == 0 || strcmp(confkey, "back-left") == 0)
            val = BACK_LEFT;
        else if(strcmp(confkey, "br") == 0 || strcmp(confkey, "back-right") == 0)
            val = BACK_RIGHT;
        else if(strcmp(confkey, "bc") == 0 || strcmp(confkey, "back-center") == 0)
            val = BACK_CENTER;
        else if(strcmp(confkey, "sl") == 0 || strcmp(confkey, "side-left") == 0)
            val = SIDE_LEFT;
        else if(strcmp(confkey, "sr") == 0 || strcmp(confkey, "side-right") == 0)
            val = SIDE_RIGHT;
        else
        {
            AL_PRINT("Unknown speaker for %s: \"%s\"\n", name, confkey);
            continue;
        }

        *(sep++) = 0;
        while(isspace(*sep))
            sep++;

        for(i = 0;i < chans;i++)
        {
            if(Speaker2Chan[i] == val)
            {
                long angle = strtol(sep, NULL, 10);
                if(angle >= -180 && angle <= 180)
                    SpeakerAngle[i] = ALfpMult(int2ALfp(angle), float2ALfp(M_PI/180.0f));
                else
                    AL_PRINT("Invalid angle for speaker \"%s\": %ld\n", confkey, angle);
                break;
            }
        }
    }

    for(i = 0;i < chans;i++)
    {
        int min = i;
        int i2;

        for(i2 = i+1;i2 < chans;i2++)
        {
            if(SpeakerAngle[i2] < SpeakerAngle[min])
                min = i2;
        }

        if(min != i)
        {
            ALfp tmpf;
            Channel tmpc;

            tmpf = SpeakerAngle[i];
            SpeakerAngle[i] = SpeakerAngle[min];
            SpeakerAngle[min] = tmpf;

            tmpc = Speaker2Chan[i];
            Speaker2Chan[i] = Speaker2Chan[min];
            Speaker2Chan[min] = tmpc;
        }
    }
}

static ALfp aluLUTpos2Angle(ALint pos)
{
    if(pos < QUADRANT_NUM)
        return aluAtan(ALfpDiv(int2ALfp(pos), int2ALfp(QUADRANT_NUM - pos)));
    if(pos < 2 * QUADRANT_NUM)
        return (float2ALfp(M_PI_2) + aluAtan(ALfpDiv(int2ALfp(pos - QUADRANT_NUM),int2ALfp(2 * QUADRANT_NUM - pos))));
    if(pos < 3 * QUADRANT_NUM)
        return (aluAtan(ALfpDiv(int2ALfp(pos - 2 * QUADRANT_NUM), int2ALfp(3 * QUADRANT_NUM - pos))) - float2ALfp(M_PI));
    return (aluAtan(ALfpDiv(int2ALfp(pos - 3 * QUADRANT_NUM), int2ALfp(4 * QUADRANT_NUM - pos))) - float2ALfp(M_PI));
}

ALint aluCart2LUTpos(ALfp re, ALfp im)
{
    ALint pos = 0;
    ALfp denom = (aluFabs(re) + aluFabs(im));
    if(denom > int2ALfp(0))
        pos = (ALint)ALfp2int(ALfpDiv(ALfpMult(int2ALfp(QUADRANT_NUM),aluFabs(im)), (denom + float2ALfp(0.5))));

    if(re < int2ALfp(0))
        pos = 2 * QUADRANT_NUM - pos;
    if(im < int2ALfp(0))
        pos = LUT_NUM - pos;
    return pos%LUT_NUM;
}

ALvoid aluInitPanning(ALCdevice *Device)
{
    ALfp SpeakerAngle[MAXCHANNELS];
    ALfp (*Matrix)[MAXCHANNELS];
    Channel *Speaker2Chan;
    ALfp Alpha, Theta;
    ALfp *PanningLUT;
    ALint pos, offset;
    ALuint s, s2;

    for(s = 0;s < MAXCHANNELS;s++)
    {
        for(s2 = 0;s2 < MAXCHANNELS;s2++)
            Device->ChannelMatrix[s][s2] = ((s==s2) ? int2ALfp(1) : int2ALfp(0));
    }

    Speaker2Chan = Device->Speaker2Chan;
    Matrix = Device->ChannelMatrix;
    switch(Device->FmtChans)
    {
        case DevFmtMono:
            Matrix[FRONT_LEFT][FRONT_CENTER]  = aluSqrt(float2ALfp(0.5));
            Matrix[FRONT_RIGHT][FRONT_CENTER] = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_LEFT][FRONT_CENTER]   = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_RIGHT][FRONT_CENTER]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_LEFT][FRONT_CENTER]   = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_RIGHT][FRONT_CENTER]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][FRONT_CENTER] = int2ALfp(1);
            Device->NumChan = 1;
            Speaker2Chan[0] = FRONT_CENTER;
			SpeakerAngle[0] = int2ALfp(0);
            break;

        case DevFmtStereo:
#ifdef APPORTABLE_OPTIMIZED_OUT
            // Leave as identity matrix if Apportable-optimized
            Matrix[FRONT_CENTER][FRONT_LEFT]  = aluSqrt(float2ALfp(0.5));
            Matrix[FRONT_CENTER][FRONT_RIGHT] = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_LEFT][FRONT_LEFT]     = int2ALfp(1);
            Matrix[SIDE_RIGHT][FRONT_RIGHT]   = int2ALfp(1);
            Matrix[BACK_LEFT][FRONT_LEFT]     = int2ALfp(1);
            Matrix[BACK_RIGHT][FRONT_RIGHT]   = int2ALfp(1);
            Matrix[BACK_CENTER][FRONT_LEFT]   = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][FRONT_RIGHT]  = aluSqrt(float2ALfp(0.5));
#endif
            Device->NumChan = 2;
            Speaker2Chan[0] = FRONT_LEFT;
            Speaker2Chan[1] = FRONT_RIGHT;
			SpeakerAngle[0] = float2ALfp(-90.0f * M_PI/180.0f);
            SpeakerAngle[1] = float2ALfp( 90.0f * M_PI/180.0f);
            SetSpeakerArrangement("layout_STEREO", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

#ifdef STEREO_ONLY
        case DevFmtQuad:
        case DevFmtX51:
        case DevFmtX61:
        case DevFmtX71:
            break;
#else
        case DevFmtQuad:
            Matrix[FRONT_CENTER][FRONT_LEFT]  = aluSqrt(float2ALfp(0.5));
            Matrix[FRONT_CENTER][FRONT_RIGHT] = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_LEFT][FRONT_LEFT]     = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_LEFT][BACK_LEFT]      = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_RIGHT][FRONT_RIGHT]   = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_RIGHT][BACK_RIGHT]    = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][BACK_LEFT]    = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][BACK_RIGHT]   = aluSqrt(float2ALfp(0.5));
            Device->NumChan = 4;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_RIGHT;
            Speaker2Chan[3] = BACK_RIGHT;
            SpeakerAngle[0] = float2ALfp(-135.0f * M_PI/180.0f);
            SpeakerAngle[1] = float2ALfp( -45.0f * M_PI/180.0f);
            SpeakerAngle[2] = float2ALfp(  45.0f * M_PI/180.0f);
            SpeakerAngle[3] = float2ALfp( 135.0f * M_PI/180.0f);
            SetSpeakerArrangement("layout_QUAD", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case DevFmtX51:
            Matrix[SIDE_LEFT][FRONT_LEFT]   = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_LEFT][BACK_LEFT]    = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_RIGHT][FRONT_RIGHT] = aluSqrt(float2ALfp(0.5));
            Matrix[SIDE_RIGHT][BACK_RIGHT]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][BACK_LEFT]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][BACK_RIGHT] = aluSqrt(float2ALfp(0.5));
            Device->NumChan = 5;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_CENTER;
            Speaker2Chan[3] = FRONT_RIGHT;
            Speaker2Chan[4] = BACK_RIGHT;
            SpeakerAngle[0] = float2ALfp(-110.0f * M_PI/180.0f);
            SpeakerAngle[1] = float2ALfp( -30.0f * M_PI/180.0f);
            SpeakerAngle[2] = float2ALfp(   0.0f * M_PI/180.0f);
            SpeakerAngle[3] = float2ALfp(  30.0f * M_PI/180.0f);
            SpeakerAngle[4] = float2ALfp( 110.0f * M_PI/180.0f);
            SetSpeakerArrangement("layout_51CHN", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case DevFmtX61:
            Matrix[BACK_LEFT][BACK_CENTER]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_LEFT][SIDE_LEFT]    = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_RIGHT][BACK_CENTER] = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_RIGHT][SIDE_RIGHT]  = aluSqrt(float2ALfp(0.5));
            Device->NumChan = 6;
            Speaker2Chan[0] = SIDE_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_CENTER;
            Speaker2Chan[3] = FRONT_RIGHT;
            Speaker2Chan[4] = SIDE_RIGHT;
            Speaker2Chan[5] = BACK_CENTER;
            SpeakerAngle[0] = float2ALfp(-90.0f * M_PI/180.0f);
            SpeakerAngle[1] = float2ALfp(-30.0f * M_PI/180.0f);
            SpeakerAngle[2] = float2ALfp(  0.0f * M_PI/180.0f);
            SpeakerAngle[3] = float2ALfp( 30.0f * M_PI/180.0f);
            SpeakerAngle[4] = float2ALfp( 90.0f * M_PI/180.0f);
            SpeakerAngle[5] = float2ALfp(180.0f * M_PI/180.0f);
            SetSpeakerArrangement("layout_61CHN", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case DevFmtX71:
            Matrix[BACK_CENTER][BACK_LEFT]  = aluSqrt(float2ALfp(0.5));
            Matrix[BACK_CENTER][BACK_RIGHT] = aluSqrt(float2ALfp(0.5));
            Device->NumChan = 7;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = SIDE_LEFT;
            Speaker2Chan[2] = FRONT_LEFT;
            Speaker2Chan[3] = FRONT_CENTER;
            Speaker2Chan[4] = FRONT_RIGHT;
            Speaker2Chan[5] = SIDE_RIGHT;
            Speaker2Chan[6] = BACK_RIGHT;
            SpeakerAngle[0] = float2ALfp(-150.0f * M_PI/180.0f);
            SpeakerAngle[1] = float2ALfp( -90.0f * M_PI/180.0f);
            SpeakerAngle[2] = float2ALfp( -30.0f * M_PI/180.0f);
            SpeakerAngle[3] = float2ALfp(   0.0f * M_PI/180.0f);
            SpeakerAngle[4] = float2ALfp(  30.0f * M_PI/180.0f);
            SpeakerAngle[5] = float2ALfp(  90.0f * M_PI/180.0f);
            SpeakerAngle[6] = float2ALfp( 150.0f * M_PI/180.0f);
            SetSpeakerArrangement("layout_71CHN", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;
#endif
    }

    if(GetConfigValueBool(NULL, "scalemix", 0))
    {
        ALfp maxout = int2ALfp(1);;
        for(s = 0;s < MAXCHANNELS;s++)
        {
            ALfp out = int2ALfp(0);
            for(s2 = 0;s2 < MAXCHANNELS;s2++)
                out = (out + Device->ChannelMatrix[s2][s]);
            maxout = __max(maxout, out);
        }

        maxout = ALfpDiv(int2ALfp(1),maxout);
        for(s = 0;s < MAXCHANNELS;s++)
        {
            for(s2 = 0;s2 < MAXCHANNELS;s2++)
                Device->ChannelMatrix[s2][s] = ALfpMult(Device->ChannelMatrix[s2][s],maxout);
        }
    }

    PanningLUT = Device->PanningLUT;
    for(pos = 0; pos < LUT_NUM; pos++)
    {
        /* clear all values */
        offset = MAXCHANNELS * pos;
        for(s = 0; s < MAXCHANNELS; s++)
            PanningLUT[offset+s] = int2ALfp(0);

        if(Device->NumChan == 1)
        {
            PanningLUT[offset + Speaker2Chan[0]] = int2ALfp(1);
            continue;
        }

        /* source angle */
        Theta = aluLUTpos2Angle(pos);

        /* set panning values */
        for(s = 0; s < Device->NumChan - 1; s++)
        {
            if(Theta >= SpeakerAngle[s] && Theta < SpeakerAngle[s+1])
            {
                /* source between speaker s and speaker s+1 */
                Alpha = ALfpDiv(ALfpMult(float2ALfp(M_PI_2), (Theta-SpeakerAngle[s])),
                                 (SpeakerAngle[s+1]-SpeakerAngle[s]));
                PanningLUT[offset + Speaker2Chan[s]]   = __cos(Alpha);
                PanningLUT[offset + Speaker2Chan[s+1]] = __sin(Alpha);
                break;
            }
        }
        if(s == Device->NumChan - 1)
        {
            /* source between last and first speaker */
            if(Theta < SpeakerAngle[0])
                Theta = (Theta + float2ALfp(2.0f * M_PI));
            Alpha = ALfpDiv(ALfpMult(float2ALfp(M_PI_2), (Theta-SpeakerAngle[s])),
                             (float2ALfp(2.0f * M_PI) + SpeakerAngle[0]-SpeakerAngle[s]));
            PanningLUT[offset + Speaker2Chan[s]] = __cos(Alpha);
            PanningLUT[offset + Speaker2Chan[0]] = __sin(Alpha);
        }
    }
}
