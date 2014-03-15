//// kiwi.ds <kiwi.ds@gmail.com>
//// nsbmd.cpp - display models with texture
//// This file is released in the hope that you will also release any modification to it.
//
//#include "stdafx.h"
//
//#include <nds.h>
//#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <memory.h>
//#include <assert.h>
//#include <string.h>
//#include <stdarg.h>
//#include <nds/arm9/video.h>	// glut.h is OpenGL specific, modify this line to where it is stored in your harddisk.
//#include <nds/arm9/videoGL.h>
//
//#include "nsbmd.h"
//
//
//
/////////////////////////////////////////////////////////
//// global rendering options
//
//BOOL gOptTexture	= TRUE;		// default is render with texture
//BOOL gOptColoring	= TRUE;		// default is render with color
//BOOL gOptWireFrame	= FALSE;
//BOOL gOptCulling	= TRUE;
//BOOL gOptFiltering	= FALSE;	// filter texture for better result, slower
//BOOL gOptPolyMode	= FALSE;	// display one polygon at a time
//BOOL gOptVertexMode	= FALSE;	// display certain vertex list at a time
//BOOL gOptGrid		= FALSE;	// display the grid
//
/////////////////////////////////////////////////////////
//// global task options
//
//BOOL gTaskDump		= FALSE;
//BOOL gTaskDumpVertex	= FALSE;
//BOOL gTaskDumpCode	= FALSE;
//BOOL gTaskDumpTexture	= FALSE;
//
/////////////////////////////////////////////////////////
//
//int g_modelnum = 0;		// total no of model 
//int g_matnum = 0;		// total no of material
//int g_animlen = 0;		// no of frame
//
//MODEL *g_model = NULL;		// model array
//MATERIAL *g_mat = NULL;		// material array
//ANIMATION *g_anim = NULL;	// animation array
//
//int gCurrentModel = 0;		// current model ID
//int gCurrentPoly = 0;		// current polygon ID - used in conjunction with poly_mode
//int gCurrentVertex = 0;		// current vertex ID - used in conjunction with vertex_mode
//
//static int stackID;
//static MTX44 MatrixStack[31];	// the matrix stack
//static MTX44 CurrentMatrix;	// the current matrix used by us
//
///*------------------------------------------------------------
//	return a 16-bit Little Endian value from file
//------------------------------------------------------------*/
//u16 getword ( FILE *f )
//{
//	u16 v;
//	v = getc( f );
//	v |= getc( f )<<8;
//	return v;
//}
//
///*------------------------------------------------------------
//	return a 32-bit Little Endian value from file
//------------------------------------------------------------*/
//u32 getdword ( FILE *f )
//{
//	register u32 v;
//	v = getc( f );
//	v |= getc( f )<<8;
//	v |= getc( f )<<16;
//	v |= getc( f )<<24;
//	return v;
//}
//
//
///*------------------------------------------------------------
//	process 3D geometry command
//------------------------------------------------------------*/
//BOOL process3Dcommand (u8 *pdata, int datasize )
//{
//	u8 *commandptr, *commandlimit;
//	int command[4], cur_vertex, mode, i;
//	float vtx_state[3] = { 0.0f, 0.0f, 0.0f };
//	float vtx_trans[3] = { 0.0f, 0.0f, 0.0f };
//
//	cur_vertex = gCurrentVertex;		// for vertex_mode
//	commandptr = pdata;
//	commandlimit = pdata + datasize;
//
//	memcpy( &CurrentMatrix, &MatrixStack[stackID], sizeof( MTX44 ) );
//	while ( commandptr < commandlimit )
//	{
//		command[0] = *commandptr++;
//		command[1] = *commandptr++;
//		command[2] = *commandptr++;
//		command[3] = *commandptr++;
//
//		for (i = 0; i < 4 && commandptr < commandlimit; i++)
//		{
//			switch ( command[i] ) {
//			case 0:		// No Operation (for padding packed GXFIFO commands)
//				break;
//			case 0x14:
///*
//  MTX_RESTORE - Restore Current Matrix from Stack (W)
//  Sets C=[N]. The stack pointer S is not used, and is left unchanged.
//  Parameter Bit0-4:  Stack Address (0..30) (31 causes overflow in GXSTAT.15)
//  Parameter Bit5-31: Not used
//*/
//			{
//				stackID = *(int *)commandptr;
//				commandptr += 4;
//				memcpy( &CurrentMatrix, &MatrixStack[stackID], sizeof( MTX44 ) );
//				break;
//			}
//			case 0x1b:
///*
//  MTX_SCALE - Multiply Current Matrix by Scale Matrix (W)
//  Sets C=M*C. Parameters: 3, m[0..2] (MTX_SCALE doesn't change Vector Matrix)
//*/
//			{
//				int x, y, z;
//				x = *(int *)commandptr;
//				commandptr += 4;
//				y = *(int *)commandptr;
//				commandptr += 4;
//				z = *(int *)commandptr;
//				commandptr += 4;
//				mtx_Scale( &CurrentMatrix, ((float)x) / 4096.0f, ((float)y) / 4096.0f, ((float)z) / 4096.0f );
//				break;
//			}
//			case 0x20:	// Directly Set Vertex Color (W)
//			{
//				int rgb, r, g, b;
//
//				rgb = *(int *)commandptr;
//				commandptr += 4;
//
//				if ( gOptColoring ) {
//					r = (rgb>> 0) & 0x1F;
//					g = (rgb>> 5) & 0x1F;
//					b = (rgb>>10) & 0x1F;
//					glColor3f( ((float)r)/31.0f, ((float)g)/31.0f, ((float)b)/31.0f );
//				}
//				break;
//			}
//			case 0x21:
///*
//  Set Normal Vector (W)
//  0-9   X-Component of Normal Vector (1bit sign + 9bit fractional part)
//  10-19 Y-Component of Normal Vector (1bit sign + 9bit fractional part)
//  20-29 Z-Component of Normal Vector (1bit sign + 9bit fractional part)
//  30-31 Not used
//*/			{
//				int xyz, x, y, z;
//
//				xyz = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (xyz>> 0) & 0x3FF;	if( x & 0x200 )		x |= 0xFFFFFC00;
//				y = (xyz>>10) & 0x3FF;	if( y & 0x200 )		y |= 0xFFFFFC00;
//				z = (xyz>>20) & 0x3FF;	if( z & 0x200 )		z |= 0xFFFFFC00;
//				glNormal3f( ((float)x)/512.0f, ((float)y)/512.0f, ((float)z)/512.0f );
//				break;
//			}
//			case 0x22:
///*
//  Set Texture Coordinates (W)
//  Parameter 1, Bit 0-15   S-Coordinate (X-Coordinate in Texture Source)
//  Parameter 1, Bit 16-31  T-Coordinate (Y-Coordinate in Texture Source)
//  Both values are 1bit sign + 11bit integer + 4bit fractional part.
//  A value of 1.0 (=1 SHL 4) equals to one Texel.
//*/
//			{
//				int st, s, t;
//
//				st = *(int *)commandptr;
//				commandptr += 4;
//
//				s = (st>> 0) & 0xffff;	if ( s & 0x8000 ) s |= 0xffff0000;
//				t = (st>>16) & 0xffff;	if ( t & 0x8000 ) t |= 0xffff0000;
//				glTexCoord2f( ((float)s)/16.0f, ((float)t)/16.0f );
//				break;
//			}
//			case 0x23:
///*
//  VTX_16 - Set Vertex XYZ Coordinates (W)
//  Parameter 1, Bit 0-15   X-Coordinate (signed, with 12bit fractional part)
//  Parameter 1, Bit 16-31  Y-Coordinate (signed, with 12bit fractional part)
//  Parameter 2, Bit 0-15   Z-Coordinate (signed, with 12bit fractional part)
//  Parameter 2, Bit 16-31  Not used
//*/
//			{
//				int parameter, x, y, z;
//
//				parameter = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (parameter>> 0) & 0xFFFF;	if( x & 0x8000 )	x |= 0xFFFF0000;
//				y = (parameter>>16) & 0xFFFF;	if( y & 0x8000 )	y |= 0xFFFF0000;
//
//				parameter = *(int *)commandptr;
//				commandptr += 4;
//				z = parameter & 0xFFFF;		if( z & 0x8000 )	z |= 0xFFFF0000;
//
//				vtx_state[0] = ((float)x) / 4096.0f;
//				vtx_state[1] = ((float)y) / 4096.0f;
//				vtx_state[2] = ((float)z) / 4096.0f;
//				if ( stackID != -1 ) {
//					if ( g_anim ) 
//						;
//					else
//						mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//
//				break;
//			}
//			case 0x24:
///*
//  VTX_10 - Set Vertex XYZ Coordinates (W)
//  Parameter 1, Bit 0-9    X-Coordinate (signed, with 6bit fractional part)
//  Parameter 1, Bit 10-19  Y-Coordinate (signed, with 6bit fractional part)
//  Parameter 1, Bit 20-29  Z-Coordinate (signed, with 6bit fractional part)
//  Parameter 1, Bit 30-31  Not used
//*/
//			{
//				int xyz, x, y, z;
//
//				xyz = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (xyz>> 0) & 0x3FF;	if( x & 0x200 )	x |= 0xFFFFFC00;
//				y = (xyz>>10) & 0x3FF;	if( y & 0x200 )	y |= 0xFFFFFC00;
//				z = (xyz>>20) & 0x3FF;	if( z & 0x200 )	z |= 0xFFFFFC00;
//				
//				vtx_state[0] = ((float)x) / 64.0f;
//				vtx_state[1] = ((float)y) / 64.0f;
//				vtx_state[2] = ((float)z) / 64.0f;
//
//				if ( stackID != -1 ) {
//					mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//				break;
//			}
//			case 0x25:
///*
//  VTX_XY - Set Vertex XY Coordinates (W)
//  Parameter 1, Bit 0-15   X-Coordinate (signed, with 12bit fractional part)
//  Parameter 1, Bit 16-31  Y-Coordinate (signed, with 12bit fractional part)
//*/
//			{
//				int xy, x, y;
//
//				xy = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (xy>> 0) & 0xFFFF;	if ( x & 0x8000 )	x |= 0xFFFF0000;
//				y = (xy>>16) & 0xFFFF;	if ( y & 0x8000 )	y |= 0xFFFF0000;
//				
//				vtx_state[0] = ((float)x) / 4096.0f;
//				vtx_state[1] = ((float)y) / 4096.0f;
//				
//				if ( stackID != -1 ) {
//					mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//				break;
//			}
//			case 0x26:
///*
//  VTX_XZ - Set Vertex XZ Coordinates (W)
//  Parameter 1, Bit 0-15   X-Coordinate (signed, with 12bit fractional part)
//  Parameter 1, Bit 16-31  Z-Coordinate (signed, with 12bit fractional part)
//*/
//			{
//				int xz, x, z;
//
//				xz = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (xz>> 0) & 0xFFFF;	if ( x & 0x8000 )	x |= 0xFFFF0000;
//				z = (xz>>16) & 0xFFFF;	if ( z & 0x8000 )	z |= 0xFFFF0000;
//
//				vtx_state[0] = ((float)x) / 4096.0f;
//				vtx_state[2] = ((float)z) / 4096.0f;
//
//				if ( stackID != -1 ) {
//					mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//				break;
//			}
//			case 0x27:
///*
//  VTX_YZ - Set Vertex YZ Coordinates (W)
//  Parameter 1, Bit 0-15   Y-Coordinate (signed, with 12bit fractional part)
//  Parameter 1, Bit 16-31  Z-Coordinate (signed, with 12bit fractional part)
//*/
//			{
//				int yz, y, z;
//				yz = *(int *)commandptr;
//				commandptr += 4;
//
//				y = (yz>> 0) & 0xFFFF;	if( y & 0x8000 )	y |= 0xFFFF0000;
//				z = (yz>>16) & 0xFFFF;	if( z & 0x8000 )	z |= 0xFFFF0000;
//
//				vtx_state[1] = ((float)y) / 4096.0f;
//				vtx_state[2] = ((float)z) / 4096.0f;
//
//				if ( stackID != -1 ) {
//					mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//				break;
//			}
//			case 0x28:
///*
//  VTX_DIFF - Set Relative Vertex Coordinates (W)
//  Parameter 1, Bit 0-9    X-Difference (signed, with 9bit fractional part)
//  Parameter 1, Bit 10-19  Y-Difference (signed, with 9bit fractional part)
//  Parameter 1, Bit 20-29  Z-Difference (signed, with 9bit fractional part)
//  Parameter 1, Bit 30-31  Not used
//*/
//			{
//				int xyz, x, y, z;
//				xyz = *(int *)commandptr;
//				commandptr += 4;
//
//				x = (xyz>> 0) & 0x3FF;	if ( x & 0x200 )	x |= 0xFFFFFC00;
//				y = (xyz>>10) & 0x3FF;	if ( y & 0x200 )	y |= 0xFFFFFC00;
//				z = (xyz>>20) & 0x3FF;	if ( z & 0x200 )	z |= 0xFFFFFC00;
//
//				vtx_state[0] += ((float)x) / 4096.0f;
//				vtx_state[1] += ((float)y) / 4096.0f;
//				vtx_state[2] += ((float)z) / 4096.0f;
//
//				if ( stackID != -1 ) {
//					mtx_MultVector( vtx_trans, &CurrentMatrix, vtx_state );
//					glVertex3fv( vtx_trans );
//				} else {
//					glVertex3fv( vtx_state );
//				}
//				break;
//			}
//			case 0x40:	// Start of Vertex List (W)
//			{
//				mode = *(int *)commandptr;
//				commandptr += 4;
//
//				switch (mode) {
//				case 0: mode = GL_TRIANGLES;		break;
//				case 1:	mode = GL_QUADS;		break;
//				case 2:	mode = GL_TRIANGLE_STRIP;	break;
//				case 3:	mode = GL_QUAD_STRIP;		break;
//				default:				return FALSE;
//				}
//
//				glBegin( mode );
//				break;
//			}
//			case 0x41:	// End of Vertex List (W)
//				glEnd();
//
//				// for vertex mode, display at maximum certain number of vertex-list
//				// decrease cur_vertex so that when we reach 0, stop rendering any further
//				cur_vertex--;
//				if ( cur_vertex < 0 && gOptVertexMode ) return TRUE;
//				break;
//			default:
//				assert(0);
//				return FALSE;
//			}
//		}
//	}
//	return TRUE;
//}
//
///*------------------------------------------------------------
//	decode additional model data (Skeleton / Bone)
//
//	fnsbmd - file handle
//	codeoffset - offset of the code section in file
//	codelimit - offset of the end of code section in file
//	mod - pointer to a MODEL structure (to be associated with this skeleton)
//------------------------------------------------------------*/
//BOOL decodecode ( FILE *fnsbmd, int codeoffset, int codelimit, MODEL *mod )
//{
//	int codeptr = codeoffset;
//	int begin = FALSE;		// whether there is a 0x0b begin code
//	int count = 0;
//
//	stackID = 0;
//	printf( "DEBUG: codeoffset = %08x\n", codeoffset );
//	fseek( fnsbmd, codeoffset, SEEK_SET );
//	while ( codeptr < codelimit )
//	{
//		int c = getc( fnsbmd );
//		int d,e,f,g,h,i,j,k;
//		switch (c) {
//		////////////////////////////////////////////
//		// bone-definition related byte
//		case 0x06:			// 3 bytes follow
//			d = getc(fnsbmd);
//			e = getc(fnsbmd);
//			f = getc(fnsbmd);	// dummy '0'
////			printf("DEBUG: %08x: 06: %02x --> %02x\n", codeptr, d, e);
//			codeptr += 4;
//			mod->object[d].parent = e;
//			mod->object[d].stackID = -1;
//			mod->object[d].restoreID = -1;
//			break;
//		case 0x26:			// 4 bytes follow
//			d = getc(fnsbmd);
//			e = getc(fnsbmd);
//			f = getc(fnsbmd);	// dummy '0'
//			g = getc(fnsbmd);	// store stackID
////			printf("DEBUG: %08x: %02x: %02x --> %02x\n", codeptr, c, d, e);
//			codeptr += 5;
//			mod->object[d].parent = e;
//			mod->object[d].stackID = stackID = g;
//			mod->object[d].restoreID = -1;
//			break;
//		case 0x46:			// 4 bytes follow
//			d = getc(fnsbmd);
//			e = getc(fnsbmd);
//			f = getc(fnsbmd);	// dummy '0'
//			g = getc(fnsbmd);	// restore stackID
////			printf("DEBUG: %08x: %02x: %02x --> %02x\n", codeptr, c, d, e);
//			codeptr += 5;
//			mod->object[d].parent = e;
//			mod->object[d].stackID = -1;
//			mod->object[d].restoreID = stackID = g;
//			break;
//		case 0x66:			// 5 bytes follow
//			d = getc(fnsbmd);
//			e = getc(fnsbmd);
//			f = getc(fnsbmd);	// dummy '0'
//			g = getc(fnsbmd);	// store stackID
//			h = getc(fnsbmd);	// restore stackID
////			printf("DEBUG: %08x: 66: %02x --> %02x\n", codeptr, d, e);
//			codeptr += 6;
//			mod->object[d].parent = e;
//			mod->object[d].stackID = stackID = g;
//			mod->object[d].restoreID = h;
//			break;
//		////////////////////////////////////////////
//		// node's visibility
//		case 0x02:			// 2 bytes follow
//			d = getc(fnsbmd);	// node ID
//			e = getc(fnsbmd);	// 1 = visible, 0 = hide
////			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr += 3;
//			break;
//		////////////////////////////////////////////
//		// stackID for polygon
//		case 0x03:	// 1 byte follows
//			stackID = getc(fnsbmd);
//			codeptr += 2;
//			break;
//		////////////////////////////////////////////
//		// unknown
//		case 0x07:
//		case 0x08:
//			d = getc(fnsbmd);
////			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr += 2;
//			break;
//		case 0x09:	// 8 bytes follow
//			d = getc(fnsbmd);
//			e = getc(fnsbmd);
//			f = getc(fnsbmd);
//			g = getc(fnsbmd);
//			h = getc(fnsbmd);
//			i = getc(fnsbmd);
//			j = getc(fnsbmd);
//			k = getc(fnsbmd);
//			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr += 9;
//			break;
//		////////////////////////////////////////////
//		// look like BEGIN and END pair
//		case 0x0b:	// 0 byte follows
//			if (begin) {
//				printf("DEBUG: %08x: previous 0x0b not ended.", codeptr);
//			}
//			begin = TRUE;
////			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr++;
//			break;
//		case 0x2b:	// 0 byte follows
//			if (!begin) {
//				printf( "DEBUG: %08x: previous 0x0b already ended.", codeptr );
//			}
//			begin = FALSE;
////			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr++;
//			break;
//		////////////////////////////////////////////
//		case 0x04:	// 3 bytes follow
//		case 0x24:
//		case 0x44:
//			// 04 mm 05 pp
//			// mm - specify the material ID, pp - specify the polygon ID
//
//			d = getc(fnsbmd);	assert( d < mod->matnum );
//			e = getc(fnsbmd);	assert( e == 0x05 );
//			f = getc(fnsbmd);	assert( f < mod->polynum );
//			mod->polygon[f].matid = d;
//			mod->polygon[f].stackID = stackID;
////			printf( "DEBUG: %08x: <%d> %02x %02x %02x %02x\n", codeptr, count, c, d, e, f );
//			codeptr += 4;
//			count++;
//			break;
//		////////////////////////////////////////////
//		case 0x01:	// end
////			printf( "DEBUG: %08x: %02x\n", codeptr, c );
//			codeptr++;
//			return TRUE;
//		default:
//			printf( "DEBUG: %08x: decodecode: unknown code %02x.\n", codeptr, c );
//			getchar();
//			return FALSE;
//		}
//	}
//	return TRUE;
//}
//
///*------------------------------------------------------------
//	read 'TEXO' block (appears in nsbmd and nsbtx)
//	blockoffset - the offset in nsbmd / nsbtx file
//	ptexnum - pointer to store the no. of texture found
//	ppalnum - pointer to store the no. of palette found
//	return array of MATERIAL
//------------------------------------------------------------*/
//MATERIAL *readtex0 ( FILE *fnsbmd, int blockoffset, int *ptexnum, int *ppalnum )
//{
//	int blocksize, blockptr, blocklimit;
//	int texnum, texdataoffset, texdatasize;
//	int sptexoffset, sptexsize, spdataoffset;		// for 4x4 compressed texels only
//	int palnum, paldefoffset, paldataoffset, paldatasize;
//	MATERIAL *material;
//	int r, i, j;
//
//	blockptr = blockoffset + 4;				// already read the block ID, so skip 4 bytes
//	fread( &blocksize, 4, 1, fnsbmd );			// block size
//	blocklimit = blocksize + blockoffset;
//	printf( "DEBUG: blockoffset = %08x, blocksize = %08x\n", blockoffset, blocksize );
//
//	fseek( fnsbmd, 4, SEEK_CUR );				// skip 4 padding 0s
//	texdatasize = getword( fnsbmd ) << 3;			// total texture data size div8
//	fseek( fnsbmd, 6, SEEK_CUR );				// skip 6 bytes
//	texdataoffset = getdword( fnsbmd ) + blockoffset;
//
//	fseek( fnsbmd, 4, SEEK_CUR );				// skip 4 padding 0s
//	sptexsize = getword( fnsbmd ) << 3;			// for format 5 4x4-texel, data size div8
//	fseek( fnsbmd, 6, SEEK_CUR );				// skip 6 bytes
//	sptexoffset = getdword( fnsbmd ) + blockoffset;		// for format 5 4x4-texel, data offset
//	spdataoffset = getdword( fnsbmd ) + blockoffset;	// for format 5 4x4-texel, palette info
//
//	fseek( fnsbmd, 4, SEEK_CUR );				// skip 4 bytes
//	paldatasize = getword(fnsbmd ) << 3;			// total palette data size div8
//	fseek( fnsbmd, 2, SEEK_CUR );				// skip 2 bytes
//	paldefoffset = getdword( fnsbmd ) + blockoffset;
//	paldataoffset = getdword( fnsbmd ) + blockoffset;
//
////	printf( "texdataoffset = %08x texdatasize = %08x\n", texdataoffset, texdatasize );
////	printf( "sptexoffset = %08x sptexsize = %08x spdataoffset = %08x\n", sptexoffset, sptexsize, spdataoffset );
////	printf( "paldataoffset = %08x paldatasize = %08x\n", paldataoffset, paldatasize );
//
//	////////////////////////////////////////////////
//	// texture definition
//
//	getc( fnsbmd );						// skip dummy '0'
//	texnum = getc( fnsbmd );				// no of texture
//	blockptr = ftell( fnsbmd );
//	fseek( fnsbmd, paldefoffset, SEEK_SET );
//	getc( fnsbmd );						// skip dummy '0'
//	palnum = getc( fnsbmd );				// no of palette
//	fseek( fnsbmd, blockptr, SEEK_SET );
//
//	printf( "texnum = %d, palnum = %d\n", texnum, palnum );
//
//	// allocate memory for material, great enough to hold all texture and palette
//	material = (MATERIAL *)calloc( sizeof( MATERIAL ), (texnum>palnum?texnum:palnum) );
//	if ( !material ) return FALSE;
//
//	fseek( fnsbmd, 14 + (texnum*4), SEEK_CUR );		// go straight to texture info
//	for ( i = 0; i < texnum; i++ )
//	{
//		int offset, param, format, width, height;
//		MATERIAL *mat = &material[i];
//
//		offset = (getword( fnsbmd )<<3);
//		param = getword( fnsbmd );			// texture parameter
//		fseek( fnsbmd, 4, SEEK_CUR );			// skip 4 bytes
//
//		format = (param>>10)&7;				// format 0..7, see DSTek
//		width = 8 << ((param>>4)&7);
//		height = 8 << ((param>>7)&7);
//		mat->color0 = (param>>13)&1;
//
//		if ( format == 5 )
//			mat->texoffset = offset + sptexoffset;	// 4x4-Texel Compressed Texture
//		else
//			mat->texoffset = offset + texdataoffset;
//
//		mat->format = format;
//		mat->width = width;
//		mat->height = height;
//	}
//
//	////////////////////////////////////////////////
//	// copy texture names
//	for ( i = 0; i < texnum; i++ )
//	{
//		fread( material[i].texname, 16, 1, fnsbmd );
//		material[i].texname[16] = 0;
//	}
//
//	////////////////////////////////////////////////
//	// calculate each texture's size
//	for (i = 0; i < texnum; i++)
//	{
//		static const int bpp[] = { 0, 8, 2, 4, 8, 2, 8, 16 };
//		MATERIAL *mat = &material[i];
//		mat->texsize = mat->width * mat->height * bpp[mat->format]/8;
//		printf( "tex %2d '%-16s': offset = %08x size = %08x [W,H] = [%d,%d]\n", 
//			i, mat->texname, mat->texoffset, mat->texsize, mat->width, mat->height );
//	}
//
//	////////////////////////////////////////////////
//	// palette definition
//	fseek( fnsbmd, paldefoffset + 2, SEEK_SET );		// skip palnum, already read
//	fseek( fnsbmd, 14 + (palnum*4), SEEK_CUR );		// go straight to palette info
//	for ( i = 0; i < palnum; i++ )
//	{
//		int offset = (getword( fnsbmd )<<3) + paldataoffset;
//		fseek( fnsbmd, 2, SEEK_CUR );			// skip 2 bytes
//		material[i].paloffset = offset;
//	}
//
//	////////////////////////////////////////////////
//	// copy palette names
//	for ( i = 0; i < palnum; i++ )
//	{
//		MATERIAL *mat = &material[i];
//		fread( mat->palname, 16, 1, fnsbmd );
//		mat->palname[16] = 0;
//	}
//
//	////////////////////////////////////////////////
//	// calculate each palette's size
//	// assume the palettes are stored sequentially
//	for ( i = 0; i < palnum - 1; i++ )
//	{
//		MATERIAL *mat = &material[i];
//		r = i+1;
//		while ( material[ r ].paloffset == mat->paloffset ) r++;
//		// below is a stupid way to calculate the size of palette: next's offset - current's offset
//		// it works most of the time
//		if ( r != palnum )
//			mat->palsize  = material[ r ].paloffset - mat->paloffset;
//		else
//			mat->palsize  = blocklimit - mat->paloffset;
//		printf( "pal '%s' size = %d\n", mat->palname, mat->palsize );
//	}
//	material[i].palsize = blocklimit - material[ i ].paloffset;
//
//	////////////////////////////////////////////////
//	// traverse each texture
//	for ( i = 0; i < texnum; i++ ) 
//	{
//		MATERIAL *mat = &material[i];
//		mat->texdata = (u8 *)malloc( mat->texsize );
//		if ( !mat->texdata ) return NULL;
//		////////////////////////////////////////////////
//		// read texture into memory
//		fseek( fnsbmd, mat->texoffset, SEEK_SET );
//		fread( mat->texdata, mat->texsize, 1, fnsbmd );
//
//		//printf( "DEBUG: texoffset = %08x, texsize = %08x\n", mat->texoffset, mat->texsize );
//
//		////////////////////////////////////////////////
//		// additional data for format 5 4x4 compressed texels
//		if ( mat->format == 5 ) {
//			r = mat->texsize>>1;
//			mat->spdata = (u8 *)malloc( r );
//			if ( !mat->spdata ) return NULL;
//
//			printf( "DEBUG: 4x4-texel spdataoffset = %08x, spdatasize = %08x\n", spdataoffset, r );
//
//			fseek( fnsbmd, spdataoffset, SEEK_SET );
//			fread( mat->spdata, r, 1, fnsbmd );
//			spdataoffset += r;
//		}
//	}
//
//	////////////////////////////////////////////////
//	// traverse each palette
//	for (i = 0; i < palnum; i++ )
//	{
//		MATERIAL *mat = &material[i];		
//		int palentry = mat->palsize>>1;
//
//		RGBA *rgbq = (RGBA *)malloc( sizeof( RGBA ) * palentry );
//		if (!rgbq) return FALSE; 
//
//		printf( "DEBUG: converting pal '%s', palentry = %d\n", mat->palname, palentry );
//
//		fseek( fnsbmd, mat->paloffset, SEEK_SET );
//		for ( j = 0; j < palentry; j++ )
//		{
//			u16 p;
//			fread(&p, 2, 1, fnsbmd);
//			rgbq[j].r = ((p>> 0) & 0x1f) << 3;	// red
//			rgbq[j].g = ((p>> 5) & 0x1f) << 3;	// green
//			rgbq[j].b = ((p>>10) & 0x1f) << 3;	// blue
//			//rgbq[j].a = (p&0x8000) ? 0xff : 0;
//			rgbq[j].a = 0xff;			// alpha
//		}
//		mat->paldata = rgbq;
//	}
//
//	*ptexnum = texnum;
//	*ppalnum = palnum;
//	return material;
//}
//
///*------------------------------------------------------------
//	read 'MDL0' block (appears in nsbmd)
//
//	blockoffset - the offset in file
//	modelnum - pointer to store the no of model parsed
//	return array of MODEL structure
//------------------------------------------------------------*/
//MODEL *readmdl0 ( FILE *fnsbmd, int blockoffset, int *modelnum )
//{
//	int blocksize, blockptr, blocklimit;
//	int num, i, j, r;
//	MODEL *model;
//
//	////////////////////////////////////////////////
//	// model
//	blockptr = blockoffset + 4;			// already read the ID, skip 4 bytes
//	fread( &blocksize, 4, 1, fnsbmd );		// block size
//	blocklimit = blocksize + blockoffset;
//
//	getc( fnsbmd );				// skip dummy '0'
//	num = getc( fnsbmd );				// no of model
//	printf( "No. of Model = %02x\n", num );
//	if ( num <= 0 ) return FALSE;
//
//	model = (MODEL *)calloc( sizeof( MODEL ), num );
//	if ( !model ) return FALSE;
//
//	fseek( fnsbmd, 10 + (num*4), SEEK_CUR );	// skip [char xyz], useless
//	blockptr += 10;
//
//	fseek( fnsbmd, 4, SEEK_CUR );			// go straight to model data offset
//	blockptr += 4;
//	for (i = 0; i < num; i++)
//		model[i].dataoffset = getdword( fnsbmd ) + blockoffset;
//
//	////////////////////////////////////////////////
//	// copy model names
//	for ( i = 0; i < num; i++ )
//	{
//		fread( model[i].modelname, 16, 1, fnsbmd );
//		model[i].modelname[16] = 0;
//	}
//
//	////////////////////////////////////////////////
//	// parse model data
//	for ( i = 0; i < num; i++ )
//	{
//		MODEL *mod = &model[i];
//
//		fseek( fnsbmd, mod->dataoffset, SEEK_SET );
//
//		// the following variables are all offset values
//		int totalsize, codeoffset, texpaloffset, polyoffset, polyend, polynum, matnum, texoffset, paloffset;
//		totalsize = getdword( fnsbmd ) + mod->dataoffset;
//		codeoffset = getdword( fnsbmd ) + mod->dataoffset;	// additional model data, bone definition etc., just follow object section
//		texpaloffset = getdword( fnsbmd ) + mod->dataoffset;
//		polyoffset = getdword( fnsbmd ) + mod->dataoffset;
//		polyend = getdword( fnsbmd ) + mod->dataoffset;
//
//		fseek( fnsbmd, 4, SEEK_CUR );				// skip 4 bytes of unknown model data
//		matnum = getc( fnsbmd );				// no. of material
//		polynum = getc( fnsbmd );				// no. of polygon
//
//		fseek( fnsbmd, 38, SEEK_CUR );				// skip 38 bytes, go straight to object
//
//		////////////////////////////////////////////////
//		// object section
//		int objnum, *objdataoffset, *objdatasize, objdatabase;
//		objdatabase = ftell( fnsbmd );
//		getc( fnsbmd );						// skip dummy '0'
//		objnum = getc( fnsbmd );				// no of object
//
//		fseek( fnsbmd, 14 + (objnum*4), SEEK_CUR );		// skip bytes, go striaght to object data offset
//
//		mod->object = (OBJECT *)calloc( sizeof( OBJECT ), objnum );
//		objdataoffset = (int *)malloc( sizeof( int ) * objnum );
//		objdatasize = (int *)malloc( sizeof( int ) * objnum );
//
//		if ( !objdataoffset || !objdatasize || ! mod->object ) return FALSE;
//
//		for ( j = 0; j < objnum; j++ )
//			objdataoffset[j] = getdword( fnsbmd ) + objdatabase;
//
//		for ( j = 0; j < objnum-1; j++ )
//			objdatasize[j] = objdataoffset[j+1] - objdataoffset[j];
//		objdatasize[j] = codeoffset - objdataoffset[j];
//
//		////////////////////////////////////////////////
//		// copy object names
//		for ( j = 0; j < objnum; j++ )
//		{
//			char *name = mod->object[j].name;
//			fread( name, 16, 1, fnsbmd );
//			name[16] = 0;
//		}
//
//		////////////////////////////////////////////////
//		// parse object information
//		for ( j = 0; j < objnum; j++ )
//		{
//			if ( objdatasize[j] > 4 ) {
//				fseek( fnsbmd, objdataoffset[j], SEEK_SET );
//				int v = getdword( fnsbmd );
//				if ( (v & 1) == 0 ) {
//					float *m_trans = mod->object[j].m_trans;
//					mod->object[j].trans = TRUE;
//
//					int x = getdword( fnsbmd );
//					int y = getdword( fnsbmd );
//					int z = getdword( fnsbmd );
//
//					m_trans[0] = ((float)x)/4096.0f;
//					m_trans[1] = ((float)y)/4096.0f;
//					m_trans[2] = ((float)z)/4096.0f;
//				}
//				if ( (v&0xa) == 0x8 ) {
//					mod->object[j].rot = TRUE;
//
//					int a = getword( fnsbmd );	if ( a & 0x8000 ) a |= 0xffff0000;
//					int b = getword( fnsbmd );	if ( b & 0x8000 ) b |= 0xffff0000;
//					mod->object[j].pivot = (v>>4)&0x0f;
//					mod->object[j].neg = (v>>8)&0x0f;
//					mod->object[j].a = ((float)a)/4096.0f;
//					mod->object[j].b = ((float)b)/4096.0f;
//				}
//			}
//		}
//		free( objdataoffset );
//		free( objdatasize );
//
//		////////////////////////////////////////////////
//		// material section
//		fseek( fnsbmd, texpaloffset, SEEK_SET );		// now get the texture and palette offset
//		texoffset = getword( fnsbmd ) + texpaloffset;
//		paloffset = getword( fnsbmd ) + texpaloffset;
//
//		// allocate memory for material
//		mod->material = (MATERIAL *)calloc( sizeof( MATERIAL ), matnum );
//		if ( !mod->material ) return FALSE;
//
//		////////////////////////////////////////////////
//		// parse material definition
//		// defines a material by pairing texture and palette
//		fseek( fnsbmd, 16 + (matnum*4), SEEK_CUR );		// go straight to material data offset
//		for ( j = 0; j < matnum; j++ )
//		{
//			blockptr = ftell( fnsbmd );
//			r = getdword( fnsbmd ) + texpaloffset + 4 + 18;	// skip 18 bytes (+ 2 bytes for texoffset, 2 bytes for paloffset)
//			fseek( fnsbmd, r, SEEK_SET );
//			mod->material[j].repeat = getc( fnsbmd );
//			fseek( fnsbmd, blockptr+4, SEEK_SET );
//		}
//
//		////////////////////////////////////////////////
//		// now go to read the texture definition
//		fseek( fnsbmd, texoffset, SEEK_SET );
//		getc( fnsbmd );						// skip dummy '0'
//		int texnum = getc( fnsbmd );				// no of texture definition
//		assert( texnum <= matnum );				// may not always hold?
//		printf( "DEBUG: texnum = %d\n", texnum );
//
//		if ( texnum ) {
//			fseek( fnsbmd, 14 + (texnum*4), SEEK_CUR );	// go straight to data offsets
//			for ( j = 0; j < texnum; j++ )			// matching texture with material
//			{
//				int texmatid = (getdword( fnsbmd ) & 0xffff) + texpaloffset;
//				blockptr = ftell( fnsbmd );
//				fseek( fnsbmd, texmatid, SEEK_SET );
//				texmatid = getc( fnsbmd );
//				model[i].material[j].texmatid = texmatid;
//				fseek( fnsbmd, blockptr, SEEK_SET );
//			}
//			for ( j = 0; j < texnum; j++ )			// copy texture names
//			{
//				int texmatid = mod->material[j].texmatid;
//				char *name = mod->material[ texmatid ].texname;
//				fread( name, 16, 1, fnsbmd );
//				name[16] = 0;
//				printf( "tex (matid=%d): '%s'\n", texmatid, name );
//			}
//		}
//
//		////////////////////////////////////////////////
//		// now go to read the palette definition
//		fseek(fnsbmd, paloffset, SEEK_SET);
//		getc( fnsbmd );						// skip dummy '0'
//		int palnum = getc( fnsbmd );				// no of palette definition
//		assert( palnum <= matnum );				// may not always hold?
//		printf( "DEBUG: palnum = %d\n", palnum );
//
//		if ( palnum ) {
//			fseek( fnsbmd, 14 + (palnum*4), SEEK_CUR );	// go straight to data offsets
//			for ( j = 0; j < palnum; j++ )			// matching palette with material
//			{
//				int palmatid = (getdword( fnsbmd ) & 0xffff) + texpaloffset;
//				blockptr = ftell( fnsbmd );
//				fseek( fnsbmd, palmatid, SEEK_SET );
//				palmatid = getc( fnsbmd );
//				model[i].material[j].palmatid = palmatid;
//				fseek( fnsbmd, blockptr, SEEK_SET );
//			}
//			for (j = 0; j < palnum; j++)			// copy palette names
//			{
//				int palmatid = mod->material[j].palmatid;
//				char *name = mod->material[ palmatid ].palname;
//				fread( name, 16, 1, fnsbmd );
//				name[16] = 0;
//				printf( "pal (matid=%d): '%s'\n", palmatid, name );
//			}
//		}
//
//		for ( j = 0; j < texnum; j++ )
//		{
//			MATERIAL *mat = &mod->material[j];
//			printf( "DEBUG: matid = %d, texname: %s, palname: %s\n", j, mat->texname, mat->palname );
//		}
//
//		////////////////////////////////////////////////
//		// Polygon
//		fseek( fnsbmd, polyoffset, SEEK_SET );
//		getc( fnsbmd );						// skip dummy '0'
//		r = getc( fnsbmd );					// no of polygon
//		printf( "DEBUG: polynum = %d\n", polynum );
//
//		model[i].polygon = (POLYGON *)malloc( sizeof( POLYGON ) * polynum );
//		if ( !model[i].polygon ) return FALSE;
//
//		fseek( fnsbmd, 14 + (polynum*4), SEEK_CUR);		// skip bytes, go straight to data offset
//
//		for ( j = 0; j < polynum; j++ )
//			model[i].polygon[j].dataoffset = getdword(fnsbmd) + polyoffset;
//
//		for ( j = 0; j < polynum; j++ )				// copy polygon names
//		{
//			char *name = mod->polygon[j].polyname;
//			fread( name, 16, 1, fnsbmd );
//			name[16] = 0;
//
//		}
//
//		////////////////////////////////////////////////
//		// now go to the polygon data, there is a 16-byte-header before geometry commands
//		for ( j = 0; j < polynum; j++ )
//		{
//			POLYGON *poly = &mod->polygon[j];
//			//////////////////////////////////////////////////////////
//			poly->matid = -1;	// DEFAULT: indicate no associated material
//			//////////////////////////////////////////////////////////
//			fseek( fnsbmd, poly->dataoffset + 8, SEEK_SET );	// skip 8 unknown bytes
//			poly->dataoffset = getdword( fnsbmd ) + poly->dataoffset;
//			poly->datasize = getdword( fnsbmd );
//			//printf( "poly %2d '%-16s': dataoffset: %08x datasize %08x\n", j, poly->polyname, poly->dataoffset, poly->datasize );
//		}
//		////////////////////////////////////////////////
//		// read the polygon data into memory
//		for ( j = 0; j < polynum; j++ )
//		{
//			POLYGON *poly = &mod->polygon[j];
//			fseek( fnsbmd, poly->dataoffset , SEEK_SET );
//			poly->polydata = (u8 *)malloc( poly->datasize );
//			if ( !poly->polydata ) return FALSE;
//			fread( poly->polydata, poly->datasize, 1, fnsbmd);
//		}
//
//		////////////////////////////////////////////////
//		// decode the additional model data
//		mod->objnum = objnum;
//		mod->polynum = polynum;
//		mod->matnum = matnum;
//		r = decodecode( fnsbmd, codeoffset, texpaloffset, mod );
//		if ( !r ) return FALSE;
//	}
//
//	*modelnum = num;
//	return model;
//}
//
//ANIMATION *readjnt0 ( FILE *fnsbca, int blockoffset, int *animlen )
//{
//	int blocksize, blockptr, blocklimit;
//	int num, objnum, i, j, r;
//	int *dataoffset;
//	int sec1offset, sec2offset;
//	ANIMATION *animation;
//
//	////////////////////////////////////////////////
//	// joint
//	blockptr = blockoffset + 4;			// already read the ID, skip 4 bytes
//	fread( &blocksize, 4, 1, fnsbca );		// block size
//	blocklimit = blocksize + blockoffset;
//
//	getc( fnsbca );					// skip dummy 0
//	num = getc( fnsbca );	assert( num > 0 );	// no of joint must == 1
//	printf( "No. of Joint = %02x\n", num );
//
//	dataoffset = (int *)malloc( sizeof( int ) );
//	if ( !dataoffset ) return NULL;
//
//	fseek( fnsbca, 10 + (num<<2), 1 );		// skip [char xyz], useless
//	blockptr += 10;
//
//	fseek( fnsbca, 4, 1 );				// go straight to joint data offset
//	blockptr += 4;
//	for ( i = 0; i < num; i++ )
//		dataoffset[i] = getdword( fnsbca ) + blockoffset;
//
//	//fseek( fnsbca, 16 * num, SEEK_CUR );		// skip names
//	blockptr += 16 * num;
//
//	for ( i = 0; i < num; i++ )
//	{
//		fseek( fnsbca, dataoffset[i], SEEK_SET );
//		j = getdword( fnsbca );
//		if ( j != NDS_TYPE_J_AC ) return NULL;
//		blockptr += 4;
//
//		*animlen = getword( fnsbca );
//		objnum = getword( fnsbca );
//		if ( objnum != g_model[0].objnum ) return NULL;
//		blockptr += 4;
//
//		animation = (ANIMATION *)calloc( sizeof( ANIMATION ), objnum );
//		if ( !animation ) return NULL;
//
//		fseek( fnsbca, 4, SEEK_CUR );	// skip 4 zeros
//		blockptr += 4;
//
//		sec1offset = getdword( fnsbca ) + dataoffset[i];
//		sec2offset = getdword( fnsbca ) + dataoffset[i];
//		blockptr += 8;
//
//		for ( j = 0; j < objnum; j++ )
//			animation[j].dataoffset = getword( fnsbca ) + dataoffset[i];
//
//		for ( j = 0; j < objnum; j++ )
//		{
//			ANIMATION *anim = &animation[j];
//			r = getdword( fnsbca );
//			anim->flag = r;
//			if ( (r&1) == 0 ) {		// any transformation?
//				if ( (r&2) == 0 ) {	// translation
//					if ( (r&4) == 1 ) {	// use Base T
//					} else {
//						if ( (r&8) == 1 ) {	// consTX
//							anim->m_trans[0] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//						if ( (r&0x10) == 1 ) {	// consTY
//							anim->m_trans[1] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//						if ( (r&0x20) == 1 ) {	// consTZ
//							anim->m_trans[0] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//					}
//				}
//				if ( (r&0x40) == 0 ) {	// rotation
//					if ( (r&0x100) == 1 ) {	// constR
//						anim->a = ((float)getword(fnsbca)) / 4096.0f;
//						anim->b = ((float)getword(fnsbca)) / 4096.0f;
//					} else {
//					}
//				}
//				if ( (r&0x200) == 0 ) {	// scale
//					if ( (r&0x400) == 1 ) {	// use Base S
//					} else {
//						if ( (r&0x800) == 1 ) {	// consSX
//							anim->m_scale[0] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//						if ( (r&0x1000) == 1 ) {// consSY
//							anim->m_scale[0] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//						if ( (r&0x2000) == 1 ) {// consSZ
//							anim->m_scale[0] = ((float)getdword(fnsbca)) / 4096.0f;
//						} else {
//						}
//					}
//				}
//			}
//		}
//
//	}
//
//	free( dataoffset );
//	return animation;
//}
//
//
///*------------------------------------------------------------
//	OpenGL - render the scene
//------------------------------------------------------------*/
//void render_scene ( void )
//{
//	MODEL *mod = &g_model[ gCurrentModel ];
//	POLYGON *poly;
//	MTX44 tmp;
//	int i;
//	
//	////////////////////////////////////////////////////////////
//	// prepare the matrix stack
//	for ( i = 0; i < mod->objnum; i++ )
//	{
//		OBJECT *obj = &mod->object[i];
//		float *m_trans = obj->m_trans;
//		if ( obj->restoreID != -1 ) glLoadMatrixf( (GLfloat *)&MatrixStack[ obj->restoreID ] );
//		if ( obj->stackID != -1 ) {
//			if ( obj->trans ) glTranslatef( m_trans[0], m_trans[1], m_trans[2] );
//			if ( obj->rot )  {
//				mtx_Rotate( &tmp, obj->pivot, obj->neg, obj->a, obj->b );
//				glMultMatrixf( (GLfloat *)&tmp );
//			}
//
//			glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat *)&MatrixStack[ obj->stackID ] );
//			stackID = obj->stackID;	// save the last stackID
//		}
//	}
//	glLoadIdentity();
//
//	////////////////////////////////////////////////////////////
//	// display one polygon of the current model at a time
//	if ( gOptPolyMode ) {	
//
//		poly = &mod->polygon[ gCurrentPoly ];
//
//		if ( gOptTexture && !gOptWireFrame && g_mat ) {
//			int matid = poly->matid;
//			MATERIAL *mat = &mod->material[ matid ];
//	
//			if ( matid != -1 ) {
//				glBindTexture( GL_TEXTURE_2D, matid+1 );
//
//
//				// Convert pixel coords to normalised STs
//				glMatrixMode( GL_TEXTURE );
//				glLoadIdentity();
//				if ( mat->repeat == 0x07 )
//					glScalef( 2.0f/((float)mat->width), 1.0f/((float)mat->height), 1.0f );
//				else if ( mat->repeat == 0x0b )
//					glScalef( 1.0f/((float)mat->width), 2.0f/((float)mat->height), 1.0f );
//				else
//					glScalef( 1.0f/(float)mat->width, 1.0f/(float)mat->height, 1.0f );
//			}
//		} else {
//			glBindTexture( GL_TEXTURE_2D, 0 );
//		}
//
//		if ( !gOptColoring ) glColor3f(1, 1, 1);
//		stackID = poly->stackID;	// the first matrix used by this polygon
//		process3Dcommand( poly->polydata, poly->datasize );
//
//		return;
//	}
//
//	////////////////////////////////////////////////////////////
//	// display all polygons of the current model
//	for ( i = 0; i < mod->polynum; i++ )
//	{
//		poly = &mod->polygon[i];
//
//		if ( gOptTexture && !gOptWireFrame && g_mat ) {
//			int matid = poly->matid;
//	
//			if ( matid != -1 ) {
//				MATERIAL *mat = &mod->material[ matid ];
//				glBindTexture( GL_TEXTURE_2D, matid+1 );
//
//				// Convert pixel coords to normalised STs
//				glMatrixMode( GL_TEXTURE );
//				glLoadIdentity();
//				if ( mat->repeat == 0x07 )
//					glScalef( 2.0f/((float)mat->width), 1.0f/((float)mat->height), 1.0f );
//				else if ( mat->repeat == 0x0b )
//					glScalef( 1.0f/((float)mat->width), 2.0f/((float)mat->height), 1.0f );
//				else
//					glScalef( 1.0f/(float)mat->width, 1.0f/(float)mat->height, 1.0f );
//			} else {
//				glBindTexture( GL_TEXTURE_2D, 0 );
//			}
//		} else {
//			glBindTexture( GL_TEXTURE_2D, 0 );
//		}
//
//		if ( !gOptColoring ) glColor3f(1, 1, 1);
//		stackID = poly->stackID;	// the first matrix used by this polygon
//		process3Dcommand( poly->polydata, poly->datasize );
//	}
//}
//
//
///*------------------------------------------------------------
//	Main
//------------------------------------------------------------*/
//int main (int argc, char *argv[])
//{
//	char *szfilename;
//	char szfiletitle[256];
//	char szfilepath[256];
//	int i, j, r, id;
//	int filesize, numblock;
//	int texnum, palnum;
//	FILE *fnsbmd;
//
//	if (argc == 1) {
//		showusage();
//		return 1;
//	} else if (argc == 2) {
//		i = 1;
//	} else {
//		i = parseoptions( argc, argv );
//		if ( !i ) {
//			showusage();
//			return 1;
//		}
//	}
//
//	///////////////////////////////////////////////////////
//	// open NSBMD
//	szfilename = argv[i];
//	fnsbmd = fopen( szfilename, "rb" );
//	if ( !fnsbmd ) {
//		printf( "Open file '%s' error.\n", szfilename );
//		return 1;
//	}
//
//	fseek( fnsbmd, 0, SEEK_END );
//	r = ftell( fnsbmd );
//	rewind( fnsbmd );
//	printf( "DEBUG: file size = %08x (%d)\n", r, r );
//
//	///////////////////////////////////////////////////////
//	// validate the file
//	fread( &id, 4, 1, fnsbmd );
//	if ( id == NDS_TYPE_BMD0 ) {
//		fread( &i, 4, 1, fnsbmd );
//		if ( i != NDS_TYPE_MAGIC2 ) {
//			printf( "DEBUG: magic2 not found.\n" );
//			return 1;
//		}
//	} else {
//		printf( "DEBUG: file is not BMD0.\n" );
//		return 1;
//	}
//
//	fread( &filesize, 4, 1, fnsbmd );
//	if ( filesize != r ) {
//		printf( "DEBUG: file size mismatch.\n" );
//		return 1;
//	}
//
//	r = fread( &numblock, 4, 1, fnsbmd );
//	numblock >>= 16;
//	if ( !numblock ) {
//		printf( "DEBUG: no of block zero.\n" );
//		return 1;
//	}
//
//	///////////////////////////////////////////////////////
//	// allocate memory for storing blockoffset
//	int *blockoffset = (int *)malloc( sizeof( int ) * numblock );
//	if ( !blockoffset ) return 1;
//
//	for (i = 0; i < numblock; i++)
//	{
//		r = fread( &blockoffset[i], 4, 1, fnsbmd );
//		if ( !r ) return 1;
//	}
//
//	///////////////////////////////////////////////////////
//	// now go to read the blocks
//	for (i = 0; i < numblock; i++)
//	{
//		fseek( fnsbmd, blockoffset[i], 0 );
//		r = fread( &id, 4, 1, fnsbmd );
//		if ( !r ) return 1;
//
//		switch ( id ) {
//		case NDS_TYPE_MDL0:
//			puts( "=================================\nreading 'MDL0'..." );
//			g_model = readmdl0( fnsbmd, blockoffset[i], &g_modelnum );
//			if ( !g_model ) {
//				puts( "ERROR readmdl0" );
//				return 1;
//			}
//			break;
//		case NDS_TYPE_TEX0:
//			puts( "=================================\nreading 'TEX0'..." );
//			g_mat = readtex0( fnsbmd, blockoffset[i], &texnum, &palnum );
//			if ( !g_mat ) {
//				puts( "ERROR readtex0" );
//			}
//			break;
//		default:
//			printf( "Unknown ID %c%c%c%c is not supported\n", id & 0xff, (id>>8)&0xff, (id>>16)&0xff, (id>>24)&0xff );
//			return 1;
//		}
//	}
//	fclose( fnsbmd );
//
//	///////////////////////////////////////////////////////
//	// before proceed to display the 3D model
//	// if no TEX0 in nsbmd, try to load nsbtx file, with the same filename
//	if ( g_mat == NULL ) {
//		getfilepath( szfilename, szfilepath );
//		getfiletitle( szfilename, szfiletitle );
//		strcat( szfiletitle, ".nsbtx" );
//		strcat( szfilepath, szfiletitle );
//		printf( "DEBUG: trying to open '%s'.\n", szfilepath );
//		FILE *ftex = fopen( szfilepath, "rb" );
//		if ( !ftex ) {
//			getfilepath( szfilename, szfilepath );
//			getfiletitle( szfilename, szfiletitle );
//			strcat( szfiletitle, ".btx" );
//			strcat(szfilepath, szfiletitle);
//			printf( "DEBUG: trying to open '%s'.\n", szfilepath );
//			ftex = fopen( szfilepath, "rb" );
//			if ( !ftex ) puts( "No texture found." );
//		}
//		if ( ftex ) {
//			fseek( ftex, 0, SEEK_END );
//			filesize = ftell( ftex );
//			rewind( ftex );
//
//			fread( &id, 4, 1, ftex );
//			if ( id == NDS_TYPE_BTX0 ) {
//				fread( &i, 4, 1, ftex );
//				if ( i == NDS_TYPE_MAGIC1 ) {
//					fread( &i, 4, 1, ftex );
//					if ( i == filesize ) {
//						fread( &numblock, 4, 1, ftex );
//						numblock >>= 16;
//						fread( &r, 4, 1, ftex );
//						fread( &id, 4, 1, ftex );
//						if (numblock == 1 && r == 0x14 && id == NDS_TYPE_TEX0) {
//							puts( "=================================\nreading 'TEX0'..." );
//							g_mat = readtex0( ftex, 0x14, &texnum, &palnum );
//							if ( !g_mat ) {
//								puts("ERROR readtex0");
//								return 1;
//							}
//						} else
//							printf("DEBUG: no. of block != 1.\n");
//					} else
//						printf("DEBUG: file size mismatch.\n");
//				} else
//					printf("DEBUG: magic2 not found.\n");
//			} else
//				printf("DEBUG: file is not BTX0.\n");
//			fclose( ftex );
//		}
//	}
//
//	///////////////////////////////////////////////////////
//	// try to load NSBCA file, if any
///*
//	getfilepath( szfilename, szfilepath );
//	getfiletitle( szfilename, szfiletitle );
//	strcat( szfiletitle, ".nsbca" );
//	strcat( szfilepath, szfiletitle );
//	printf( "DEBUG: trying to open '%s'.\n", szfilepath );
//	FILE *fnsbca = fopen( szfilepath, "rb" );
//	if ( !fnsbca ) {
//		getfilepath( szfilename, szfilepath );
//		getfiletitle( szfilename, szfiletitle );
//		strcat( szfiletitle, ".bca" );
//		strcat( szfilepath, szfiletitle );
//		printf( "DEBUG: trying to open '%s'.\n", szfilepath );
//		fnsbca = fopen( szfilepath, "rb" );
//		if ( !fnsbca ) puts( "No animation found." );
//	}
//	if ( fnsbca ) {
//		fseek( fnsbca, 0, SEEK_END );
//		filesize = ftell( fnsbca );
//		rewind( fnsbca );
//
//		fread( &id, 4, 1, fnsbca );
//		if ( id == NDS_TYPE_BCA0 ) {
//			fread( &i, 4, 1, fnsbca );
//			if ( i == NDS_TYPE_MAGIC1 ) {
//				fread( &i, 4, 1, fnsbca );
//				if ( i == filesize ) {
//					fread( &numblock, 4, 1, fnsbca );
//					numblock >>= 16;
//					fread( &r, 4, 1, fnsbca );
//					fread( &id, 4, 1, fnsbca );
//					if (numblock == 1 && r == 0x14 && id == NDS_TYPE_JNT0) {
//						puts( "=================================\nreading 'JNT0'..." );
//						g_anim = readjnt0( fnsbca, 0x14, &g_animlen );
//						if ( !g_anim ) {
//							puts("ERROR readjnt0");
//							return 1;
//						}
//					} else
//						printf("DEBUG: no. of block != 1.\n");
//				} else
//					printf("DEBUG: file size mismatch.\n");
//			} else
//				printf("DEBUG: magic1 not found.\n");
//		} else
//			printf("DEBUG: file is not BCA0.\n");
//		fclose( fnsbca );
//	}
//*/
//	///////////////////////////////////////////////////////
//	// Material is defined as a pair of texture and palette. All textures and palettes have their names.
//	// However, these materials are not associated with any model.
//	// The materials used by a model is only defined by names.
//	// A matching process is required to associate the materials with a model.
//	if ( g_mat ) {
//		g_matnum = texnum > palnum ? texnum : palnum;
//
//		for ( i = 0; i < g_modelnum; i++ )
//		{
//			for ( j = 0; j < g_model[i].matnum; j++ )
//			{
//				for ( r = 0; r < g_matnum; r++ )
//				{
//					MATERIAL *mat1 = &g_mat[r];
//					MATERIAL *mat2 = &g_model[i].material[j];
//
//					// match texture
//					if ( mat1->texsize != 0
//						&& strcmp( mat1->texname, mat2->texname ) == 0 ) {
//
//						printf( "tex '%s' matched.\n", mat2->texname );
//						memcpy( mat2, mat1, 46 );	// NOTE!!! copy 46 bytes, NOT the whole MATERIAL structure
//						mat1->texsize = 0;
//					}
//					// match palette
//					if ( mat2->format != 7 // NB. direct texture has no palette
//						&& mat1->palsize != 0
//						&& strcmp( mat1->palname, mat2->palname ) == 0 ) {
//
//						printf("pal '%s' matched.\n", mat1->palname );
//						strcpy( mat2->palname, mat1->palname );
//						mat2->palsize = mat1->palsize;
//						mat2->paldata = mat1->paldata; 
//						mat1->palsize = 0;
//					}
//				}
//			}
//		}
//	}
//
//	///////////////////////////////////////////////////////
//	// print out the object information
//	for ( i = 0; i < g_modelnum; i++ )
//	{
//		MODEL *mod = &g_model[i];
//		for ( j = 0; j < mod->objnum; j++ )
//		{
//			OBJECT *obj = &mod->object[j];
//			float *m_trans1 = obj->m_trans;
//			printf( "obj %02x [parent %02x] (%9f,%9f,%9f) '%s'\n", j, obj->parent, m_trans1[0], m_trans1[1], m_trans1[2], obj->name );
//			//if ( obj->rot ) printf( "\t\t\trot!\n" );
//		}
//	}
//
//	///////////////////////////////////////////////////////
//	// OpenGL specific
//	printf("Enteing OpenGL...\n");
//	glutInit( &argc, argv );
//	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
//	glutInitWindowSize( 512, 512 );
//	glutInitWindowPosition( 100, 100 );
//	glutCreateWindow( "nsbmd" );
//	glutDisplayFunc( display_func );
//	glutMouseFunc( mouse_func );
//	glutMotionFunc( motion_func );
//	glutKeyboardFunc( kb_func );
//	glutSpecialFunc( sp_func );
//
//	glEnable( GL_DEPTH_TEST );
//	glEnable( GL_TEXTURE_2D );
//	glEnable( GL_CULL_FACE );
//	glAlphaFunc( GL_GREATER, 0.0f );
//	glEnable( GL_ALPHA_TEST );
//	glClearColor( 0.2f, 0.2f, 0.2f, 0.0f );
//
//
//	///////////////////////////////////////////////////////
//	// make texture, and print out a summary of texture
//	if ( g_mat ) {
//		puts("===================================================================");
//		for ( i = 0; i < g_modelnum; i++ )
//		{
//			MODEL *mod = &g_model[i];
//			make_texture( mod );
//
//			for ( j = 0; j < mod->polynum; j++ )
//			{
//				POLYGON *poly = &mod->polygon[j];
//				int matid = poly->matid;
//				MATERIAL *mat = &mod->material[ matid ];
//
//				if ( matid != -1 )
//					printf( "poly %d '%s': matid = %d, texname = '%s', palname = '%s'\n", 
//						j, poly->polyname, matid, mat->texname, mat->palname);
//			}
//		}
//		puts("===================================================================");
//	}
//
//	///////////////////////////////////////////////////////
//	// usage
//	puts( " - Hold left mouse button to rotate" );
//	puts( " - Hold right mouse button to zoom" );
//	puts( " - T toggles texturing" );
//	puts( " - C toggles vertex colours" );
//	puts( " - W toggles wireframe" );
//	puts( " - B toggles backface culling" );
//	puts( " - F toggles texture filtering" );
//	puts( " - P toggles polygon mode" );
//
//	glutMainLoop();
//
//	///////////////////////////////////////////////////////
//	// Termination
//	for ( i = 0; i < g_modelnum; i++ )
//	{
//		MODEL *mod = &g_model[i];
//		for ( j = 0; j < mod->polynum; j++ )
//			free( mod->polygon[j].polydata );
//		free( mod->polygon );
//	}
//
//	free( g_model );
//	if ( g_mat ) free( g_mat );
//	if ( g_anim ) free( g_anim );
//
//	return 0;
//}