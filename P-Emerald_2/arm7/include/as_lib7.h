/*

  Advanced Sound Library (ASlib)
  ------------------------------

  file        : sound7.c
  author      : Lasorsa Yohan (Noda)
  description : ARM7 sound definitions

  history :

  28/11/2007 - v1.0
  = Original release

  */

#ifndef __SOUND7_H__
#define __SOUND7_H__

#define NDS_IPC_INCLUDE

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef SOUND_VOL
#define SOUND_VOL(n)	(n)
#endif
#ifndef SOUND_FREQ
#define SOUND_FREQ(n)	((-0x1000000 / (n)))
#endif
#ifndef SOUND_ENABLE
#define SOUND_ENABLE	BIT(15)
#endif
#ifndef SOUND_REPEAT
#define SOUND_REPEAT    BIT(27)
#endif
#ifndef SOUND_ONE_SHOT
#define SOUND_ONE_SHOT  BIT(28)
#endif
#ifndef SOUND_FORMAT_16BIT
#define SOUND_FORMAT_16BIT (1<<29)
#endif
#ifndef SOUND_FORMAT_8BIT
#define SOUND_FORMAT_8BIT	(0<<29)
#endif
#ifndef SOUND_FORMAT_PSG
#define SOUND_FORMAT_PSG    (3<<29)
#endif
#ifndef SOUND_FORMAT_ADPCM
#define SOUND_FORMAT_ADPCM  (2<<29)
#endif
#ifndef SOUND_16BIT
#define SOUND_16BIT      (1<<29)
#endif
#ifndef SOUND_8BIT
#define SOUND_8BIT       (0)
#endif

#ifndef SOUND_PAN
#define SOUND_PAN(n)	((n) << 16)
#endif

#ifndef SCHANNEL_ENABLE
#define SCHANNEL_ENABLE BIT(31)
#endif

    //---------------------------------------------------------------------------------
    // registers
    //---------------------------------------------------------------------------------
#ifndef SCHANNEL_CR
#define SCHANNEL_CR(n)				(*(vuint32*)(0x04000400 + ((n)<<4)))
#endif
#ifndef SCHANNEL_VOL
#define SCHANNEL_VOL(n)				(*(vuint8*)(0x04000400 + ((n)<<4)))
#endif
#ifndef SCHANNEL_PAN
#define SCHANNEL_PAN(n)				(*(vuint8*)(0x04000402 + ((n)<<4)))
#endif
#ifndef SCHANNEL_SOURCE
#define SCHANNEL_SOURCE(n)			(*(vuint32*)(0x04000404 + ((n)<<4)))
#endif
#ifndef SCHANNEL_TIMER
#define SCHANNEL_TIMER(n)			(*(vint16*)(0x04000408 + ((n)<<4)))
#endif
#ifndef SCHANNEL_REPEAT_POINT
#define SCHANNEL_REPEAT_POINT(n)	(*(vuint16*)(0x0400040A + ((n)<<4)))
#endif
#ifndef SCHANNEL_LENGTH
#define SCHANNEL_LENGTH(n)			(*(vuint32*)(0x0400040C + ((n)<<4)))
#endif

#ifndef SOUND_CR
#define SOUND_CR          (*(vuint16*)0x04000500)
#endif
#ifndef SOUND_MASTER_VOL
#define SOUND_MASTER_VOL  (*(vuint8*)0x04000500)
#endif
    //---------------------------------------------------------------------------------
    // not sure on the following
    //---------------------------------------------------------------------------------
#ifndef SOUND_BIAS
#define SOUND_BIAS        (*(vuint16*)0x04000504)
#endif
#ifndef SOUND508
#define SOUND508          (*(vuint16*)0x04000508)
#endif
#ifndef SOUND510
#define SOUND510          (*(vuint16*)0x04000510)
#endif
#ifndef SOUND514
#define SOUND514		  (*(vuint16*)0x04000514)
#endif
#ifndef SOUND518
#define SOUND518          (*(vuint16*)0x04000518)
#endif
#ifndef SOUND51C
#define SOUND51C          (*(vuint16*)0x0400051C)
#endif


    // mp3 static defines
