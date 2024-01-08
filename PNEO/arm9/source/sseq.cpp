// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
#ifndef NO_SOUND
#include <cstdio>
#include <nds.h>

#include "fs/data.h"
#include "sound/sseq.h"
#include "sound/sseqData.h"

#include "io/message.h"

namespace SOUND::SSEQ {
    static void sndsysMsgHandler( int, void * );
    static void returnMsgHandler( int, void * );

    soundSysMessage CURRENT_SEQUENCE{ };
    u32             CURRENT_SEQUENCE_OFFSET[ 6 ] = { 0, 0, 0, 0, 0, 0 };
    u32             CURRENT_SEQUENCE_SIZE[ 6 ]
        = { 0, 0, 0, 0, 0, 0 }; // To save reloading stuff that is already loaded.

    u16  CURRENT_SEQUENCE_ID   = 0;
    u16  NEXT_SEQUENCE_ID[ 2 ] = { 0, 0 };
    bool SEQ_SWAP_IN_PROGRESS  = false;

    bool SEQ_SWAP_AM = false;

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
        fifoGetDatamsg( FIFO_RETURN, p_length, (u8 *) &msg );

        if( msg.m_count < 1 ) {
            // nothing of value is lost
            return;
        }
        switch( msg.m_data[ 0 ] ) {
        default: return;
        case returnMessage::MSG_SEQUENCE_ENDED: {
            // restart orig bgm (after one shot)
            // TODO
            break;
        }
        case returnMessage::MSG_SEQUENCE_STOPPED: {
            if( SEQ_SWAP_IN_PROGRESS ) {
                // printf( "\nFADE COMPLETE NEXT: %u", NEXT_SEQUENCE_ID[ 0 ] );
                // start queued bgm
                if( NEXT_SEQUENCE_ID[ 0 ] ) {
                    playSequence( NEXT_SEQUENCE_ID[ 0 ], false );
                } else {
                    SEQ_SWAP_IN_PROGRESS = false;
                }
            }
            break;
        }
        case returnMessage::MSG_SEQUENCE_UNFADED: {
            // bgm swap complete, check if another chage was requested
            SEQ_SWAP_IN_PROGRESS  = false;
            NEXT_SEQUENCE_ID[ 0 ] = NEXT_SEQUENCE_ID[ 1 ];
            NEXT_SEQUENCE_ID[ 1 ] = 0;

            if( NEXT_SEQUENCE_ID[ 0 ] ) {
                // next fade already lined up, start new fade
                fadeSequence( );
            } else {
                ANIMATE_MAP = SEQ_SWAP_AM;
            }
            break;
        }
        }
    }

    void freeSequenceData( sequenceData *p_userdata ) {
        if( p_userdata->m_size > 0 && p_userdata->m_data ) {
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

    bool fadeSwapSequence( u16 p_seqId ) {
        if( !CURRENT_SEQUENCE_ID ) {
            // if no sequence is currently playing, just start playing
            return playSequence( p_seqId, true );
        }

        // DESQUID_LOG( "FADESWAP REQUEST " + std::to_string( SEQ_SWAP_IN_PROGRESS ) );

        if( NEXT_SEQUENCE_ID[ 0 ] == p_seqId ) {
            // cancel previously enqueued swap
            NEXT_SEQUENCE_ID[ 1 ] = 0;
            return true;
        }
        if( NEXT_SEQUENCE_ID[ SEQ_SWAP_IN_PROGRESS ] == p_seqId ) {
            // sequence already queued, do nothing
            return true;
        }
        NEXT_SEQUENCE_ID[ SEQ_SWAP_IN_PROGRESS ] = p_seqId;
        if( !SEQ_SWAP_IN_PROGRESS ) {
            SEQ_SWAP_AM = ANIMATE_MAP;
            ANIMATE_MAP = false;
            fadeSequence( );
        }
        return true;
    }

    bool playSequence( u16 p_seqId, bool p_fadeIn ) {
        if( !p_fadeIn ) {
            if( SEQ_SWAP_IN_PROGRESS ) { ANIMATE_MAP = SEQ_SWAP_AM; }
            SEQ_SWAP_IN_PROGRESS  = false;
            NEXT_SEQUENCE_ID[ 0 ] = NEXT_SEQUENCE_ID[ 1 ] = 0;
        }
        auto oa     = ANIMATE_MAP;
        ANIMATE_MAP = false;

        if( !p_fadeIn && CURRENT_SEQUENCE_ID ) { stopSequence( ); }

        CURRENT_SEQUENCE.m_message = SNDSYS_PLAYSEQ;

        const auto &seq = SSEQ_LIST[ p_seqId ];

        if( !FS::loadSoundSequence( &CURRENT_SEQUENCE.m_seq, seq.m_sseqId ) ) {
            CURRENT_SEQUENCE_ID  = 0;
            SEQ_SWAP_IN_PROGRESS = false;
            ANIMATE_MAP          = oa;
            return false;
            // DESQUID_LOG( std::string( "Sound sequence " ) + std::to_string( p_seqId )
            // + " failed." );
        }

        if( !FS::loadSoundBank( &CURRENT_SEQUENCE.m_bnk, seq.m_bank ) ) {
            CURRENT_SEQUENCE_ID  = 0;
            SEQ_SWAP_IN_PROGRESS = false;
            ANIMATE_MAP          = oa;
            return false;
        }
        for( u8 i = 0; i < seq.m_sampleCnt; ++i ) {
            if( seq.m_samplesId[ i ]
                && !FS::loadSoundSample( CURRENT_SEQUENCE.m_war + i, seq.m_samplesId[ i ], i ) ) {
                CURRENT_SEQUENCE_ID  = 0;
                SEQ_SWAP_IN_PROGRESS = false;
                ANIMATE_MAP          = oa;
                return false;
            }
        }
        CURRENT_SEQUENCE.m_fadeIn = false; // p_fadeIn;
        CURRENT_SEQUENCE_ID       = p_seqId;
        // printf( "\n PLAY %i %i", p_seqId, p_fadeIn );

        fifoSendDatamsg( FIFO_SNDSYS, sizeof( CURRENT_SEQUENCE ), (u8 *) &CURRENT_SEQUENCE );
        ANIMATE_MAP = oa;
        return true;
    }

    void stopSequence( ) {
        CURRENT_SEQUENCE_ID   = 0;
        SEQ_SWAP_IN_PROGRESS  = false;
        NEXT_SEQUENCE_ID[ 0 ] = NEXT_SEQUENCE_ID[ 1 ] = 0;

        soundSysMessage msg;
        msg.m_message = SNDSYS_STOPSEQ;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );

        freeSequence( );
    }

    void pauseSequence( ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_PAUSESEQ;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    void setMasterVolume( u8 p_volume ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_VOLUME;
        msg.m_volume  = p_volume;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    void fadeSequence( ) {
        soundSysMessage msg;
        msg.m_message        = SNDSYS_FADESEQ;
        SEQ_SWAP_IN_PROGRESS = true;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

    int playSample( void *p_data, const sampleInfo &p_sampleInfo, const playInfo &p_playInfo ) {
        soundSysMessage msg;
        msg.m_message    = SNDSYS_PLAY_SAMPLE;
        msg.m_sample     = sequenceData{ p_data, p_sampleInfo.m_nonLoopLen };
        msg.m_sampleInfo = p_sampleInfo;
        msg.m_playInfo   = p_playInfo;

        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
        return (int) fifoGetRetValue( FIFO_SNDSYS );
    }

    void stopSample( int p_handle ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_STOP_SAMPLE;
        msg.m_channel = p_handle;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

} // namespace SOUND::SSEQ

#endif
