// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
#ifndef NO_SOUND
#ifndef MMOD

#include <nds.h>

#include "fs/data.h"
#include "sound/sseq.h"
#include "sound/sseqData.h"

#include "nav/nav.h"

namespace SOUND::SSEQ {
    static void sndsysMsgHandler( int, void * );
    static void returnMsgHandler( int, void * );

    constexpr u16   MAX_MESSAGE_POINTER = 1536;
    u8              MESSAGE_DATA[ MAX_MESSAGE_POINTER ];
    u32             MESSAGE_POINTER = 0;
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

    void playSequence( u16 p_seqId ) {
        stopSequence( );
        freeSequence( );
        CURRENT_SEQUENCE.m_message = SNDSYS_PLAYSEQ;

        auto &seq = SSEQ_LIST[ p_seqId ];

        if( !FS::loadSoundSequence( &CURRENT_SEQUENCE.m_seq, seq.m_sseqId ) ) {
            DESQUID_LOG( "meh 1:/" );
        }
        if( !FS::loadSoundBank( &CURRENT_SEQUENCE.m_bnk, seq.m_bank ) ) {
            DESQUID_LOG( "meh 2:/" );
        }
        for( u8 i = 0; i < seq.m_sampleCnt; ++i ) {
            if( !FS::loadSoundSample( CURRENT_SEQUENCE.m_war + i, seq.m_samplesId[ i ] ) ) {
                DESQUID_LOG( std::to_string( p_seqId ) + " " + std::to_string( i ) + " meh 3:/" );
            }
        }
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

} // namespace SOUND::SSEQ

#endif
#endif
