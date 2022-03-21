// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
#include <cstring>
#include <nds.h>

#include "sound/sseq.h"

#ifndef NO_SOUND
#ifndef MMOD
namespace SOUND::SSEQ {
    int          TRACK_CNT         = 0;
    u8*          SEQUENCE_DATA     = NULL;
    void*        SEQUENCE_BANK     = NULL;
    void*        SEQUENCE_WAR[ 4 ] = { NULL, NULL, NULL, NULL };
    trackState   TRACKS[ NUM_CHANNEL ];
    int          MESSAGE_SEND_FLAG = 0;
    volatile int SEQ_BPM           = 0;

    sampleInfo* getWav( void* p_war, int p_id ) {
        return (sampleInfo*) ( int( p_war ) + ( (int*) ( int( p_war ) + 60 ) )[ p_id ] );
    }

    u32 getInstr( void* p_bnk, int p_id ) {
        return *(u32*) ( int( p_bnk ) + 60 + 4 * p_id );
    }

    /*
     * @brief: plays the specified note on a free channel.
     */
    int playNote( void* p_bnk, void** p_war, int p_instr, int p_note, int p_priority,
                  playInfo* p_playInfo, int p_duration, int p_track ) {
        int isPsg   = 0;
        int channel = nextFreeChannel( p_priority );
        if( channel < 0 ) { return -1; }

        adsrState* chstat = ADSR_CHANNEL + channel;

        u32         inst    = getInstr( p_bnk, p_instr );
        u8*         insdata = GETINSTDATA( p_bnk, inst );
        noteDef*    notedef = NULL;
        sampleInfo* wavinfo = NULL;
        int         fRecord = INST_TYPE( inst );
    _ReadRecord:
        if( fRecord == 0 ) {
            return -1;
        } else if( fRecord == 1 ) {
            notedef = (noteDef*) insdata;
        } else if( fRecord < 4 ) {
            // PSG
            // fRecord = 2 -> PSG tone, notedef->wavid -> PSG duty
            // fRecord = 3 -> PSG noise
            isPsg   = 1;
            notedef = (noteDef*) insdata;
            if( fRecord == 3 ) {
                channel = nextFreeNoiseChannel( p_priority );
                if( channel < 0 ) { return -1; }
                chstat             = ADSR_CHANNEL + channel;
                chstat->m_reg.m_cr = SOUND_FORMAT_PSG | SCHANNEL_ENABLE;
            } else {
#define SOUND_DUTY( p_n ) ( ( p_n ) << 24 )
                channel = nextFreeToneChannel( p_priority );
                if( channel < 0 ) { return -1; }
                chstat = ADSR_CHANNEL + channel;
                chstat->m_reg.m_cr
                    = SOUND_FORMAT_PSG | SCHANNEL_ENABLE | SOUND_DUTY( notedef->m_wavid );
            }
            // TODO: figure out what notedef->m_tnote means for PSG channels
            chstat->m_freq = adjustFreq( -SOUND_FREQ( 440 * 8 ), p_note, 69 );
            chstat->m_reg.m_timer
                = adjustPitchBend( chstat->m_freq, p_playInfo->m_pitchb, p_playInfo->m_pitchr );
        } else if( fRecord == 16 ) {
            if( ( insdata[ 0 ] <= p_note ) && ( p_note <= insdata[ 1 ] ) ) {
                int rn     = p_note - insdata[ 0 ];
                int offset = 2 + rn * ( 2 + sizeof( noteDef ) );
                fRecord    = insdata[ offset ];
                insdata += offset + 2;
                goto _ReadRecord;
            } else {
                return -1;
            }
        } else if( fRecord == 17 ) {
            int reg;
            for( reg = 0; reg < 8; reg++ ) {
                if( p_note <= insdata[ reg ] ) { break; }
            }
            if( reg == 8 ) { return -1; }

            int offset = 8 + reg * ( 2 + sizeof( noteDef ) );
            fRecord    = insdata[ offset ];
            insdata += offset + 2;
            goto _ReadRecord;
        } else {
            return -1;
        }

        if( !isPsg ) {
            wavinfo            = getWav( p_war[ notedef->m_warid ], notedef->m_wavid );
            chstat->m_reg.m_cr = SOUND_FORMAT( wavinfo->m_waveType ) | SOUND_LOOP( wavinfo->m_loop )
                                 | SCHANNEL_ENABLE;
            chstat->m_reg.m_source = (u32) GETSAMP( wavinfo );
            chstat->m_freq         = adjustFreq( wavinfo->m_time, p_note, notedef->m_tnote );
            chstat->m_reg.m_timer
                = adjustPitchBend( chstat->m_freq, p_playInfo->m_pitchb, p_playInfo->m_pitchr );
            chstat->m_reg.m_repeatPoint = wavinfo->m_loopOffset;
            chstat->m_reg.m_length      = wavinfo->m_nonLoopLen;
        }

        trackState* pTrack = TRACKS + p_track;

        chstat->m_vol         = p_playInfo->m_vol;
        chstat->m_vel         = p_playInfo->m_vel;
        chstat->m_expr        = p_playInfo->m_expr;
        chstat->m_pan         = p_playInfo->m_pan;
        chstat->m_pan2        = notedef->m_pan;
        chstat->m_modType     = p_playInfo->m_modType;
        chstat->m_modDepth    = p_playInfo->m_modDepth;
        chstat->m_modRange    = p_playInfo->m_modRange;
        chstat->m_modSpeed    = p_playInfo->m_modSpeed;
        chstat->m_modDelay    = p_playInfo->m_modDelay;
        chstat->m_modDelayCnt = 0;
        chstat->m_modCounter  = 0;
        chstat->m_attackRate  = ( pTrack->m_attackRate == -1 )
                                    ? convertAttack( notedef->m_attackRate )
                                    : pTrack->m_attackRate;
        chstat->m_decayRate   = ( pTrack->m_decayRate == -1 ) ? convertFall( notedef->m_decayRate )
                                                              : pTrack->m_decayRate;
        chstat->m_sustainRate = ( pTrack->m_sustainRate == -1 )
                                    ? convertSustain( notedef->m_sustainRate )
                                    : pTrack->m_sustainRate;
        chstat->m_releaseRate = ( pTrack->m_releaseRate == -1 )
                                    ? convertFall( notedef->m_releaseRate )
                                    : pTrack->m_releaseRate;
        chstat->m_priority    = p_priority;
        chstat->m_count       = p_duration;
        chstat->m_track       = p_track;
        chstat->m_note        = p_note;
        chstat->m_patch       = p_instr;
        updateSequencePortamento( chstat, pTrack );
        pTrack->m_portakey = p_note | ( pTrack->m_portakey & 0x80 );

        chstat->m_state = adsrState::ADSR_START;

        return channel;
    }

