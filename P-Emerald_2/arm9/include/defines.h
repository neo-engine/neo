#include <nds/ndstypes.h>
//#define USE_AS_LIB

#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))

#define COLOR_IDX (u8(253))
#define WHITE_IDX (u8(250))
#define GRAY_IDX (u8(251))
#define BLACK_IDX (u8(252))
#define RED_IDX (u8(253))
#define BLUE_IDX (u8(254))

#define CHOICE_COLOR RGB(16,25,19)

#define BG_PAL( p_sub ) ( ( p_sub ) ? BG_PALETTE_SUB : BG_PALETTE )
#define BG_BMP( p_sub ) ( ( p_sub ) ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 ) )