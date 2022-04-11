// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)

#pragma once
#ifndef NO_SOUND

#include <nds.h>

#define FIFO_SNDSYS FIFO_USER_01
#define FIFO_RETURN FIFO_USER_02

namespace SOUND::SSEQ {
    constexpr u16    PITCH_TABLE_SIZE = 0x300;
    extern const u16 PITCH_TABLE[ PITCH_TABLE_SIZE ];

    void installSoundSys( );

    struct sampleInfo {
        enum waveType : u8 {
            WT_PCM8  = 0,
            WT_PCM16 = 1,
            WT_ADPCM = 2,
        };

        waveType m_waveType;
        u8       m_loop;       // Loop flag = TRUE|FALSE
        u16      m_sampleRate; // Sampling Rate
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

    enum soundCommandType : u8 {
        SC_COMMAND_RANGE_START = 0x80,

        SC_REST         = 0x80,
        SC_PATCH_CHANGE = 0x81,

        SC_OPEN_TRACK = 0x93,
        SC_JUMP       = 0x94,
        SC_CALL       = 0x95,

        SC_PARAMETER_RANDOM        = 0xA0,
        SC_PARAMETER_FROM_VARIABLE = 0xA1, // basic arithmetic
        SC_IF                      = 0xA2,

        // basic arithmetic block
        SC_ARITH_START       = 0xB0, // =
        SC_SET_VARIABLE      = 0xB0, // =
        SC_ADD_VARIABLE      = 0xB1, // +=
        SC_SUBTRACT_VARIABLE = 0xB2, // -=
        SC_MULTIPLY_VARIABLE = 0xB3, // *=
        SC_DIVIDE_VARIABLE   = 0xB4, // /=
        SC_SHIFT_VARIABLE    = 0xB5, // [Shift]
        SC_RANDOM_VARIABLE   = 0xB6, // [Rand]
        SC_UNKNOWN_9         = 0xB7, // ""
        SC_COMPARE_EQUAL     = 0xB8, // ==
        SC_COMPARE_GTOE      = 0xB9, // >=
        SC_COMPARE_GT        = 0xBA, // >
        SC_COMPARE_LTOE      = 0xBB, // <=
        SC_COMPARE_LT        = 0xBC, // <
        SC_COMPARE_NE        = 0xBD, // !=
        SC_ARITH_END         = 0xBD, // =

        SC_PAN               = 0xC0,
        SC_VOL               = 0xC1,
        SC_MASTER_VOL        = 0xC2,
        SC_TRANSPOSE         = 0xC3,
        SC_PITCH_BEND        = 0xC4,
        SC_PITCH_BEND_RANGE  = 0xC5,
        SC_PRIORITY          = 0xC6,
        SC_NOTEWAIT          = 0xC7,
        SC_TIE               = 0xC8,
        SC_PORTAMENTO        = 0xC9,
        SC_MODULATION_DEPTH  = 0xCA,
        SC_MODULATION_SPEED  = 0xCB,
        SC_MODULATION_TYPE   = 0xCC,
        SC_MODULATION_RANGE  = 0xCD,
        SC_PORTAMENTO_TOGGLE = 0xCE,
        SC_PORTAMENTO_TIME   = 0xCF,
        SC_ATTACK            = 0xD0,
        SC_DECAY             = 0xD1,
        SC_SUSTAIN           = 0xD2,
        SC_RELEASE           = 0xD3,
        SC_LOOP_START        = 0xD4,
        SC_EXPR              = 0xD5,
        SC_PRINT_VAR         = 0xD6,

        SC_MODULATION_DELAY = 0xE0,
        SC_TEMPO            = 0xE1,
        SC_SWEEP_PITCH      = 0xE2,
        SC_SWEEP_PITCH_ALT  = 0xE3,

        SC_LOOP_END     = 0xFC,
        SC_RET          = 0xFD,
        SC_EXTRA_TRACKS = 0xFE,
        SC_END          = 0xFF,
    };