    inline void stopNote( u8 p_channel ) {
        ADSR_CHANNEL[ p_channel ].m_state = adsrState::ADSR_RELEASE;
    }

#define SEQ_READ8( p_pos ) SEQUENCE_DATA[ p_pos ]
#define SEQ_READ16( p_pos ) \
    ( (u16) SEQUENCE_DATA[ ( p_pos ) ] | ( (u16) SEQUENCE_DATA[ ( p_pos ) + 1 ] << 8 ) )
#define SEQ_READ24( p_pos )                                                            \
    ( (u32) SEQUENCE_DATA[ ( p_pos ) ] | ( (u32) SEQUENCE_DATA[ ( p_pos ) + 1 ] << 8 ) \
      | ( (u32) SEQUENCE_DATA[ ( p_pos ) + 2 ] << 16 ) )

    static inline void prepareTrack( int p_track, int p_pos ) {
        std::memset( TRACKS + p_track, 0, sizeof( trackState ) );
        TRACKS[ p_track ].m_pos                 = p_pos;
        TRACKS[ p_track ].m_playInfo.m_vol      = 64;
        TRACKS[ p_track ].m_playInfo.m_vel      = 64;
        TRACKS[ p_track ].m_playInfo.m_expr     = 127;
        TRACKS[ p_track ].m_playInfo.m_pan      = 64;
        TRACKS[ p_track ].m_playInfo.m_pitchb   = 0;
        TRACKS[ p_track ].m_playInfo.m_pitchr   = 2;
        TRACKS[ p_track ].m_playInfo.m_modType  = 0;
        TRACKS[ p_track ].m_playInfo.m_modDepth = 0;
        TRACKS[ p_track ].m_playInfo.m_modRange = 1;
        TRACKS[ p_track ].m_playInfo.m_modSpeed = 16;
        TRACKS[ p_track ].m_playInfo.m_modDelay = 10;
        TRACKS[ p_track ].m_priority            = 64;
        TRACKS[ p_track ].m_trackLooped         = 0;
        TRACKS[ p_track ].m_trackEnded          = 0;
        TRACKS[ p_track ].m_attackRate          = -1;
        TRACKS[ p_track ].m_decayRate           = -1;
        TRACKS[ p_track ].m_sustainRate         = -1;
        TRACKS[ p_track ].m_releaseRate         = -1;
    }

