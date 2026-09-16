// Based on SSEQ Player by "RocketRobz" (https://github.com/RocketRobz/SSEQPlayer)
// arm9 sseq
#ifndef NO_SOUND
#include <cstdio>
#include <nds.h>

#include "sound/sseq.h"

namespace SOUND::SSEQ {
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

    int playSample( void *p_data, const sampleInfo &p_sampleInfo, const playInfo &p_playInfo ) {
        soundSysMessage msg;
        msg.m_message    = SNDSYS_PLAY_SAMPLE;
        msg.m_sample     = sequenceData{ p_data, p_sampleInfo.m_nonLoopLen };
        msg.m_sampleInfo = p_sampleInfo;
        msg.m_playInfo   = p_playInfo;

        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
        int ch = (int) fifoGetRetValue( FIFO_SNDSYS );
        return ( ch == 0xFFFF ) ? -1 : ch;
    }

    void stopSample( int p_handle ) {
        soundSysMessage msg;
        msg.m_message = SNDSYS_STOP_SAMPLE;
        msg.m_channel = p_handle;
        fifoSendDatamsg( FIFO_SNDSYS, sizeof( msg ), (u8 *) &msg );
    }

} // namespace SOUND::SSEQ

#endif
