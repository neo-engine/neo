// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)

#pragma once
#ifndef NO_SOUND
#ifndef MMOD

#include <nds.h>

#define FIFO_SNDSYS FIFO_USER_01
#define FIFO_RETURN FIFO_USER_02

namespace SOUND::SSEQ {
    constexpr u16    PITCH_TABLE_SIZE = 0x300;
    extern const u16 PITCH_TABLE[ PITCH_TABLE_SIZE ];

    void installSoundSys( );

    enum soundCommandType : u8 {
        SC_COMMAND_RANGE_START = 0x80,

        SC_REST         = 0x80,
        SC_PATCH_CHANGE = 0x81,

        SC_OPEN_TRACK = 0x93,
        SC_JUMP       = 0x94,
        SC_CALL       = 0x95,

        SC_RANDOM    = 0xA0,
        SC_UNKNOWN_1 = 0xA1, // basic arithmetic
        SC_IF        = 0xA2,

        // basic arithmetic block
        SC_UNKNOWN_2  = 0xB0, // =
        SC_UNKNOWN_3  = 0xB1, // +=
        SC_UNKNOWN_4  = 0xB2, // -=
        SC_UNKNOWN_5  = 0xB3, // *=
        SC_UNKNOWN_6  = 0xB4, // /=
        SC_UNKNOWN_7  = 0xB5, // [Shift]
        SC_UNKNOWN_8  = 0xB6, // [Rand]
        SC_UNKNOWN_9  = 0xB7, // ""
        SC_UNKNOWN_10 = 0xB8, // ==
        SC_UNKNOWN_11 = 0xB9, // >=
        SC_UNKNOWN_12 = 0xBA, // >
        SC_UNKNOWN_13 = 0xBB, // <=
        SC_UNKNOWN_14 = 0xBC, // <
        SC_UNKNOWN_15 = 0xBD, // !=

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

        SC_SWEEP_PITCH = 0xE3,

        SC_LOOP_END     = 0xFC,
        SC_RET          = 0xFD,
        SC_EXTRA_TRACKS = 0xFE,
        SC_END          = 0xFF,
    };

    enum messageType : u16 {
        /*SNDSYS_PLAY = 1, SNDSYS_STOP,*/ SNDSYS_PLAYSEQ,
        SNDSYS_STOPSEQ,
        SNDSYS_FADESEQ,
        SNDSYS_PAUSESEQ
    };

    enum { STATUS_PLAYING, STATUS_STOPPED, STATUS_FADING, STATUS_PAUSED };

    struct soundReg {
        u32 m_cr;
        u32 m_source;
        u16 m_timer;
        u16 m_repeatPoint;
        u32 m_length;
    };

    struct sequenceData {
        void *m_data;
        u32   m_size;
    };

    struct soundSysMessage {
        messageType m_message;
        union {
            /*
            struct
            {
                    soundReg m_sndreg;
                    u8 m_attackRate,
                    u8 m_d;
                    u8 m_s;
                    u8 m_r;
                    u8 m_vol;
                    u8 m_vel;
                    u8 m_pan;
                    u8 m_padding;
            };
            int m_ch;
            */
            struct {
                sequenceData m_seq;
                sequenceData m_bnk;
                sequenceData m_war[ 4 ];
            };
        };
    };

    struct returnMessage {
        u8 m_count;
        u8 m_data[ 4 ];
        u8 m_channel;
    };

#define fifoRetWait( p_ch )  while( !fifoCheckValue32( p_ch ) )
#define fifoRetValue( p_ch ) fifoGetValue32( p_ch )

    static inline u32 fifoGetRetValue( int p_ch ) {
        fifoRetWait( p_ch );
        return fifoRetValue( p_ch );
    }

    constexpr u8 NUM_CHANNEL         = 16;
    constexpr u8 TONE_CHANNEL_START  = 8;
    constexpr u8 TONE_CHANNEL_NUM    = 6;
    constexpr u8 NOISE_CHANNEL_START = 14;
    constexpr u8 NOISE_CHANNEL_NUM   = 2;

#ifdef ARM7

#define SCHANNEL_ACTIVE( ch ) ( SCHANNEL_CR( ch ) & SCHANNEL_ENABLE )

    constexpr int ADSR_K_AMP2VOL = 723;
    constexpr int ADSR_THRESHOLD = ADSR_K_AMP2VOL * 128;

    struct adsrState {
        enum adsrStateType : int {
            ADSR_NONE = 0,
            ADSR_START,
            ADSR_ATTACK,
            ADSR_DECAY,
            ADSR_SUSTAIN,
            ADSR_RELEASE
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

    volatile extern int SEQ_BPM;
    volatile extern int SEQ_STATUS;

    volatile extern int ADSR_MASTER_VOLUME;

    void seq_tick( );

    void playSequence( sequenceData *p_seq, sequenceData *p_bnk, sequenceData *p_war );
    void stopSequence( );

    inline s8 nextFreeChannelInRange( u8 p_chStart, u8 p_chEnd ) {
        for( u8 i = p_chStart; i < p_chEnd; ++i ) {
            if( !SCHANNEL_ACTIVE( i ) ) { return i; }
        }
        return -1;
    }
    inline s8 nextFreeChannel( ) {
        return nextFreeChannelInRange( 0, NUM_CHANNEL );
    }
    inline s8 nextFreeToneChannel( ) {
        return nextFreeChannelInRange( TONE_CHANNEL_START, TONE_CHANNEL_START + TONE_CHANNEL_NUM );
    }
    inline s8 nextFreeNoiseChannel( ) {
        return nextFreeChannelInRange( NOISE_CHANNEL_START,
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

    void playSequence( u16 );
    void stopSequence( );

#endif
} // namespace SOUND::SSEQ

#endif
#endif