    void playSequence( sequenceData* p_sequence, sequenceData* p_bnk, sequenceData* p_war ) {
        SEQUENCE_BANK     = p_bnk->m_data;
        SEQUENCE_WAR[ 0 ] = p_war[ 0 ].m_data;
        SEQUENCE_WAR[ 1 ] = p_war[ 1 ].m_data;
        SEQUENCE_WAR[ 2 ] = p_war[ 2 ].m_data;
        SEQUENCE_WAR[ 3 ] = p_war[ 3 ].m_data;

        // Some TRACKS alter this, and may cause undesireable effects with playing other
        // TRACKS
        // later.
        ADSR_MASTER_VOLUME = 127;

        // Load sequence data
        SEQUENCE_DATA = (u8*) p_sequence->m_data + ( (u32*) p_sequence->m_data )[ 6 ];
        TRACK_CNT     = 1;

        int pos = 0;

        if( *SEQUENCE_DATA == SC_EXTRA_TRACKS ) {
            // Prepare extra TRACKS
            for( pos = 3; SEQ_READ8( pos ) == SC_OPEN_TRACK; TRACK_CNT++, pos += 3 ) {
                pos += 2;
                prepareTrack( TRACK_CNT, SEQ_READ24( pos ) );
            }
        }

        // Prepare first track
        prepareTrack( 0, pos );
        SEQ_BPM           = 120;
        MESSAGE_SEND_FLAG = 0;
    }

    void stopSequence( ) {
        returnMessage msg;
        SEQ_BPM = 0; // stop sound_timer

        for( u8 i = 0; i < NUM_CHANNEL; ++i ) { // stop p_note
            adsrState* chstat = ADSR_CHANNEL + i;
            chstat->m_state   = adsrState::ADSR_NONE;
            chstat->m_count   = 0;
            chstat->m_track   = -1;
            SCHANNEL_CR( i )  = 0;
        }
        msg.m_count     = 1;
        msg.m_data[ 0 ] = 6;
        fifoSendDatamsg( FIFO_RETURN, sizeof( msg ), (u8*) &msg );
    }

    void seq_tick( ) {
        int looped_twice = 0;
        int ended        = 0;

        // Handle p_note durations
        for( u8 i = 0; i < NUM_CHANNEL; ++i ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_count ) {
                chstat->m_count--;
                if( !chstat->m_count ) { stopNote( i ); }
            }
        }

