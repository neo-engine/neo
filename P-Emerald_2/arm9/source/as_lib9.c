/*

  Advanced Sound Library (ASlib)
  ------------------------------

  file        : sound9.c 
  author      : Lasorsa Yohan (Noda)
  description : ARM7 sound functions

  history : 

    02/12/2007 - v1.0
      = Original release

*/

#include <nds.h>
#include <malloc.h>
#include <string.h>

#include "as_lib9.h"


#ifdef __cplusplus
extern "C" {
#endif

// variable for the mp3 file stream
MP3FILE *mp3file = NULL;
u8* mp3filebuffer = NULL;

// default settings for sounds
u8 as_default_format;
s32 as_default_rate; 
u8 as_default_delay;

// initialize the ASLib
void AS_Init(u8 mode)
{
    int i, nb_chan = 16;
    
    // initialize default settings
    as_default_format = AS_PCM_8BIT;
    as_default_rate = 22050; 
    as_default_delay = AS_SURROUND;

    // initialize channels
    for(i = 0; i < 16; i++) {
        IPC_Sound->chan[i].busy = false;
        IPC_Sound->chan[i].reserved = false;
        IPC_Sound->chan[i].volume = 0;
        IPC_Sound->chan[i].pan = 64;
        IPC_Sound->chan[i].cmd = SNDCMD_NONE;
    }

    // use only 8 channels
    if(mode & AS_MODE_8CH) {

        nb_chan = 8;
        for(i = 8; i < 16; i++)
            IPC_Sound->chan[i].reserved = true;
    }
    
    // use surround
    if(mode & AS_MODE_SURROUND) {

        IPC_Sound->surround = true;
        for(i = nb_chan / 2; i < nb_chan; i++)
            IPC_Sound->chan[i].reserved = true;
    
    } else {
        IPC_Sound->surround = false;
    }

    IPC_Sound->num_chan = nb_chan / 2;

    // use mp3
    if(mode & AS_MODE_MP3) {
    
        IPC_Sound->mp3.mixbuffer = (s8*)memalign(4, AS_AUDIOBUFFER_SIZE * 2);
        IPC_Sound->mp3.buffersize = AS_AUDIOBUFFER_SIZE / 2;
        IPC_Sound->mp3.channelL = 0;
        IPC_Sound->mp3.prevtimer = 0;
        IPC_Sound->mp3.soundcursor = 0;
        IPC_Sound->mp3.numsamples = 0;
        IPC_Sound->mp3.delay = AS_SURROUND;
        IPC_Sound->mp3.cmd = MP3CMD_INIT;
        IPC_Sound->mp3.state = MP3ST_STOPPED;
        
        IPC_Sound->chan[0].reserved = true;
        
        if(IPC_Sound->surround) {
            IPC_Sound->mp3.channelR = nb_chan / 2;
            IPC_Sound->chan[nb_chan / 2].reserved = true;
        } else {
            IPC_Sound->mp3.channelR = 1;
            IPC_Sound->chan[1].reserved = true;
        }

        IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan = 64;
        AS_SetMP3Volume(127);

        // wait for the mp3 engine to be initialized
        while(IPC_Sound->mp3.cmd & MP3CMD_INIT)
            swiWaitForVBlank();
    }
    AS_SetMasterVolume(127);
}

// play a sound using the priority system
// return the sound channel allocated or -1 if the sound was skipped
int AS_SoundPlay(SoundInfo sound) 
{
    int i, free_ch = -1, minp_ch = -1;

    // search a free channel
    for(i = 0; i < 16; i++) {
        if(!(IPC_Sound->chan[i].reserved || IPC_Sound->chan[i].busy))
            free_ch = i;
    }

    // if a free channel was found
    if(free_ch != -1) {

        // play the sound
        AS_SoundDirectPlay(free_ch, sound);
        return free_ch;
    
    } else {
    
        // find the channel with the least priority
        for(i = 0; i < 16; i++) {
            if(!IPC_Sound->chan[i].reserved) {
                if(minp_ch == -1)
                    minp_ch = i;
                else if(IPC_Sound->chan[i].snd.priority < IPC_Sound->chan[minp_ch].snd.priority)
                    minp_ch = i;
            }
        }
        
        // if the priority of the found channel is <= the one of the sound
        if(IPC_Sound->chan[i].snd.priority <= sound.priority) {
        
            // play the sound
            AS_SoundDirectPlay(minp_ch, sound);
            return minp_ch;

        } else {
        
            // skip the sound
            return -1;
        }
    }
}

// set the panning of a sound (0=left, 64=center, 127=right)
void AS_SetSoundPan(u8 chan, u8 pan)
{
    IPC_Sound->chan[chan].snd.pan = pan;

    if(IPC_Sound->surround) {
        
        int difference = ((pan - 64) >> AS_PANNING_SHIFT) * IPC_Sound->chan[chan].snd.volume / AS_VOL_NORMALIZE;
    
        IPC_Sound->chan[chan].pan = 0;
        IPC_Sound->chan[chan].volume = IPC_Sound->chan[chan].snd.volume + difference;

        if(IPC_Sound->chan[chan].volume < 0)
            IPC_Sound->chan[chan].volume = 0;
    
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETVOLUME;
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETPAN;        

        IPC_Sound->chan[chan + IPC_Sound->num_chan].pan = 127;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].volume = IPC_Sound->chan[chan].snd.volume - difference;

        if(IPC_Sound->chan[chan + IPC_Sound->num_chan].volume < 0)
            IPC_Sound->chan[chan + IPC_Sound->num_chan].volume = 0;
            
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETVOLUME;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETPAN;
    
    } else {
    
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETPAN;
        IPC_Sound->chan[chan].pan = pan;
        
    }
}