    enum messageType : int {
        SNDSYS_PLAYSEQ,
        SNDSYS_STOPSEQ,
        SNDSYS_FADESEQ,
        SNDSYS_PAUSESEQ,
        SNDSYS_PLAY_SAMPLE,
        SNDSYS_STOP_SAMPLE,
        SNDSYS_VOLUME,
    };

    enum { STATUS_PLAYING, STATUS_STOPPED, STATUS_FADE_OUT, STATUS_FADE_IN, STATUS_PAUSED };

    struct soundReg {
        u32 m_cr;
        u32 m_source;
        u16 m_timer;
        u16 m_repeatPoint;
        u32 m_length;
    };

    struct sequenceData {
        void  *m_data;
        size_t m_size;
    };

    struct soundSysMessage {
        messageType m_message;
        union {
            struct {
                sequenceData m_sample;
                sampleInfo   m_sampleInfo;
                playInfo     m_playInfo;
            };
            int m_channel;
            u8  m_volume;
            struct {
                sequenceData m_seq;
                sequenceData m_bnk;
                sequenceData m_war[ 4 ];
            };
        };
    };

    struct returnMessage {
        enum msg : u8 {
            MSG_SEQUENCE_STOPPED      = 6,
            MSG_SEQUENCE_LOOPED_TWICE = 7,
            MSG_SEQUENCE_ENDED        = 8,
        };

        u8  m_count;
        msg m_data[ 4 ];
        u8  m_channel;
    };

#define fifoRetWait( p_ch )  while( !fifoCheckValue32( p_ch ) )
#define fifoRetValue( p_ch ) fifoGetValue32( p_ch )

    static inline u32 fifoGetRetValue( int p_ch ) {
        fifoRetWait( p_ch );
        return fifoRetValue( p_ch );
    }

    constexpr u8 NUM_BLOCKED_CHANNEL = 0;
    constexpr u8 NUM_CHANNEL         = 16;
    constexpr u8 TONE_CHANNEL_START  = 8;
    constexpr u8 TONE_CHANNEL_NUM    = 6;
    constexpr u8 NOISE_CHANNEL_START = 14;
    constexpr u8 NOISE_CHANNEL_NUM   = 2;

#ifdef ARM7

#define SCHANNEL_ACTIVE( ch ) ( SCHANNEL_CR( ch ) & SCHANNEL_ENABLE )

#define SOUND_FORMAT( p_a )       ( ( (int) ( p_a ) ) << 29 )
#define SOUND_LOOP( p_a )         ( ( p_a ) ? SOUND_REPEAT : SOUND_ONE_SHOT )
#define GETSAMP( p_a )            ( (void *) ( (char *) ( p_a ) + sizeof( sampleInfo ) ) )
#define INST_TYPE( p_a )          ( 0xFF & ( p_a ) )
#define INST_OFF( p_a )           ( ( p_a ) >> 8 )
#define GETINSTDATA( p_bnk, p_a ) ( (u8 *) ( (int) ( p_bnk ) + (int) INST_OFF( p_a ) ) )

    constexpr int ADSR_K_AMP2VOL = 723;
    constexpr int ADSR_THRESHOLD = ADSR_K_AMP2VOL * 128;

    struct noteDef {
        u16 m_wavid;
        u16 m_warid;
        u8  m_tnote;
        u8  m_attackRate, m_decayRate, m_sustainRate, m_releaseRate;
        u8  m_pan;
    };

    constexpr u8 CALLSTACK_SIZE = 3;
    constexpr u8 MAX_VAR        = 15;
    constexpr u8 NUM_VARS       = MAX_VAR + 1;
    constexpr u8 MAX_GLOB_VAR   = 15;
    constexpr u8 NUM_GLOB_VARS  = MAX_GLOB_VAR + 1;

    struct trackState {
        int      m_count;
        int      m_pos;
        int      m_priority;
        u16      m_patch;
        u16      m_waitmode;
        playInfo m_playInfo;
        int      m_attackRate, m_decayRate, m_sustainRate, m_releaseRate;
        int      m_loopcount, m_looppos;
        int      m_ret[ CALLSTACK_SIZE ];
        int      m_retpos;
        int      m_trackEnded;
        int      m_trackLooped;
        u8       m_portakey, m_portatime;
        s16      m_sweepPitch;
        s16      m_variables[ NUM_VARS ];
        u8       m_lastConditionTrue;
        u8       m_tiemode;
        u8       m_muteState;
    };