        for( u8 i = 0; i < TRACK_CNT; i++ ) {
            trackTick( i );
            if( TRACKS[ i ].m_trackLooped >= 2 ) { looped_twice++; }
            if( TRACKS[ i ].m_trackEnded > 0 ) { ended++; }
        }
        returnMessage msg;
        if( !MESSAGE_SEND_FLAG ) {
            if( looped_twice == TRACK_CNT ) {
                MESSAGE_SEND_FLAG = 1;
                msg.m_count       = 1;
                msg.m_data[ 0 ]   = 7;
                fifoSendDatamsg( FIFO_RETURN, sizeof( msg ), (u8*) &msg );
                return;
            }
            if( ended == TRACK_CNT ) {
                MESSAGE_SEND_FLAG = 1;
                msg.m_count       = 1;
                msg.m_data[ 0 ]   = 8;
                fifoSendDatamsg( FIFO_RETURN, sizeof( msg ), (u8*) &msg );
                return;
            }
            if( ( looped_twice + ended ) >= TRACK_CNT ) {
                MESSAGE_SEND_FLAG = 1;
                msg.m_count       = 1;
                msg.m_data[ 0 ]   = 7;
                fifoSendDatamsg( FIFO_RETURN, sizeof( msg ), (u8*) &msg );
                return;
            }
        }
    }

    int readValue( int* p_pos ) {
        int v = 0;
        for( ;; ) {
            int data = SEQ_READ8( *p_pos );
            ( *p_pos )++;
            v = ( v << 7 ) | ( data & 0x7F );
            if( !( data & 0x80 ) ) break;
        }
        return v;
    }

    void updateSequenceNote( int p_track, playInfo* p_info ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) continue;
            chstat->m_vol  = p_info->m_vol;
            chstat->m_expr = p_info->m_expr;
            chstat->m_pan  = p_info->m_pan;
        }
    }

    void updateSequencePitchBend( int p_track, playInfo* p_info ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) { continue; }
            chstat->m_reg.m_timer
                = adjustPitchBend( chstat->m_freq, p_info->m_pitchb, p_info->m_pitchr );
        }
    }

    void updateSequenceModulation( int p_track, playInfo* p_info, int p_target ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) { continue; }
            if( p_target & BIT( 0 ) ) { chstat->m_modDepth = p_info->m_modDepth; }
            if( p_target & BIT( 1 ) ) { chstat->m_modSpeed = p_info->m_modSpeed; }
            if( p_target & BIT( 2 ) ) { chstat->m_modType = p_info->m_modType; }
            if( p_target & BIT( 3 ) ) { chstat->m_modRange = p_info->m_modRange; }
            if( p_target & BIT( 4 ) ) { chstat->m_modDelay = p_info->m_modDelay; }
        }
    }

    void updateSequencePortamento( adsrState* p_state, trackState* p_track ) {
        p_state->m_sweepPitch = p_track->m_sweepPitch;
        if( p_track->m_portakey & 0x80 ) {
            p_state->m_sweepLen = 0;
            p_state->m_sweepCnt = 0;
            return;
        }

        int diff = ( (int) p_track->m_portakey - (int) p_state->m_note ) << 22;
        p_state->m_sweepPitch += diff >> 16;

        if( p_track->m_portatime == 0 ) {
            p_state->m_sweepLen = ( p_state->m_count * 240 + SEQ_BPM - 1 ) / SEQ_BPM;
        } else {
            u32 sq_time         = p_track->m_portatime * p_track->m_portatime;
            int abs_sp          = p_state->m_sweepPitch;
            abs_sp              = abs_sp < 0 ? -abs_sp : abs_sp;
            p_state->m_sweepLen = ( abs_sp * sq_time ) >> 11;
        }
    }

    void trackTick( int p_trackId ) {
        returnMessage msg;
        trackState*   track = TRACKS + p_trackId;

        if( track->m_count ) {
            track->m_count--;
            if( track->m_count ) return;
        }

        while( !track->m_count ) {
            soundCommandType cmd = soundCommandType( SEQ_READ8( track->m_pos ) );
            track->m_pos++;
            msg.m_count     = 0;
            msg.m_channel   = p_trackId;
            msg.m_data[ 0 ] = cmd;
            msg.m_data[ 1 ] = SEQ_READ8( track->m_pos );
            msg.m_data[ 2 ] = SEQ_READ8( track->m_pos + 1 );
            msg.m_data[ 3 ] = SEQ_READ8( track->m_pos + 2 );
            if( cmd < SC_COMMAND_RANGE_START ) {
                // p_note-ON
                u8 vel = SEQ_READ8( track->m_pos );
                track->m_pos++;
                int len = readValue( &track->m_pos );
                if( track->m_waitmode ) { track->m_count = len; }

                track->m_playInfo.m_vel = vel;
                int handle = playNote( SEQUENCE_BANK, SEQUENCE_WAR, track->m_patch, cmd,
                                       track->m_priority, &track->m_playInfo, len, p_trackId );
                if( handle < 0 ) { continue; }
            } else
                switch( cmd ) {
                default: break;
                case SC_REST: {
                    track->m_count = readValue( &track->m_pos );
                    break;
                }
                case SC_PATCH_CHANGE: {
                    track->m_patch = readValue( &track->m_pos );
                    break;
                }
                case SC_JUMP: {
                    if( u32( track->m_pos ) > SEQ_READ24( track->m_pos ) ) {
                        track->m_trackLooped++;
                    }
                    track->m_pos = SEQ_READ24( track->m_pos );
                    break;
                }
                case SC_CALL: {
                    int dest     = SEQ_READ24( track->m_pos );
                    track->m_ret = track->m_pos + 3;
                    track->m_pos = dest;
                    break;
                }
                case SC_RANDOM: {
                    // TODO
                    // [statusByte] [min16] [max16]
                    track->m_pos += 5;
                    break;
                }
                case SC_UNKNOWN_1: {
                    // TODO
                    int t = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    if( t >= 0xB0 && t <= 0xBD ) track->m_pos++;
                    track->m_pos++;
                    break;
                }
                case SC_IF: {
                    // TODO
                    break;
                }
                case SC_UNKNOWN_2:
                case SC_UNKNOWN_3:
                case SC_UNKNOWN_4:
                case SC_UNKNOWN_5:
                case SC_UNKNOWN_6:
                case SC_UNKNOWN_7:
                case SC_UNKNOWN_8:
                case SC_UNKNOWN_9:
                case SC_UNKNOWN_10:
                case SC_UNKNOWN_11:
                case SC_UNKNOWN_12:
                case SC_UNKNOWN_13:
                case SC_UNKNOWN_14:
                case SC_UNKNOWN_15: {
                    // TODO
                    track->m_pos += 3;
                    break;
                }
                case SC_RET: {
                    track->m_pos = track->m_ret;
                    break;
                }
                case SC_PAN: {
                    track->m_playInfo.m_pan = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceNote( p_trackId, &track->m_playInfo );
                    break;
                }
                case SC_VOL: {
                    track->m_playInfo.m_vol = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceNote( p_trackId, &track->m_playInfo );
                    break;
                }
                case SC_MASTER_VOL: {
                    ADSR_MASTER_VOLUME = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    break;
                }
                case SC_TRANSPOSE:
                case SC_TIE:
                case SC_PRINT_VAR: {
                    // TODO
                    track->m_pos++;
                    break;
                }
                case SC_PORTAMENTO: {
                    track->m_portakey = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    break;
                }
                case SC_PORTAMENTO_TOGGLE: {
                    track->m_portakey &= ~0x80;
                    track->m_portakey |= ( !SEQ_READ8( track->m_pos ) ) << 7;
                    track->m_pos++;
                    break;
                }
                case SC_PORTAMENTO_TIME: {
                    track->m_portatime = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    break;
                }
                case SC_PITCH_BEND: {
                    track->m_playInfo.m_pitchb = (s8) SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequencePitchBend( p_trackId, &track->m_playInfo );
                    break;
                }
                case SC_PITCH_BEND_RANGE: {
                    track->m_playInfo.m_pitchr = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequencePitchBend( p_trackId, &track->m_playInfo );
                    break;
                }
                case SC_PRIORITY: {
                    track->m_priority = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    break;
                }
                case SC_NOTEWAIT: {
                    track->m_waitmode = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    break;
                }
                case SC_MODULATION_DEPTH: {
                    track->m_playInfo.m_modDepth = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceModulation( p_trackId, &track->m_playInfo, BIT( 0 ) );
                    break;
                }
                case SC_MODULATION_SPEED: {
                    track->m_playInfo.m_modSpeed = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceModulation( p_trackId, &track->m_playInfo, BIT( 1 ) );
                    break;
                }
                case SC_MODULATION_TYPE: {
                    track->m_playInfo.m_modType = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceModulation( p_trackId, &track->m_playInfo, BIT( 2 ) );
                    break;
                }
                case SC_MODULATION_RANGE: {
                    track->m_playInfo.m_modRange = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceModulation( p_trackId, &track->m_playInfo, BIT( 3 ) );
                    break;
                }
                case SC_ATTACK: {
                    track->m_attackRate = convertAttack( SEQ_READ8( track->m_pos ) );
                    track->m_pos++;
                    break;
                }
                case SC_DECAY: {
                    track->m_decayRate = convertFall( SEQ_READ8( track->m_pos ) );
                    track->m_pos++;
                    break;
                }
                case SC_SUSTAIN: {
                    track->m_sustainRate = convertSustain( SEQ_READ8( track->m_pos ) );
                    track->m_pos++;
                    break;
                }
                case SC_RELEASE: {
                    track->m_releaseRate = convertFall( SEQ_READ8( track->m_pos ) );
                    track->m_pos++;
                    break;
                }
                case SC_LOOP_START: {
                    track->m_loopcount = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    track->m_looppos = track->m_pos;
                    if( !track->m_loopcount ) track->m_loopcount = -1;
                    break;
                }
                case SC_LOOP_END: {
                    int shouldRepeat = 1;
                    if( track->m_loopcount > 0 ) shouldRepeat = --track->m_loopcount;
                    if( shouldRepeat ) track->m_pos = track->m_looppos;
                    if( ( shouldRepeat == 1 ) && ( track->m_loopcount == 0 ) )
                        track->m_trackLooped++;
                    break;
                }
                case SC_EXPR: {
                    track->m_playInfo.m_expr = SEQ_READ8( track->m_pos );
                    track->m_pos++;
                    updateSequenceNote( p_trackId, &track->m_playInfo );
                    break;
                }
                case SC_MODULATION_DELAY: {
                    track->m_playInfo.m_modDelay = SEQ_READ16( track->m_pos );
                    track->m_pos += 2;
                    updateSequenceModulation( p_trackId, &track->m_playInfo, BIT( 4 ) );
                    break;
                }
                case SC_SWEEP_PITCH: {
                    track->m_sweepPitch = SEQ_READ16( track->m_pos );
                    track->m_pos += 2;
                    break;
                }
                case SC_TEMPO: {
                    SEQ_BPM = SEQ_READ16( track->m_pos );
                    track->m_pos += 2;
                    break;
                }
                case SC_END: {
                    track->m_trackEnded = 1;
                    track->m_pos--;
                    return;
                }
                }
            if( msg.m_count ) { fifoSendDatamsg( FIFO_RETURN, sizeof( msg ), (u8*) &msg ); }
        }
    }
} // namespace SOUND::SSEQ
#endif
#endif
