// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
#include <cstring>
#include <nds.h>
#include "sseq.h"

#ifndef MMOD
namespace SOUND::SSEQ {
    // info about the sample
    struct sampleInfo {
        enum waveType : u8 {
            WT_PCM8  = 0,
            WT_PCM16 = 1,
            WT_ADPCM = 2,
        };

        waveType m_waveType;
        u8       m_loop;      // Loop flag = TRUE|FALSE
        u16      m_ampleRate; // Sampling Rate
        u16 m_time; // (ARM7_CLOCK / nSampleRate) [ARM7_CLOCK: 33.513982MHz / 2 = 1.6756991 E +7]
        u16 m_loopOffset; // Loop Offset (expressed in words (32-bits))
        u32 m_nonLoopLen; // Non Loop Length (expressed in words (32-bits))
    };

    struct playInfo {
        u8  m_vol, m_vel, m_expr, m_pan, m_pitchr;
        s8  m_pitchb;
        u8  m_modType, m_modSpeed, m_modDepth, m_modRange;
        u16 m_modDelay;
    };

    struct noteDef {
        u16 m_wavid;
        u16 m_warid;
        u8  m_tnote;
        u8  m_attackRate, m_decayRate, m_sustainRate, m_releaseRate;
        u8  m_pan;
    };

    struct trackState {
        static constexpr u8 CLLSTCK_LIMIT = 3;

        s32      m_count;
        s32      m_pos;
        s32      m_priority;
        u16      m_patch;
        u16      m_waitmode;
        playInfo m_playInfo;
        s32      m_attackRate, m_decayRate, m_sustainRate, m_releaseRate;
        s32      m_loopcount, m_looppos;
        s32      m_ret[ CLLSTCK_LIMIT ];
        u8       m_retpos;
        s32      m_trackEnded;
        s32      m_trackLooped;
        u8       m_portakey, m_portatime;
        s16      m_sweepPitch;
    };

    s32          TRACK_CNT         = 0;
    u8*          SEQUENCE_DATA     = NULL;
    void*        SEQUENCE_BANK     = NULL;
    void*        SEQUENCE_WAR[ 4 ] = { NULL, NULL, NULL, NULL };
    trackState   TRACKS[ NUM_CHANNEL ];
    s32          MESSAGE_SEND_FLAG = 0;
    volatile s32 SEQ_BPM           = 0;

    void trackTick( s32 n );
    void updateSequencePortamento( adsrState* p_state, trackState* p_track );

    // This function was obtained through disassembly of Ns32y's sound driver
    u16 adjustFreq( u16 p_baseFreq, s32 p_pitch ) {
        s32 shift = 0;
        p_pitch   = -p_pitch;
        while( p_pitch < 0 ) {
            shift--;
            p_pitch += PITCH_TABLE_SIZE;
        }
        while( p_pitch >= PITCH_TABLE_SIZE ) {
            shift++;
            p_pitch -= PITCH_TABLE_SIZE;
        }

        u64 freq = (u64) p_baseFreq * ( (u32) PITCH_TABLE[ p_pitch ] + 0x10000 );
        shift -= 16;
        if( shift <= 0 ) {
            freq >>= -shift;
        } else if( shift < 32 ) {
            if( freq & ( ( ~0ULL ) << ( 32 - shift ) ) ) { return 0xFFFF; }
            freq <<= shift;
        } else {
            return 0x10;
        }
        if( freq < 0x10 ) { return 0x10; }
        if( freq > 0xFFFF ) { return 0xFFFF; }
        return (u16) freq;
    }

    static inline u16 adjustFreq( u16 p_baseFreq, s32 p_noteN, s32 p_baseN ) {
        return adjustFreq( p_baseFreq, ( p_noteN - p_baseN ) * 64 );
    }

    static inline u16 adjustPitchBend( u16 p_baseFreq, s32 p_pitchb, s32 p_pitchr ) {
        if( !p_pitchb ) { return p_baseFreq; }
        return adjustFreq( p_baseFreq, ( p_pitchb * p_pitchr ) >> 1 );
    }

#define SOUND_FORMAT( p_a ) ( ( (s32) ( p_a ) ) << 29 )
#define SOUND_LOOP( p_a )   ( ( p_a ) ? SOUND_REPEAT : SOUND_ONE_SHOT )
#define GETSAMP( p_a )      ( (void*) ( (char*) ( p_a ) + sizeof( sampleInfo ) ) )

    sampleInfo* getWav( void* p_war, s32 p_id ) {
        return (sampleInfo*) ( s32( p_war ) + ( (s32*) ( s32( p_war ) + 60 ) )[ p_id ] );
    }

    u32 getInstr( void* p_bnk, s32 p_id ) {
        return *(u32*) ( s32( p_bnk ) + 60 + 4 * p_id );
    }

#define INST_TYPE( p_a ) ( 0xFF & ( p_a ) )
#define INST_OFF( p_a )  ( ( p_a ) >> 8 )

#define GETINSTDATA( p_bnk, p_a ) ( (u8*) ( (s32) ( p_bnk ) + (s32) INST_OFF( p_a ) ) )

