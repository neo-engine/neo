//// nsbmd.h
//
//#pragma once
//
//#define TRUE	1
//#define FALSE	0
//typedef int	BOOL;
//typedef unsigned char		u8;
//typedef unsigned short int	u16;
//typedef unsigned long		u32;
//typedef signed char		s8;
//typedef signed short int	s16;
//typedef signed long		s32;
//
//#define NDS_TYPE_BMD0	0x30444d42	// 'BMD0'
//#define NDS_TYPE_MDL0	0x304c444d	// 'MDL0'
//#define NDS_TYPE_BTX0	0x30585442	// 'BTX0'
//#define NDS_TYPE_TEX0	0x30584554	// 'TEX0'
//#define NDS_TYPE_BCA0	0x30414342	// 'BCA0'
//#define NDS_TYPE_JNT0	0x30544e4a	// 'JNT0'
//#define NDS_TYPE_J_AC	0x4341004a	// 'J AC'
//#define NDS_TYPE_MAGIC2	0x0002feff
//#define NDS_TYPE_MAGIC1	0x0001feff
//
//#pragma pack(push)			// tell the compiler dont pack!
//#pragma pack(1)
//
/////////////////////////////////////////////////////////
//// COPIED from MS Platform SDK wingdi.h
//typedef struct {
//        u16	bfType;
//        u32	bfSize;
//        u16	bfReserved1;
//        u16	bfReserved2;
//        u32	bfOffBits;
//} BITMAPFILEHEADER;
//
//typedef struct {
//        u32	biSize;
//        s32	biWidth;
//        s32	biHeight;
//        u16	biPlanes;
//        u16	biBitCount;
//        u32	biCompression;
//        u32	biSizeImage;
//        s32	biXPelsPerMeter;
//        s32	biYPelsPerMeter;
//        u32	biClrUsed;
//        u32	biClrImportant;
//} BITMAPINFOHEADER;
/////////////////////////////////////////////////////////
//// Note the difference between RGBQUAD (for window API) and RGBA (for OpenGL)
//typedef struct {
//    u8 b;
//    u8 g;
//    u8 r;
//    u8 a;
//} RGBQUAD;
//
//typedef struct {
//    u8 r;
//    u8 g;
//    u8 b;
//    u8 a;
//} RGBA;
//
/////////////////////////////////////////////////////////
//// MATERIAL - a pair of texture and palette
//// a model may contain a number of material
//
//typedef struct
//{
//    char texname[18];	// max length is 16 chars
//    int texoffset;		// texture data offset in NSBMD / NSBTX
//    int texsize;		// texture data size
//    u16 format;		// 0..7 format (see DSTek)
//    u16 color0;		// 0: displayed, 1: transparent
//    int width;
//    int height;
//    u8 *texdata;
//    u8 *spdata;		// for 4x4 compressed texels only
//
//    char palname[18];	// max length is 16 chars
//    int paloffset;		// palette data offset in NSBMD / NSBTX
//    int palsize;		// palette data size
//    RGBA *paldata;
//
//    u8 texmatid;		// matching ID of the texture
//    u8 palmatid;		// matching ID of the palette
//    u16 repeat;		// repeat & flip flag
//} MATERIAL;
//
//// POLYGON is a sequence of DS 3D geometry command
//// which can easily be mapped with OpenGL functions
//typedef struct
//{
//    char polyname[18];	// max length is 16 chars
//    int dataoffset;		// polygon data offset in NSBMD (geometry command)
//    int datasize;		// polygon data size
//    u8 *polydata;
//    int matid;		// this polygon's associated material ID
//    int stackID;		// the first stackID used by this polygon
//} POLYGON;
//
//// OBJECT
//typedef struct
//{
//    BOOL trans;		// whether object needs translation
//    float m_trans[3];	// translation
//    BOOL rot;		// whether object needs rotation
//    int pivot;		// = -1 if pivot not exists
//    int neg;		// applies to rotation matrix
//    float a,b;		// rotation
//    int stackID;		// stackID used by this object
//    int restoreID;		// restoreID is the ID of the matrix in stack; to be restored as current matrix
//    int parent;		// this object's parent object ID
//    char name[18];		// name of this object
//} OBJECT;
//
//typedef struct
//{
//    char modelname[18];
//    int dataoffset;		// the 64 bytes data
//    int polynum;
//    POLYGON *polygon;
//    int matnum;
//    MATERIAL *material;
//    int objnum;
//    OBJECT *object;
//} MODEL;
//
//typedef struct
//{
//    int dataoffset;
//    int flag;
//    float m_trans[3];
//    float a,b;
//    float m_scale[3];
//    int frame;
//    int framelen;
//    u16 *animdata;
//} ANIMATION;
//
//
//BOOL convert_4x4texel ( u32 *tex, int width, int height, u16 *data, RGBA *pal, RGBA *out );
//BOOL make_texture ( MODEL *mod );
//
/////////////////////////////////////////////////////////
//
//typedef union
//{
//    struct
//    {
//        float _00, _01, _02, _03;
//        float _10, _11, _12, _13;
//        float _20, _21, _22, _23;
//        float _30, _31, _32, _33;
//    };
//    float m[4][4];
//    float a[16];
//} MTX44;
//
//void mtx_Zero ( MTX44 *m );
//void mtx_LoadIdentity ( MTX44 *m );
//void mtx_MultMatrix ( MTX44 *dest, MTX44 *a, MTX44 *b );
//void mtx_Translate ( MTX44 *dest, float x, float y, float z );
//void mtx_Scale ( MTX44 *dest, float x, float y, float z );
//void mtx_MultVector ( float *dest, MTX44 *a, float *v );
//void mtx_Rotate ( MTX44 *dest, int pivot, int neg, float a, float b );
//
//#pragma pack(pop)
//
/////////////////////////////////////////////////////////
//// global rendering options
//
//extern BOOL gOptTexture;	// default is render with texture
//extern BOOL gOptColoring;	// default is render with color
//extern BOOL gOptWireFrame;
//extern BOOL gOptCulling;
//extern BOOL gOptFiltering;	// filter texture for better result, slower
//extern BOOL gOptPolyMode;	// display one polygon at a time
//extern BOOL gOptVertexMode;	// display certain vertex list at a time
//extern BOOL gOptGrid;		// display the grid
//
/////////////////////////////////////////////////////////
//
//extern int g_modelnum;		// total no of model 
//extern int g_matnum;		// total no of material
//extern int g_animlen;		// no of frame
//extern MODEL *g_model;		// model array
//extern MATERIAL *g_mat;		// material array
//extern ANIMATION *g_anim;	// animation array
//extern int gCurrentModel;		// current model ID
//extern int gCurrentPoly;		// current polygon ID - used in conjunction with poly_mode
//extern int gCurrentVertex;	// current vertex ID - used in conjunction with vertex_mode
//
//void mouse_func ( int button, int state, int x, int y );
//void motion_func ( int x, int y );
//void kb_func ( u8 key, int x, int y );
//void sp_func ( int key, int x, int y );
//void render_scene ( void );
//void display_func ( void );
//// END