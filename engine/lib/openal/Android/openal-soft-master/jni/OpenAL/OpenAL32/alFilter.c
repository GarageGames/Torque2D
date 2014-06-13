/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
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

#include <stdlib.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alFilter.h"
#include "alThunk.h"
#include "alError.h"


static void InitFilterParams(ALfilter *filter, ALenum type);

#define LookupFilter(m, k) ((ALfilter*)LookupUIntMapKey(&(m), (k)))

AL_API ALvoid AL_APIENTRY alGenFilters(ALsizei n, ALuint *filters)
{
    ALCcontext *Context;
    ALsizei i=0;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0 || IsBadWritePtr((void*)filters, n * sizeof(ALuint)))
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        ALCdevice *device = Context->Device;
        ALenum err;

        while(i < n)
        {
            ALfilter *filter = calloc(1, sizeof(ALfilter));
            if(!filter)
            {
                alSetError(Context, AL_OUT_OF_MEMORY);
                alDeleteFilters(i, filters);
                break;
            }

            filter->filter = ALTHUNK_ADDENTRY(filter);
            err = InsertUIntMapEntry(&device->FilterMap, filter->filter, filter);
            if(err != AL_NO_ERROR)
            {
                ALTHUNK_REMOVEENTRY(filter->filter);
                memset(filter, 0, sizeof(ALfilter));
                free(filter);

                alSetError(Context, err);
                alDeleteFilters(i, filters);
                break;
            }

            filters[i++] = filter->filter;
            InitFilterParams(filter, AL_FILTER_NULL);
        }
    }

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alDeleteFilters(ALsizei n, ALuint *filters)
{
    ALCcontext *Context;
    ALCdevice *device;
    ALfilter *ALFilter;
    ALboolean Failed;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    Failed = AL_TRUE;
    device = Context->Device;
    if(n < 0)
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        Failed = AL_FALSE;
        // Check that all filters are valid
        for(i = 0;i < n;i++)
        {
            if(!filters[i])
                continue;

            if(LookupFilter(device->FilterMap, filters[i]) == NULL)
            {
                alSetError(Context, AL_INVALID_NAME);
                Failed = AL_TRUE;
                break;
            }
        }
    }

    if(!Failed)
    {
        // All filters are valid
        for(i = 0;i < n;i++)
        {
            // Recheck that the filter is valid, because there could be duplicated names
            if((ALFilter=LookupFilter(device->FilterMap, filters[i])) == NULL)
                continue;

            RemoveUIntMapKey(&device->FilterMap, ALFilter->filter);
            ALTHUNK_REMOVEENTRY(ALFilter->filter);

            memset(ALFilter, 0, sizeof(ALfilter));
            free(ALFilter);
        }
    }

    ProcessContext(Context);
}

AL_API ALboolean AL_APIENTRY alIsFilter(ALuint filter)
{
    ALCcontext *Context;
    ALboolean  result;

    Context = GetContextSuspended();
    if(!Context) return AL_FALSE;

    result = ((!filter || LookupFilter(Context->Device->FilterMap, filter)) ?
              AL_TRUE : AL_FALSE);

    ProcessContext(Context);

    return result;
}

AL_API ALvoid AL_APIENTRY alFilteri(ALuint filter, ALenum param, ALint iValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALfilter   *ALFilter;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALFilter=LookupFilter(Device->FilterMap, filter)) != NULL)
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            if(iValue == AL_FILTER_NULL ||
               iValue == AL_FILTER_LOWPASS)
                InitFilterParams(ALFilter, iValue);
            else
                alSetError(Context, AL_INVALID_VALUE);
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alFilteriv(ALuint filter, ALenum param, ALint *piValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if(LookupFilter(Device->FilterMap, filter) != NULL)
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            alFilteri(filter, param, piValues[0]);
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alFilterf(ALuint filter, ALenum param, ALfloat flArg)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALfilter   *ALFilter;
	ALfp flValue = float2ALfp(flArg);

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALFilter=LookupFilter(Device->FilterMap, filter)) != NULL)
    {
        switch(ALFilter->type)
        {
        case AL_FILTER_LOWPASS:
            switch(param)
            {
            case AL_LOWPASS_GAIN:
                if(flValue >= int2ALfp(0) && flValue <= int2ALfp(1))
                    ALFilter->Gain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_LOWPASS_GAINHF:
                if(flValue >= int2ALfp(0) && flValue <= int2ALfp(1))
                    ALFilter->GainHF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alFilterfv(ALuint filter, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if(LookupFilter(Device->FilterMap, filter) != NULL)
    {
        switch(param)
        {
        default:
            alFilterf(filter, param, pflValues[0]);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetFilteri(ALuint filter, ALenum param, ALint *piValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALfilter   *ALFilter;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALFilter=LookupFilter(Device->FilterMap, filter)) != NULL)
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            *piValue = ALFilter->type;
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetFilteriv(ALuint filter, ALenum param, ALint *piValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if(LookupFilter(Device->FilterMap, filter) != NULL)
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            alGetFilteri(filter, param, piValues);
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetFilterf(ALuint filter, ALenum param, ALfloat *pflValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALfilter   *ALFilter;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALFilter=LookupFilter(Device->FilterMap, filter)) != NULL)
    {
        switch(ALFilter->type)
        {
        case AL_FILTER_LOWPASS:
            switch(param)
            {
            case AL_LOWPASS_GAIN:
                *pflValue = ALfp2float(ALFilter->Gain);
                break;

            case AL_LOWPASS_GAINHF:
                *pflValue = ALfp2float(ALFilter->GainHF);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
            break;

        default:
            alSetError(Context, AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetFilterfv(ALuint filter, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if(LookupFilter(Device->FilterMap, filter) != NULL)
    {
        switch(param)
        {
        default:
            alGetFilterf(filter, param, pflValues);
            break;
        }
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}


ALvoid ReleaseALFilters(ALCdevice *device)
{
    ALsizei i;
    for(i = 0;i < device->FilterMap.size;i++)
    {
        ALfilter *temp = device->FilterMap.array[i].value;
        device->FilterMap.array[i].value = NULL;

        // Release filter structure
        ALTHUNK_REMOVEENTRY(temp->filter);
        memset(temp, 0, sizeof(ALfilter));
        free(temp);
    }
}


static void InitFilterParams(ALfilter *filter, ALenum type)
{
    filter->type = type;

    filter->Gain = int2ALfp(1);
    filter->GainHF = int2ALfp(1);
}