    s32 nextFreeChannel( s32 p_priority, u8 p_chStart = 0, u8 p_chEnd = NUM_CHANNEL ) {
        for( u8 i = p_chStart; i < p_chEnd; ++i ) {
            if( !SCHANNEL_ACTIVE( i ) && ADSR_CHANNEL[ i ].m_state != adsrState::ADSR_START ) {
                return i;
            }
        }
        s32 j = -1, ampl = 1;
        for( u8 i = p_chStart; i < p_chEnd; ++i ) {
            if( ADSR_CHANNEL[ i ].m_state == adsrState::ADSR_RELEASE
                && ADSR_CHANNEL[ i ].m_ampl < ampl ) {
                ampl = ADSR_CHANNEL[ i ].m_ampl;
                j    = i;
            }
        }

        if( j != -1 ) { return j; }

        for( u8 i = p_chStart; i < p_chEnd; ++i ) {
            if( ADSR_CHANNEL[ i ].m_priority < p_priority ) { return i; }
        }
        return -1;
    }

    s32 nextFreeToneChannel( s32 p_priority ) {
        return nextFreeChannel( p_priority, TONE_CHANNEL_START,
                                TONE_CHANNEL_START + TONE_CHANNEL_NUM );
    }

    s32 nextFreeNoiseChannel( s32 p_priority ) {
        return nextFreeChannel( p_priority, NOISE_CHANNEL_START,
                                NOISE_CHANNEL_START + NOISE_CHANNEL_NUM );
    }

    /*
     * @brief: plays the specified note on a free channel.
     */
    s32 playNote( void* p_bnk, void** p_war, s32 p_instr, s32 p_note, s32 p_priority,
                  playInfo* p_playInfo, s32 p_duration, s32 p_track ) {
        s32 isPsg   = 0;
        s32 channel = nextFreeChannel( p_priority );
        if( channel < 0 ) { return -1; }

        adsrState* chstat = ADSR_CHANNEL + channel;

        u32         inst    = getInstr( p_bnk, p_instr );
        u8*         insdata = GETINSTDATA( p_bnk, inst );
        noteDef*    notedef = NULL;
        sampleInfo* wavinfo = NULL;
        s32         fRecord = INST_TYPE( inst );
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
                s32 rn     = p_note - insdata[ 0 ];
                s32 offset = 2 + rn * ( 2 + sizeof( noteDef ) );
                fRecord    = insdata[ offset ];
                insdata += offset + 2;
                goto _ReadRecord;
            } else {
                return -1;
            }
        } else if( fRecord == 17 ) {
            u8 reg;
            for( reg = 0; reg < 8; reg++ ) {
                if( p_note <= insdata[ reg ] ) { break; }
            }
            if( reg == 8 ) { return -1; }

            s32 offset = 8 + reg * ( 2 + sizeof( noteDef ) );
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

    static inline void prepareTrack( s32 p_track, s32 p_pos ) {
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

        s32 pos = 0;

        if( *SEQUENCE_DATA == 0xFE ) {
            // Prepare extra TRACKS
            for( pos = 3; SEQ_READ8( pos ) == 0x93; TRACK_CNT++, pos += 3 ) {
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
        s32 looped_twice = 0;
        s32 ended        = 0;

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

    s32 readValue( s32* p_pos ) {
        s32 v = 0;
        for( ;; ) {
            s32 data = SEQ_READ8( *p_pos );
            ( *p_pos )++;
            v = ( v << 7 ) | ( data & 0x7F );
            if( !( data & 0x80 ) ) break;
        }
        return v;
    }

    void updateSequenceNote( s32 p_track, playInfo* p_info ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) continue;
            chstat->m_vol  = p_info->m_vol;
            chstat->m_expr = p_info->m_expr;
            chstat->m_pan  = p_info->m_pan;
        }
    }

    void updateSequencePitchBend( s32 p_track, playInfo* p_info ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) { continue; }
            chstat->m_reg.m_timer
                = adjustPitchBend( chstat->m_freq, p_info->m_pitchb, p_info->m_pitchr );
        }
    }

    void updateSequenceModulation( s32 p_track, playInfo* p_info, s32 what ) {
        for( u8 i = 0; i < NUM_CHANNEL; i++ ) {
            adsrState* chstat = ADSR_CHANNEL + i;
            if( chstat->m_track != p_track ) { continue; }
            if( what & BIT( 0 ) ) { chstat->m_modDepth = p_info->m_modDepth; }
            if( what & BIT( 1 ) ) { chstat->m_modSpeed = p_info->m_modSpeed; }
            if( what & BIT( 2 ) ) { chstat->m_modType = p_info->m_modType; }
            if( what & BIT( 3 ) ) { chstat->m_modRange = p_info->m_modRange; }
            if( what & BIT( 4 ) ) { chstat->m_modDelay = p_info->m_modDelay; }
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

    void trackTick( s32 p_trackId ) {
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
                s32 len = readValue( &track->m_pos );
                if( track->m_waitmode ) track->m_count = len;

                track->m_playInfo.m_vel = vel;
                s32 handle = playNote( SEQUENCE_BANK, SEQUENCE_WAR, track->m_patch, cmd,
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
                    s32 dest                        = SEQ_READ24( track->m_pos );
                    track->m_ret[ track->m_retpos ] = track->m_pos + 3;
                    track->m_pos                    = dest;
                    if( track->m_retpos + 1 < trackState::CLLSTCK_LIMIT ) { track->m_retpos++; }
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
                    s32 t = SEQ_READ8( track->m_pos );
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
                    track->m_pos = track->m_ret[ track->m_retpos ];
                    if( track->m_retpos > 0 ) { --track->m_retpos; }
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
                    s32 shouldRepeat = 1;
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
