/*---------------------------------------------------------------------------------

libnds_internal.h -- internal variables for libnds

Copyright (C) 2005 - 2008
Dave Murphy (WinterMute)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.
2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.
3.	This notice may not be removed or altered from any source
distribution.

---------------------------------------------------------------------------------*/

#pragma once

#include <nds/arm9/input.h>
#include <nds/system.h>
#include <time.h>

//---------------------------------------------------------------------------------
typedef struct __TransferRegion {
    //---------------------------------------------------------------------------------
    vs16               touchX, touchY;     // TSC X, Y
    vs16               touchXpx, touchYpx; // TSC X, Y pixel values
    vs16               touchZ1, touchZ2;   // TSC x-panel measurements
    vu16               buttons;            // X, Y, /PENIRQ buttons
    time_t             unixTime;
    struct __bootstub *bootcode;
} __TransferRegion, *__pTransferRegion;

#define ipc ( (__TransferRegion volatile *) ( 0x02FFF000 ) )
#define transfer ( *ipc )

static inline void setTransferInputData( touchPosition *p_touch, u16 p_buttons ) {
    transfer.buttons  = p_buttons;
    transfer.touchX   = p_touch->rawx;
    transfer.touchY   = p_touch->rawy;
    transfer.touchXpx = p_touch->px;
    transfer.touchYpx = p_touch->py;
    transfer.touchZ1  = p_touch->z1;
    transfer.touchZ2  = p_touch->z2;
}
static inline __TransferRegion volatile *__transferRegion( ) {
    return &transfer;
}

//---------------------------------------------------------------------------------