    struct adsrState {
        enum adsrStateType : int {
            ADSR_NONE = 0,
            ADSR_START,
            ADSR_ATTACK,
            ADSR_DECAY,
            ADSR_SUSTAIN,
            ADSR_RELEASE,
            ADSR_LOCKED, // for sample playback
        };

        adsrStateType m_state;

        int m_vol;
        int m_vel;
        int m_expr;
        int m_pan;
        int m_pan2;
        int m_ampl;
        int m_attackRate;
        int m_decayRate;
        int m_sustainRate;
        int m_releaseRate;
        int m_priority;
        int m_count;
        int m_track;
        u16 m_freq;
        u8  m_modType, m_modSpeed, m_modDepth, m_modRange;
        u16 m_modDelay, m_modDelayCnt, m_modCounter;
        u8  m_note, m_patch;
        u32 m_sweepLen, m_sweepCnt;
        s16 m_sweepPitch;

        soundReg m_reg;
    };

    extern adsrState ADSR_CHANNEL[ NUM_CHANNEL ];
    extern s16       GLOBAL_VARS[ NUM_GLOB_VARS ];

    volatile extern int SEQ_BPM;
    volatile extern int SEQ_STATUS;

    volatile extern int ADSR_MASTER_VOLUME;
    volatile extern int ADSR_FADE_TARGET_VOLUME;

    void sequenceTick( );
    void trackTick( int p_n );
    void updateSequencePortamento( adsrState *p_state, trackState *p_track );

    void playSequence( sequenceData *p_seq, sequenceData *p_bnk, sequenceData *p_war,
                       bool p_fadeIn = false );
    void stopSequence( );

    inline s8 nextFreeChannel( int p_priority = 0, u8 p_chStart = 0, u8 p_chEnd = NUM_CHANNEL ) {
        for( u8 i = p_chStart; i < p_chEnd; ++i ) {
            if( !SCHANNEL_ACTIVE( i ) && ADSR_CHANNEL[ i ].m_state != adsrState::ADSR_START ) {
                return i;
            }
        }
        u8  j    = -1;
        int ampl = 1;
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

    inline s8 nextFreeToneChannel( int p_priority = 0 ) {
        return nextFreeChannel( p_priority, TONE_CHANNEL_START,
                                TONE_CHANNEL_START + TONE_CHANNEL_NUM );
    }

    inline s8 nextFreeNoiseChannel( int p_priority = 0 ) {
        return nextFreeChannel( p_priority, NOISE_CHANNEL_START,
                                NOISE_CHANNEL_START + NOISE_CHANNEL_NUM );
    }

    int convertAttack( int p_attack );
    int convertFall( int p_fall );
    int convertSustain( int p_sustain );
    int getSoundSine( int p_arg );

    // This function was obtained through disassembly of Ninty's sound driver
    constexpr u16 adjustFreq( u16 p_baseFreq, int p_pitch ) {
        int shift = 0;
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

    constexpr u16 adjustFreq( u16 p_baseFreq, int p_noteN, int p_baseN ) {
        return adjustFreq( p_baseFreq, ( p_noteN - p_baseN ) * 64 );
    }

    constexpr u16 adjustPitchBend( u16 p_baseFreq, int p_pitchb, int p_pitchr ) {
        if( !p_pitchb ) { return p_baseFreq; }
        return adjustFreq( p_baseFreq, ( p_pitchb * p_pitchr ) >> 1 );
    }

#endif

#ifdef ARM9
    extern soundSysMessage CURRENT_SEQUENCE;

    int  playSample( void *p_data, const sampleInfo &p_sampleInfo, const playInfo &p_playInfo );
    void stopSample( int p_handle );

    void playSequence( u16 );
    void stopSequence( );

    void setMasterVolume( u8 p_volume );
#endif
} // namespace SOUND::SSEQ

#endif
