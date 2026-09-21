/*
Pokémon neo
------------------------------

file        : card.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.


Copyright (C) 2012 - 2026
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <functional>
#include <string>

#include <nds.h>

namespace FS::CARD {
    /*
     * @brief: Check if an (emulated) flash storage device is available and usable.
     */
    bool checkCard( );

    /*
     * @brief: Wait until the latest flash mem op completes.
     */
    void waitBusy( );

    /*
     * @brief: Wait until the latest write to flash mem completes.
     */
    void waitWriteInProgress( );

    /*
     * @brief: Write data to REG_AUXSPIDATA and read the possibly different result
     * of the write op from REG_AUXSPIDATA.
     */
    u8 transfer( u8 p_data );

    /*
     * @brief: Alias for transfer that ignores the return value.
     */
    void write( u8 p_data );

    /*
     * @brief: Read p_cnt bytes from flash mem address p_address into p_out.
     */
    void readData( u32 p_address, u8* p_out, u32 p_cnt );

    /*
     * @brief: Write p_cnt bytes from p_data to flash mem (starting from flash mem
     * address 0).
     * @param p_progress: callback that is called whenever a block (256 bytes) has been
     * successfully written; gets #num blocks written and #total blocks to be written
     * as parameters.
     */
    bool writeData( u8* p_data, u32 p_cnt, std::function<void( u16, u16 )> p_progress );

    /*
     * @brief: Write p_cnt bytes from p_data to flash memory starting from flash mem
     * address p_address.
     */
    bool writeData( u32 p_address, u8* p_data, u32 p_cnt );
} // namespace FS::CARD
