// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
#include <nds.h>

#include "defines.h"
#include "sseq.h"

#ifndef MMOD

namespace SOUND::SSEQ {

    static void sndsysMsgHandler( int, void * );
    static void returnMsgHandler( int, void * );

    constexpr u16   MAX_MESSAGE_POINTER = 1536;
    volatile u8     MESSAGE_DATA[ MAX_MESSAGE_POINTER ];
    volatile u32    MESSAGE_POINTER = 0;
    soundSysMessage CURRENT_SEQUENCE;
    u32             CURRENT_SEQUENCE_OFFSET[ 6 ] = { 0, 0, 0, 0, 0, 0 };
    u32             CURRENT_SEQUENCE_SIZE[ 6 ]
        = { 0, 0, 0, 0, 0, 0 }; // To save reloading stuff that is already loaded.

    void installSoundSys( ) {
        /* Install FIFO */
        fifoSetDatamsgHandler( FIFO_SNDSYS, sndsysMsgHandler, 0 );
        fifoSetDatamsgHandler( FIFO_RETURN, returnMsgHandler, 0 );
    }

    static void sndsysMsgHandler( int p_length, void * ) {
        soundSysMessage msg;
        fifoGetDatamsg( FIFO_SNDSYS, p_length, (u8 *) &msg );
    }

    static void returnMsgHandler( int p_length, void * ) {
        returnMessage msg;
        if( MESSAGE_POINTER + p_length < MAX_MESSAGE_POINTER ) {
            fifoGetDatamsg( FIFO_RETURN, p_length, (u8 *) &MESSAGE_DATA[ MESSAGE_POINTER ] );
            MESSAGE_POINTER += p_length;
        } else {
            fifoGetDatamsg( FIFO_RETURN, p_length,
                            (u8 *) &msg ); // Toss the overflowing message.
        }
    }

    void freeSequenceData( sequenceData *p_userdata ) {
        if( p_userdata->m_size > 0 ) {
            free( p_userdata->m_data );
            p_userdata->m_size = 0;
        }
    }

    void freeSequence( ) {
        freeSequenceData( &CURRENT_SEQUENCE.m_seq );
        freeSequenceData( &CURRENT_SEQUENCE.m_bnk );
        freeSequenceData( CURRENT_SEQUENCE.m_war + 0 );
        freeSequenceData( CURRENT_SEQUENCE.m_war + 1 );
        freeSequenceData( CURRENT_SEQUENCE.m_war + 2 );
        freeSequenceData( CURRENT_SEQUENCE.m_war + 3 );
    }

    /* The following code must be rethought: */

    /*
    int PlaySmp(sndreg_t* smp, int a, int d, int s, int r, int vol, int vel, int
    pan)
    {
            soundSysMessage msg;
            msg.msg = SNDSYS_PLAY;
            msg.sndreg = *smp;
            msg.a = (u8) a;
            msg.d = (u8) d;
            msg.s = (u8) s;
            msg.r = (u8) r;
            msg.vol = (u8) vol;
            msg.vel = (u8) vel;
            msg.pan = (s8) pan;
            fifoSendDatamsg(FIFO_SNDSYS, sizeof(msg), (u8*) &msg);
            return (int) fifoGetRetValue(FIFO_SNDSYS);
    }

    void StopSmp(int handle)
    {
            soundSysMessage msg;
            msg.msg = SNDSYS_STOP;
            msg.ch = handle;
            fifoSendDatamsg(FIFO_SNDSYS, sizeof(msg), (u8*) &msg);
    }
    */
    /*
    static bool LoadNDS( sequenceData *p_data, const char *fname, const u32 Offset,
                         const u32 Size ) {
        FILE *f = fopen( fname, "rb" );
        if( !f ) return false;
        fseek( f, Offset, SEEK_SET );
        p_data->size = Size;
        p_data->data = malloc( Size );
        fread( p_data->data, 1, Size, f );
        fclose( f );
        DC_FlushRange( p_data->data, Size );
        return true;
    }
    */

    void playSequence( const char *seqFile, const char *bnkFile, const char *war1, const char *war2,
                       const char *war3, const char *war4 ) {
        stopSequence( );
        freeSequence( );
        CURRENT_SEQUENCE.m_message = SNDSYS_PLAYSEQ;

        loadSequenceData( &CURRENT_SEQUENCE.m_seq, seqFile );
        loadSequenceData( &CURRENT_SEQUENCE.m_bnk, bnkFile );
        loadSequenceData( CURRENT_SEQUENCE.m_war + 0, war1 );
        loadSequenceData( CURRENT_SEQUENCE.m_war + 1, war2 );
        loadSequenceData( CURRENT_SEQUENCE.m_war + 2, war3 );
        loadSequenceData( CURRENT_SEQUENCE.m_war + 3, war4 );

        fifoSendDatamsg( FIFO_SNDSYS, sizeof( CURRENT_SEQUENCE ), (u8 *) &CURRENT_SEQUENCE );
    }

