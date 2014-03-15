//#include <nds.h>
//#include <stdio.h>
//#include <n3d.h>
//#include "n3dsampleframework.h"
//
//
//void N3DSampleFrameworkInit()
//{
//	// Initialize NDS hardware and switch into 3D mode
//	powerOn(POWER_ALL);
//
//	videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
//
//	vramSetBankA(VRAM_A_TEXTURE);
//	vramSetBankB(VRAM_B_TEXTURE);
//	vramSetBankC(VRAM_C_SUB_BG); 
//	vramSetBankF(VRAM_F_TEX_PALETTE);
//	
//}
//
//void N3DSampleFrameworkWaitForVBlank()
//{
//	swiWaitForVBlank();
//}
//
//void N3DSampleFrameworkLoadTexture(const unsigned _vramOffset, const u8* _pData, const unsigned _size)
//{
//	unsigned vramTemp = vramSetMainBanks(VRAM_A_LCD,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);
//
//	swiCopy(_pData, &VRAM_A[_vramOffset>>1], (_size >> 2) | COPY_MODE_WORD);
//
//	vramRestoreMainBanks(vramTemp);
//}
//void N3DSampleFrameworkLoadPalette(const unsigned _vramOffset, const u8* _pData, const unsigned _size)
//{
//	vramSetBankF(VRAM_F_LCD);
//
//	swiCopy(_pData, &VRAM_F[_vramOffset>>1], (_size >> 2) | COPY_MODE_WORD);
//
//	vramSetBankF(VRAM_F_TEX_PALETTE);
//}
//void N3DSampleFrameworkClearTexturePaletteRAM()
//{
//	vramSetBankF(VRAM_F_LCD);
//
//	for(unsigned n = 0; n < 1024*16; n++)
//	{
//		VRAM_F[n] = 0;
//	}
//
//	vramSetBankF(VRAM_F_TEX_PALETTE);
//}
