/*
Pokémon neo
------------------------------

file        : auxspi.cpp
description : An even thinner reimplementation of some functions of card.cpp, that
              actually works, I hope. Uses ideas from "savegame_manager" and based on the
              documentation at "gbatek".

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

#include <algorithm>
#include <cstdio>
#include <memory>

#include "defines.h"
#include "fs.h"

namespace FS::CARD {
    bool checkCard( ) {
        u8 header1[ 512 ], header2[ 512 ];
        cardReadHeader( header1 );
        cardReadHeader( header2 );

        // Encrypted cards are always bad
        if( !!std::memcmp( header1, header2, 32 ) ) { return false; }
// #ifndef DESQUID
//         if( !!std::memcmp( header1, GAME_TITLE, 12 ) ) { return false; }
// #endif
        return true;
    }

    void waitBusy( ) {
        while( REG_AUXSPICNT & 0x80 ) {
            // Do nothing!
        }
    }

    u8 write( u16 p_cnt, u8 p_data ) {
        REG_AUXSPICNT = ( 0xa000 | p_cnt );
        waitBusy( );
        REG_AUXSPIDATA = p_data;
        waitBusy( );
        return REG_AUXSPIDATA;
    }

    void waitWriteInProgress( ) {
        write( 0x00, 0x00 );
        waitBusy( );
        while( REG_AUXSPIDATA & 0x01 ) {
            write( 0x40, 0x05 );
            waitBusy( );
            write( 0x00, 0x00 );
            waitBusy( );
        }
    }

    u8 transfer( u8 p_data ) {
        // Write data
        REG_AUXSPIDATA = p_data;
        // Wait (things will happen)
        waitBusy( );
        // Read (possibly changed) data
        return REG_AUXSPIDATA;
    }

    void write( u8 p_data ) {
        transfer( p_data );
    }

    void readData( u32 p_address, u8* p_out, u32 p_cnt ) {
        write( 0x40, 0x03 );
        write( ( p_address >> 16 ) & 0xff );
        write( ( p_address >> 8 ) & 0xff );
        write( p_address & 0xFF );

        while( p_cnt > 0 ) {
            // Get data
            u8 data = 0;
            if( p_cnt > 1 ) {
                data = transfer( 0x00 );
            } else {
                data = write( 0x00, 0x00 );
            }

            *p_out++ = data;
            p_cnt--;
        }

        // Wait for stuff to stabilize
        write( 0x40, 0x05 );
        waitWriteInProgress( );
        write( 0x40, 0x05 );
        waitWriteInProgress( );
        swiDelay( 14 );
    }

    const u32 BLOCK_LEN = 256;
    u8        WRITE_BUFFER[ BLOCK_LEN + 4 ];
    bool      writeData( u32 p_address, u8* p_data, u32 p_cnt ) {
        u32 addr_end = p_address + p_cnt;

        bool error = false;

        // we can only write a finite amount of data at once, so we need a separate loop
        //  for multiple passes.
        while( p_address < addr_end ) {
            // set WEL (Write Enable Latch)
            write( 0x40, 0x06 );
            write( 0x00,
                   0x00 ); // This should not be necessary, but desmume seems to think otherwise
                           // (just write( 0, 6 ); suffices for melonDS)

            swiDelay( 14 );

            auto old_ad = p_address;

            // open for replace/write
            write( 0x40, 0x0a );
            write( ( p_address >> 16 ) & 0xff );
            write( ( p_address >> 8 ) & 0xff );
            write( p_address & 0xFF );

            u32 dtw = 0;
            for( u32 i = 0; p_address < addr_end && i < BLOCK_LEN; ++i, ++p_address, ++dtw ) {
                if( p_address == addr_end - 1 || i == BLOCK_LEN - 1 ) {
                    // Close before last byte
                    if( p_data ) {
                        write( 0x00, *p_data++ );
                    } else {
                        write( 0x00, 0 );
                    }
                    continue;
                }
                if( p_data ) {
                    write( *p_data++ );
                } else {
                    write( 0 );
                }
            }

            // Wait for write to finish
            write( 0x40, 0x05 );
            waitWriteInProgress( );
            write( 0x40, 0x05 );
            waitWriteInProgress( );
            swiDelay( 14 );

            // Verify that the write succeeded
            write( 0x40, 0x03 );
            write( ( old_ad >> 16 ) & 0xff );
            write( ( old_ad >> 8 ) & 0xff );
            write( old_ad & 0xFF );

            for( u32 i = 0; i < dtw; ++i ) {
                // Get data
                u8 data = 0;
                if( i < dtw - 1 ) {
                    data = transfer( 0x00 );
                } else {
                    data = write( 0x00, 0x00 );
                }

                // Check that it is indeed correct
                if( !p_data ) {
                    if( data ) { error = true; } // Meh something went wrong
                } else {
                    if( data != ( *( p_data - dtw + i ) & 0xFF ) ) {
                        error = true;
                    } // Meh again, error
                }
            }

            // Wait for stuff to stabilize
            write( 0x40, 0x05 );
            waitWriteInProgress( );
            write( 0x40, 0x05 );
            waitWriteInProgress( );
            swiDelay( 14 );
        }
        return !error;
    }

    bool writeData( u8* p_data, u32 p_cnt, std::function<void( u16, u16 )> p_progress ) {
        u16 numBlocks = BACKUP_SIZE / BLOCK_LEN;

        for( u16 b = 0; b < numBlocks; ++b ) {
            std::memset( WRITE_BUFFER, 0xFF, sizeof( WRITE_BUFFER ) );
            if( b * BLOCK_LEN < p_cnt ) {
                std::memcpy( WRITE_BUFFER, p_data + ( b * BLOCK_LEN ),
                             std::min( BLOCK_LEN, p_cnt - b * BLOCK_LEN ) );
            } else {
                break;
            }
            if( !writeData( b * BLOCK_LEN, WRITE_BUFFER, BLOCK_LEN ) ) { return false; }
            p_progress( b + 1, numBlocks );
        }

        return true;
    }
} // namespace FS::CARD
