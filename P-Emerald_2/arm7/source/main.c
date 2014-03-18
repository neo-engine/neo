/*

  Advanced Sound Library (ASlib)
  ------------------------------

  file        : main.c 
  author      : Lasorsa Yohan (Noda)
  description : ARM7 main program

  history : 

    28/11/2007 - v1.0
      = Original release

*/

#include <nds.h>
#include <nds/bios.h>
#include <nds/arm7/audio.h>
#include <nds/arm7/touch.h>
#include "as_lib7.h"


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}


extern void enableSound(void);
//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	fifoInit();

	SetYtrigger(80);

	installSoundFIFO();
    enableSound();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
    irqSet(IRQ_VBLANK, AS_SoundVBL);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);
	
	setPowerButtonCB(powerButtonCB);   

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
        AS_MP3Engine();
		swiWaitForVBlank();
	}
	return 0;
}