// set the volume of a sound (0..127)
void AS_SetSoundVolume(u8 chan, u8 volume)
{   
    if(IPC_Sound->surround) {
        IPC_Sound->chan[chan].snd.volume = volume * AS_BASE_VOLUME / 127;
        AS_SetSoundPan(chan, IPC_Sound->chan[chan].snd.pan);
    } else {
        IPC_Sound->chan[chan].volume = volume;
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETVOLUME;
    }
}

// set the sound sample rate
void AS_SetSoundRate(u8 chan, u32 rate)
{
    IPC_Sound->chan[chan].snd.rate = rate;
    IPC_Sound->chan[chan].cmd |= SNDCMD_SETRATE;
    
    if(IPC_Sound->surround) {
        IPC_Sound->chan[chan + IPC_Sound->num_chan].snd.rate = rate;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETRATE;
    }
}

// play a sound directly using the given channel
void AS_SoundDirectPlay(u8 chan, SoundInfo sound)
{
    IPC_Sound->chan[chan].snd = sound;
    IPC_Sound->chan[chan].busy = true;
    IPC_Sound->chan[chan].cmd = SNDCMD_PLAY;
    IPC_Sound->chan[chan].volume = sound.volume;
    
    if(IPC_Sound->surround) {
        IPC_Sound->chan[chan + IPC_Sound->num_chan].snd = sound;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].busy = true;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd = SNDCMD_DELAY;

        // set the correct surround volume & pan
        AS_SetSoundVolume(chan, sound.volume);
    }
}

// fill the given buffer with the required amount of mp3 data
void AS_MP3FillBuffer(u8 *buffer, u32 bytes)
{
    u32 read = FILE_READ(buffer, 1, bytes, mp3file);
    
    if((read < bytes) && IPC_Sound->mp3.loop) {
        FILE_SEEK(mp3file, 0, SEEK_SET);
        FILE_READ(buffer + read, 1, bytes - read, mp3file);
    }
}

// play an mp3 directly from memory
void AS_MP3DirectPlay(u8 *mp3_data, u32 size) 
{
    if(IPC_Sound->mp3.state & (MP3ST_PLAYING | MP3ST_PAUSED))
        return;

    IPC_Sound->mp3.mp3buffer = mp3_data;
    IPC_Sound->mp3.mp3filesize = size;
    IPC_Sound->mp3.stream = false;
    IPC_Sound->mp3.cmd = MP3CMD_PLAY;
}

// play an mp3 stream
void AS_MP3StreamPlay(char *path) 
{
    if(IPC_Sound->mp3.state & (MP3ST_PLAYING | MP3ST_PAUSED))
        return;

    if(mp3file)
        FILE_CLOSE(mp3file);

    mp3file = FILE_OPEN(path);

    if(mp3file) {
    
        // allocate the file buffer the first time
        if(!mp3filebuffer) 
        {
            mp3filebuffer = (u8*)memalign(4, AS_FILEBUFFER_SIZE * 2);   // 2 buffers, to swap
            IPC_Sound->mp3.mp3buffer = mp3filebuffer;
            IPC_Sound->mp3.mp3buffersize = AS_FILEBUFFER_SIZE;
        }
        
        // get the file size
        FILE_SEEK(mp3file, 0, SEEK_END);
        IPC_Sound->mp3.mp3filesize = FILE_TELL(mp3file);
        
        // fill the file buffer
        FILE_SEEK(mp3file, 0, SEEK_SET); 
        AS_MP3FillBuffer(mp3filebuffer, AS_FILEBUFFER_SIZE * 2);
        
        // start playing
        IPC_Sound->mp3.stream = true;
        IPC_Sound->mp3.cmd = MP3CMD_PLAY;
    }
    else{
        AS_MP3Stop();
        fprintf(stderr,"AS_MP3StreamPlay: ERROR: FILE NOT FOUND");
    }
}