    void stopSequence( ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_STOPSEQ;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    void pauseSequence( ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_PAUSESEQ;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    void fadeSequence( ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_FADESEQ;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    /*
    void PlaySeqNDS( const char *ndsFile, const u32 SSEQOffset, const u32 SSEQSize,
                     const u32 BANKOffset, const u32 BANKSize, const u32 WAVEARC1Offset,
                     const u32 WAVEARC1Size, const u32 WAVEARC2Offset, const u32 WAVEARC2Size,
                     const u32 WAVEARC3Offset, const u32 WAVEARC3Size, const u32 WAVEARC4Offset,
                     const u32 WAVEARC4Size ) {
        stopSequence( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        // freeSequence();
        CURRENT_SEQUENCE.msg = SNDSYS_PLAYSEQ;

        if( ( SSEQOffset == CURRENT_SEQUENCE_OFFSET[ 0 ] )
            && ( SSEQSize == CURRENT_SEQUENCE_SIZE[ 0 ] ) ) {
            iprintf( "SSEQ Already Loaded.\n" );
        } else {
            freeSequenceData( &CURRENT_SEQUENCE.seq );
            CURRENT_SEQUENCE_OFFSET[ 0 ] = SSEQOffset;
            CURRENT_SEQUENCE_SIZE[ 0 ]   = SSEQSize;
            iprintf( "Loading SSEQ.\n" );
            LoadNDS( &CURRENT_SEQUENCE.seq, ndsFile, SSEQOffset, SSEQSize );
        }

        if( ( BANKOffset == CURRENT_SEQUENCE_OFFSET[ 1 ] )
            && ( BANKSize == CURRENT_SEQUENCE_SIZE[ 1 ] ) ) {
            iprintf( "BANK Already Loaded.\n" );
        } else {
            freeSequenceData( &CURRENT_SEQUENCE.bnk );
            CURRENT_SEQUENCE_OFFSET[ 1 ] = BANKOffset;
            CURRENT_SEQUENCE_SIZE[ 1 ]   = BANKSize;
            iprintf( "Loading BANK.\n" );
            LoadNDS( &CURRENT_SEQUENCE.bnk, ndsFile, BANKOffset, BANKSize );
        }

        if( ( WAVEARC1Offset == CURRENT_SEQUENCE_OFFSET[ 2 ] )
            && ( WAVEARC1Size == CURRENT_SEQUENCE_SIZE[ 2 ] ) ) {
            if( WAVEARC1Offset != 0 ) iprintf( "WAVEARC1 Already Loaded\n" );
        } else {
            freeSequenceData( CURRENT_SEQUENCE.war + 0 );
            CURRENT_SEQUENCE_OFFSET[ 2 ] = WAVEARC1Offset;
            CURRENT_SEQUENCE_SIZE[ 2 ]   = WAVEARC1Size;
            if( WAVEARC1Offset != 0 ) {
                iprintf( "Loading WAVEARC1.\n" );
                LoadNDS( CURRENT_SEQUENCE.war + 0, ndsFile, WAVEARC1Offset, WAVEARC1Size );
            }
        }

        if( ( WAVEARC2Offset == CURRENT_SEQUENCE_OFFSET[ 3 ] )
            && ( WAVEARC2Size == CURRENT_SEQUENCE_SIZE[ 3 ] ) ) {
            if( WAVEARC2Offset != 0 ) iprintf( "WAVEARC2 Already Loaded\n" );
        } else {
            freeSequenceData( CURRENT_SEQUENCE.war + 1 );
            CURRENT_SEQUENCE_OFFSET[ 3 ] = WAVEARC2Offset;
            CURRENT_SEQUENCE_SIZE[ 3 ]   = WAVEARC2Size;
            if( WAVEARC2Offset != 0 ) {
                iprintf( "Loading WAVEARC2.\n" );
                LoadNDS( CURRENT_SEQUENCE.war + 1, ndsFile, WAVEARC2Offset, WAVEARC2Size );
            }
        }

        if( ( WAVEARC3Offset == CURRENT_SEQUENCE_OFFSET[ 4 ] )
            && ( WAVEARC3Size == CURRENT_SEQUENCE_SIZE[ 4 ] ) ) {
            if( WAVEARC3Offset != 0 ) iprintf( "WAVEARC3 Already Loaded\n" );
        } else {
            freeSequenceData( CURRENT_SEQUENCE.war + 2 );
            CURRENT_SEQUENCE_OFFSET[ 4 ] = WAVEARC3Offset;
            CURRENT_SEQUENCE_SIZE[ 4 ]   = WAVEARC3Size;
            if( WAVEARC3Offset != 0 ) {
                iprintf( "Loading WAVEARC3.\n" );
                LoadNDS( CURRENT_SEQUENCE.war + 2, ndsFile, WAVEARC3Offset, WAVEARC3Size );
            }
        }

        if( ( WAVEARC4Offset == CURRENT_SEQUENCE_OFFSET[ 5 ] )
            && ( WAVEARC4Size == CURRENT_SEQUENCE_SIZE[ 5 ] ) ) {
            if( WAVEARC4Offset != 0 ) iprintf( "WAVEARC4 Already Loaded\n" );
        } else {
            freeSequenceData( CURRENT_SEQUENCE.war + 3 );
            CURRENT_SEQUENCE_OFFSET[ 5 ] = WAVEARC4Offset;
            CURRENT_SEQUENCE_SIZE[ 5 ]   = WAVEARC4Size;
            if( WAVEARC4Offset != 0 ) {
                iprintf( "Loading WAVEARC4.\n" );
                LoadNDS( CURRENT_SEQUENCE.war + 3, ndsFile, WAVEARC4Offset, WAVEARC4Size );
            }
        }

        fifoSendDatamsg( FIFO_SNDSYS, sizeof( CURRENT_SEQUENCE ), (u8 *) &CURRENT_SEQUENCE );
    }
*/
} // namespace SOUND::SSEQ

#endif