#define AS_DECODEBUFFER_SIZE    MAX_NCHAN * MAX_NGRAN * MAX_NSAMP   

    // locate the IPC structure after the libnds one
#define IPC_Sound   ((IPC_SoundSystem*)((u32)(IPC) + sizeof(TransferRegion)))

    // mp3 commands
    typedef enum {
        // internal commands
        MP3CMD_NONE = 0,
        MP3CMD_MIX = 1,
        MP3CMD_MIXING = 2,
        MP3CMD_WAITING = 4,

        // user commands
        MP3CMD_INIT = 8,
        MP3CMD_STOP = 16,
        MP3CMD_PLAY = 32,
        MP3CMD_PAUSE = 64,
        MP3CMD_SETRATE = 128

    } MP3Command;

    // sound commands
    typedef enum {
        // internal commands
        SNDCMD_NONE = 0,
        SNDCMD_DELAY = 1,

        // user commands
        SNDCMD_STOP = 2,
        SNDCMD_PLAY = 4,
        SNDCMD_SETVOLUME = 8,
        SNDCMD_SETPAN = 16,
        SNDCMD_SETRATE = 32,
        SNDCMD_SETMASTERVOLUME = 64

    } SoundCommand;

    // mp3 states
    typedef enum {
        MP3ST_STOPPED = 0,
        MP3ST_PLAYING = 1,
        MP3ST_PAUSED = 2,
        MP3ST_OUT_OF_DATA = 4,
        MP3ST_DECODE_ERROR = 8

    } MP3Status;

    // ASlib modes
    typedef enum {
        AS_MODE_MP3 = 1,        // use mp3
        AS_MODE_SURROUND = 2,   // use surround
        AS_MODE_16CH = 4,       // use all DS channels
        AS_MODE_8CH = 8         // use DS channels 1-8 only

    } AS_MODE;

    // delay values
    typedef enum {
        AS_NO_DELAY = 0,    // 0 ms delay
        AS_SURROUND = 1,    // 16 ms delay
        AS_REVERB = 4,      // 66 ms delay

    } AS_DELAY;

    // sound formats
    typedef enum {
        AS_PCM_8BIT = 0,
        AS_PCM_16BIT = 1,
        AS_ADPCM = 2

    } AS_SOUNDFORMAT;

    // sound info
    typedef struct {
        u8  *data;
        u32 size;
        u8  format;
        s32 rate;
        u8  volume;
        s8  pan;
        u8  loop;
        u8  priority;
        u8  delay;

    } SoundInfo;

    // sound channel info
    typedef struct {
        SoundInfo snd;
        u8  busy;
        u8  reserved;
        s8  volume;
        s8  pan;
        u8  cmd;

    } SoundChannel;

    // MP3 player info
    typedef struct {
        s8  *mixbuffer;
        u32 buffersize;
        s32 rate;
        u32 state;
        u32 soundcursor;
        u32 numsamples;
        s32 prevtimer;
        u8  *mp3buffer;
        u32 mp3buffersize;
        u32 mp3filesize;
        u32 cmd;
        u8  channelL, channelR;
        u8  loop;
        u8  stream;
        u8  needdata;
        u8  delay;

    } MP3Player;

    // IPC structure for the sound system
    typedef struct {
        MP3Player mp3;
        SoundChannel chan[ 16 ];
        u8 surround;
        u8 num_chan;
        u8 volume;

    } IPC_SoundSystem;


    // the sound engine, must be called each VBlank
    void AS_SoundVBL( );

    // the mp3 decoding engine, must be called on a regular basis (like after VBlank)
    void AS_MP3Engine( );


#ifdef __cplusplus
}
#endif

#endif