// set the mp3 panning (0=left, 64=center, 127=right)
void AS_SetMP3Pan(u8 pan)
{
    int difference = ((pan - 64) >> AS_PANNING_SHIFT) * IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume / AS_VOL_NORMALIZE;

    IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan = pan;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].pan = 0;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].volume = IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume - difference;

    if(IPC_Sound->chan[IPC_Sound->mp3.channelL].volume < 0)
        IPC_Sound->chan[IPC_Sound->mp3.channelL].volume = 0;

    IPC_Sound->chan[IPC_Sound->mp3.channelL].cmd |= SNDCMD_SETVOLUME;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].cmd |= SNDCMD_SETPAN;
    
    IPC_Sound->chan[IPC_Sound->mp3.channelR].pan = 127;
    IPC_Sound->chan[IPC_Sound->mp3.channelR].volume = IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume + difference;

    if(IPC_Sound->chan[IPC_Sound->mp3.channelR].volume < 0)
        IPC_Sound->chan[IPC_Sound->mp3.channelR].volume = 0;
        
    IPC_Sound->chan[IPC_Sound->mp3.channelR].cmd |= SNDCMD_SETVOLUME;
    IPC_Sound->chan[IPC_Sound->mp3.channelR].cmd |= SNDCMD_SETPAN;
        
}

 void AS_ReserveChannel(u8 channel) 
{ 
    IPC_Sound->chan[channel].reserved = true; 
}

// set the master volume (0..127)
 void AS_SetMasterVolume(u8 volume) 
{
    IPC_Sound->volume = volume;
    IPC_Sound->chan[0].cmd |= SNDCMD_SETMASTERVOLUME;
}

// set the default sound settings
 void AS_SetDefaultSettings(u8 format, s32 rate, u8 delay)
{
    as_default_format = format;
    as_default_rate = rate; 
    as_default_delay = delay;
}

// play a sound using the priority system with the default settings
// return the sound channel allocated or -1 if the sound was skipped
 int AS_SoundDefaultPlay(u8 *data, u32 size, u8 volume, u8 pan, u8 loop, u8 prio)
{
    SoundInfo snd = { 
        data, 
        size, 
        as_default_format, 
        as_default_rate, 
        volume, 
        pan, 
        loop, 
        prio, 
        as_default_delay 
    };
    return AS_SoundPlay(snd);
}

// stop playing a sound
 void AS_SoundStop(u8 chan)
{
    IPC_Sound->chan[chan].cmd = SNDCMD_STOP;
    
    if(IPC_Sound->surround)
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd = SNDCMD_STOP;
}

// pause an mp3
 void AS_MP3Pause()
{
    if(IPC_Sound->mp3.state & MP3ST_PLAYING)
        IPC_Sound->mp3.cmd = MP3CMD_PAUSE;
}

// unpause an mp3
 void AS_MP3Unpause()
{
    if(IPC_Sound->mp3.state & MP3ST_PAUSED)
        IPC_Sound->mp3.cmd = MP3CMD_PLAY;
}

// stop an mp3
 void AS_MP3Stop()
{
    IPC_Sound->mp3.cmd = MP3CMD_STOP;
    FILE_CLOSE(mp3file);
}

// get the current mp3 status
 int AS_GetMP3Status()
{
    return IPC_Sound->mp3.state;
}

// set the mp3 volume (0..127)
 void AS_SetMP3Volume(u8 volume)
{
    IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume = volume * AS_BASE_VOLUME / 127;
    AS_SetMP3Pan(IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan);
}

// set the default mp3 delay mode (warning: high values can cause glitches)
 void AS_SetMP3Delay(u8 delay)
{
    IPC_Sound->mp3.delay = delay;
}

// set the mp3 loop mode (false = one shot, true = loop indefinitely)
 void AS_SetMP3Loop(u8 loop)
{
    IPC_Sound->mp3.loop = loop;
}

// set the mp3 sample rate
 void AS_SetMP3Rate(s32 rate)
{
    IPC_Sound->mp3.rate = rate;
    IPC_Sound->mp3.cmd |= MP3CMD_SETRATE;
}

#ifdef __cplusplus
}
#endif

