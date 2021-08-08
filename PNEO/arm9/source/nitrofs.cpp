#include <cstdlib>
#include <cstring>
#include <sys/dir.h>
#include <sys/fcntl.h>
#include <sys/iosupport.h>
#include <unistd.h>

#include <nds/card.h>
#include <nds/memory.h>
#include <nds/system.h>

#include <fat.h>

static DIR_ITER *nitroFSDirOpen( struct _reent *p_r, DIR_ITER *p_dirState, const char *p_path );
static int       nitroDirReset( struct _reent *p_r, DIR_ITER *p_dirState );
static int       nitroFSDirNext( struct _reent *p_r, DIR_ITER *p_dirState, char *p_filename,
                                 struct stat *p_st );
static int       nitroFSDirClose( struct _reent *p_r, DIR_ITER *p_dirState );
static int       nitroFSOpen( struct _reent *p_r, void *fileStruct, const char *p_path, int p_flags,
                              int p_mode );
static int       nitroFSClose( struct _reent *p_r, void *p_fd );
static int       nitroFSRead( struct _reent *p_r, void *p_fd, char *p_data, size_t p_length );
static off_t     nitroFSSeek( struct _reent *p_r, void *p_fd, off_t p_pos, int p_dir );
static int       nitroFSFstat( struct _reent *p_r, void *p_fd, struct stat *p_st );
static int       nitroFSstat( struct _reent *p_r, const char *p_file, struct stat *p_st );
static int       nitroFSChdir( struct _reent *p_r, const char *p_name );

// static tNDSHeader *__gba_cart_header = (tNDSHeader *) 0x08000000;

#define NITRONAMELENMAX 0x80 // max file name is 127 +1 for zero byte
#define NITROMAXPATHLEN 0x100

#define NITROROOT    0xf000 // root entry_file_id
#define NITRODIRMASK 0x0fff // remove leading 0xf

#define NITROISDIR 0x80 // mask to indicate this name entry is a dir, other 7 bits = name length

// Directory filename subtable entry structure
struct ROM_FNTDir {
    u32 entry_start;
    u16 entry_file_id;
    u16 parent_id;
};

struct ROM_FAT {
    u32 top;    // start of file in rom image
    u32 bottom; // end of file in rom image
};

struct nitroFSStruct {
    unsigned int pos;   // where in the file am i?
    unsigned int start; // where in the rom this file starts
    unsigned int end;   // where in the rom this file ends
};

struct nitroDIRStruct {
    unsigned int   pos;     // where in the file am i?
    unsigned int   namepos; // ptr to next name to lookup in list
    struct ROM_FAT romfat;
    u16 entry_id;   // which entry this is (for files only) incremented with each new file in dir?
    u16 dir_id;     // which directory entry this is
    u16 cur_dir_id; // which directory entry we are using
    u16 parent_id;  // who is the parent of the current directory
    u8  spc;        // system path count.. used by dirnext, when 0=./ 1=../ >=2 actual dirs
};

// Globals!
static u32          fntOffset;   // offset to start of filename table
static u32          fatOffset;   // offset to start of file alloc table
static u16          chdirpathid; // default dir path id...
static int          ndsFileFD = -1;
static unsigned int ndsFileLastpos; // Used to determine need to fseek or not
static bool         cardRead = false;

devoptab_t nitroFSdevoptab = {
    "nitro",
    sizeof( struct nitroFSStruct ), //	int	structSize;
    &nitroFSOpen,  //	int (*open_r)(struct _reent *p_r, void *fileStruct, const char *p_path,int
                   // flags,int mode);
    &nitroFSClose, //	int (*close_r)(struct _reent *p_r,int p_fd);
    NULL,          //	int (*write_r)(struct _reent *p_r,int p_fd,const char *p_data,int p_length);
    &nitroFSRead,  //	int (*read_r)(struct _reent *p_r,int p_fd,char *p_data,int p_length);
    &nitroFSSeek,  //	int (*seek_r)(struct _reent *p_r,int p_fd,int p_pos,int p_dir);
    &nitroFSFstat, //	int (*fstat_r)(struct _reent *p_r,int p_fd,struct stat *p_st);
    &nitroFSstat,  //	int (*stat_r)(struct _reent *p_r, const char *p_file,struct stat *p_st);
    NULL, //	int (*link_r)(struct _reent *p_r, const char *p_existing, const char *p_newLink);
    NULL, //	int (*unlink_r)(struct _reent *p_r,const char *p_name);
    &nitroFSChdir, //	int (*chdir_r)(struct _reent *p_r,const char *p_name);
    NULL, //	int (*rename_r) (struct _reent *p_r, const char *oldName, const char *newName);
    NULL, //	int (*mkdir_r) (struct _reent *p_r, const char *p_path, int mode);
    sizeof( struct nitroDIRStruct ), //	int dirStateSize;
    &nitroFSDirOpen,  //	DIR_ITER* (*diropen_r)(struct _reent *p_r, DIR_ITER *p_dirState, const
                      // char *p_path);
    &nitroDirReset,   //	int (*dirreset_r)(struct _reent *p_r, DIR_ITER *p_dirState);
    &nitroFSDirNext,  //	int (*dirnext_r)(struct _reent *p_r, DIR_ITER *p_dirState, char *filename,
                      // struct stat *filestat);
    &nitroFSDirClose, //	int (*dirclose_r)(struct _reent *p_r, DIR_ITER *p_dirState);
    NULL, //	int (*statvfs_r)(struct _reent *p_r, const char *p_path, struct statvfs *buf);
    NULL, //	int (*ftruncate_r)(struct _reent *p_r, int p_fd, off_t p_length);
    NULL, //	int (*fsync_r)(struct _reent *p_r, int p_fd);
    NULL, //	void *deviceData;
    NULL, //	int (*chmod_r)(struct _reent *p_r, const char *p_path, mode_t mode);
    NULL, //	int (*fchmod_r)(struct _reent *p_r, int p_fd, mode_t mode);
    NULL, NULL, NULL };

char NITRO_PATH[ PATH_MAX ] = { 0 };

bool nitroFSInit( char **p_basepath ) {
    bool nitroInit = false;

    if( __NDSHeader->fatSize == 0 ) return false;

    sysSetCartOwner( BUS_OWNER_ARM9 );
    sysSetCardOwner( BUS_OWNER_ARM9 );

    // test for argv & open nds file
    if( __system_argv->argvMagic == ARGV_MAGIC && __system_argv->argc >= 1 ) {
        if( strncmp( __system_argv->argv[ 0 ], "fat", 3 ) == 0
            || strncmp( __system_argv->argv[ 0 ], "sd", 2 ) == 0 ) {
            if( fatInitDefault( ) ) {
                ndsFileFD = open( __system_argv->argv[ 0 ], O_RDONLY );
                if( ndsFileFD != -1 ) {
                    nitroInit = true;
                    getcwd( NITRO_PATH, PATH_MAX );
                }
            }
        }
    }

    // fallback to direct card reads for desmume
    // TODO: validate nitrofs
    if( !nitroInit ) {
        cardRead  = true;
        nitroInit = true;
        strcpy( NITRO_PATH, "nitro:/" );
    }

    if( nitroInit ) {
        fntOffset = __NDSHeader->filenameOffset;
        fatOffset = __NDSHeader->fatOffset;
        AddDevice( &nitroFSdevoptab );
        chdir( NITRO_PATH );
    }

    if( p_basepath != NULL ) { *p_basepath = NITRO_PATH; }
    return nitroInit;
}

// cannot read across block boundaries (multiples of 0x200 bytes)
static void nitroSubReadBlock( u32 p_pos, u8 *p_data, u32 p_length ) {
    if( ( p_length & 3 ) == 0 && ( ( (u32) p_data ) & 3 ) == 0 && ( p_pos & 3 ) == 0 ) {
        // if everything is word-aligned, read directly
        cardParamCommand( 0xB7, p_pos,
                          CARD_DELAY1( 0x1FFF ) | CARD_DELAY2( 0x3F ) | CARD_CLK_SLOW | CARD_nRESET
                              | CARD_SEC_CMD | CARD_SEC_DAT | CARD_ACTIVATE | CARD_BLK_SIZE( 1 ),
                          (u32 *) p_data, p_length >> 2 );
    } else {
        // otherwise, use a temporary buffer
        static u32 temp[ 128 ];
        cardParamCommand( 0xB7, ( p_pos & ~0x1ff ),
                          CARD_DELAY1( 0x1FFF ) | CARD_DELAY2( 0x3F ) | CARD_CLK_SLOW | CARD_nRESET
                              | CARD_SEC_CMD | CARD_SEC_DAT | CARD_ACTIVATE | CARD_BLK_SIZE( 1 ),
                          temp, 0x200 >> 2 );
        memcpy( p_data, ( (u8 *) temp ) + ( p_pos & 0x1FF ), p_length );
    }
}

static int nitroSubReadCard( unsigned int *p_npos, void *p_data, u32 p_length ) {
    u8 *ptr_u8    = (u8 *) p_data;
    u32 remaining = p_length;

    if( ( *p_npos ) & 0x1FF ) {
        u32 amt = 0x200 - ( *p_npos & 0x1FF );
        if( amt > remaining ) { amt = remaining; }
        nitroSubReadBlock( *p_npos, ptr_u8, amt );
        remaining -= amt;
        ptr_u8 += amt;
        *p_npos += amt;
    }

    while( remaining >= 0x200 ) {
        nitroSubReadBlock( *p_npos, ptr_u8, 0x200 );
        remaining -= 0x200;
        ptr_u8 += 0x200;
        *p_npos += 0x200;
    }

    if( remaining > 0 ) {
        nitroSubReadBlock( *p_npos, ptr_u8, remaining );
        *p_npos += remaining;
    }
    return p_length;
}

static inline int nitroSubRead( unsigned int *p_npos, void *p_data, u32 p_length ) {
    if( cardRead ) {
        unsigned int tmpPos = *p_npos;
        p_length            = nitroSubReadCard( &tmpPos, p_data, p_length );
    } else if( ndsFileFD != -1 ) { // read from ndsfile
        if( ndsFileLastpos != *p_npos ) lseek( ndsFileFD, *p_npos, SEEK_SET );
        p_length = read( ndsFileFD, p_data, p_length );
    }

    if( p_length > 0 ) { *p_npos += p_length; }

    ndsFileLastpos = *p_npos; // save the current file position

    return p_length;
}

static inline void nitroSubSeek( unsigned int *p_npos, int p_pos, int p_dir ) {
    if( ( p_dir == SEEK_SET ) || ( p_dir == SEEK_END ) ) { // otherwise just set the pos :)
        *p_npos = p_pos;
    } else if( p_dir == SEEK_CUR ) {
        *p_npos += p_pos;
    }
}

// Directory functions
static DIR_ITER *nitroFSDirOpen( struct _reent *p_r, DIR_ITER *p_dirState, const char *p_path ) {
    struct nitroDIRStruct *dirStruct = (struct nitroDIRStruct *) p_dirState->dirStruct;
    struct stat            st;
    char                   dirname[ NITRONAMELENMAX ];
    char *                 cptr;
    char                   mydirpath[ NITROMAXPATHLEN ]; // to hold copy of path string
    char *                 dirpath = mydirpath;
    bool                   pathfound;

    if( ( cptr = strchr( p_path, ':' ) ) ) {
        p_path = cptr + 1; // move path past any device names
    }

    strncpy( dirpath, p_path, sizeof( mydirpath ) - 1 );

    dirStruct->pos = 0;

    if( *dirpath == '/' ) {                // if first character is '/' use absolute root path
        dirStruct->cur_dir_id = NITROROOT; // first root dir
    } else {
        dirStruct->cur_dir_id = chdirpathid; // else use chdirpath
    }
    nitroDirReset( p_r, p_dirState ); // set dir to current path

    do {
        while( dirpath[ 0 ] == '/' ) {
            dirpath++; // move past leading /
        }
        cptr = strchr( dirpath, '/' );

        if( cptr ) {
            *cptr = 0; // erase /
        }
        if( *dirpath == 0 ) { // are we at the end of the path string?? if so there is nothing to
                              // search for we're already here !
            pathfound
                = true; // mostly this handles searches for root or /  or no path specified cases
            break;
        }
        pathfound = false;

        while( nitroFSDirNext( p_r, p_dirState, dirname, &st ) == 0 ) {
            if( S_ISDIR( st.st_mode )
                && !( strcmp( dirname,
                              dirpath ) ) ) { // if its a directory and name matches dirpath
                dirStruct->cur_dir_id = dirStruct->dir_id; // move us to the next dir in tree
                nitroDirReset( p_r, p_dirState ); // set dir to current path we just found...
                pathfound = true;
                break;
            }
        };
        if( !pathfound ) { break; }
        dirpath = cptr + 1; // move to right after last / we found
    } while( cptr );        // go till after the last /

    if( pathfound ) {
        return p_dirState;
    } else {
        return NULL;
    }
}

static int nitroFSDirClose( struct _reent *, DIR_ITER * ) {
    return 0;
}

/*Consts containing relative system path strings*/
const char *syspaths[ 2 ] = { ".", ".." };

// reset dir to start of entry selected by dirStruct->cur_dir_id
static int nitroDirReset( struct _reent *, DIR_ITER *p_dirState ) {
    struct nitroDIRStruct *dirStruct = (struct nitroDIRStruct *) p_dirState->dirStruct;
    struct ROM_FNTDir      dirsubtable;
    unsigned int *         pos = &dirStruct->pos;
    nitroSubSeek(
        pos, fntOffset + ( ( dirStruct->cur_dir_id & NITRODIRMASK ) * sizeof( struct ROM_FNTDir ) ),
        SEEK_SET );
    nitroSubRead( pos, &dirsubtable, sizeof( dirsubtable ) );
    dirStruct->namepos  = dirsubtable.entry_start; // set namepos to first entry in this dir's table
    dirStruct->entry_id = dirsubtable.entry_file_id; // get number of first file ID in this branch
    dirStruct->parent_id = dirsubtable.parent_id;    // save parent ID
    dirStruct->spc       = 0; // system path counter, first two dirnext's deliver . and ..
    return 0;
}

static int nitroFSDirNext( struct _reent *, DIR_ITER *p_dirState, char *p_filename,
                           struct stat *p_st ) {
    unsigned char          next;
    struct nitroDIRStruct *dirStruct = (struct nitroDIRStruct *) p_dirState->dirStruct;
    unsigned int *         pos       = &dirStruct->pos;
    if( dirStruct->spc <= 1 ) {
        if( p_st ) { p_st->st_mode = S_IFDIR; }
        if( ( dirStruct->spc == 0 )
            || ( dirStruct->cur_dir_id == NITROROOT ) ) { // "." or its already root (no parent)
            dirStruct->dir_id = dirStruct->cur_dir_id;
        } else { // ".."
            dirStruct->dir_id = dirStruct->parent_id;
        }
        strcpy( p_filename, syspaths[ dirStruct->spc++ ] );
        return 0;
    }
    nitroSubSeek( pos, fntOffset + dirStruct->namepos, SEEK_SET );
    nitroSubRead( pos, &next, sizeof( next ) );
    // next: high bit 0x80 = entry isdir.. other 7 bits are size, the 16 bits following name are
    // dir's entryid (starts with f000)
    //  00 = end of table //
    if( next ) {
        if( next & NITROISDIR ) {
            if( p_st ) { p_st->st_mode = S_IFDIR; }
            next &= NITROISDIR ^ 0xff; // invert bits and mask off 0x80
            nitroSubRead( pos, p_filename, next );
            nitroSubRead( &dirStruct->pos, &dirStruct->dir_id,
                          sizeof( dirStruct->dir_id ) ); // read the dir_id
            dirStruct->namepos
                += next + sizeof( u16 ) + 1; // now we point to next one plus dir_id size
        } else {
            if( p_st ) { p_st->st_mode = 0; }
            nitroSubRead( pos, p_filename, next );
            dirStruct->namepos += next + 1; // now we point to next one
            // read file info to get filesize (and for fileopen)
            nitroSubSeek( pos, fatOffset + ( dirStruct->entry_id * sizeof( struct ROM_FAT ) ),
                          SEEK_SET );
            nitroSubRead( pos, &dirStruct->romfat,
                          sizeof( dirStruct->romfat ) ); // retrieve romfat entry (contains
                                                         // filestart and end positions)
            dirStruct->entry_id++;                       // advance ROM_FNTStrFile ptr
            if( p_st ) {
                p_st->st_size
                    = dirStruct->romfat.bottom - dirStruct->romfat.top; // calculate filesize
            }
        }
        p_filename[ (int) next ] = 0; // zero last char
        return 0;
    } else {
        return -1;
    }
}

static int nitroFSOpen( struct _reent *, void *fileStruct, const char *p_path, int, int ) {
    struct nitroFSStruct *fatStruct = (struct nitroFSStruct *) fileStruct;
    struct nitroDIRStruct dirStruct;
    DIR_ITER              dirState;
    dirState.dirStruct = &dirStruct; // create a temp dirstruct
    struct _reent dre;
    struct stat   st;                     // all these are just used for reading the dir ~_~
    char dirfilename[ NITROMAXPATHLEN ];  // to hold a full path (i tried to avoid using so much
                                          // stack but blah :/)
    const char *filename;                 // to hold filename
    const char *cptr;                     // used to string searching and manipulation
    cptr     = p_path + strlen( p_path ); // find the end...
    filename = NULL;

    do {
        if( ( *cptr == '/' )
            || ( *cptr == ':' ) ) { // split at either / or : (whichever comes first form the end!)
            cptr++;
            strncpy( dirfilename, p_path,
                     cptr - p_path ); // copy string up till and including/ or : zero rest
            dirfilename[ cptr - p_path ] = 0;
            filename                     = cptr; // filename = now remainder of string
            break;
        }
    } while( cptr-- != p_path ); // search till start

    if( !filename ) {
        filename         = (const char *) p_path; // filename = complete path
        dirfilename[ 0 ] = 0;                     // make directory path ""
    }

    if( nitroFSDirOpen( &dre, &dirState, dirfilename ) ) {
        fatStruct->start = 0;
        while( nitroFSDirNext( &dre, &dirState, dirfilename, &st ) == 0 ) {
            if( !( st.st_mode & S_IFDIR ) && ( strcmp( dirfilename, filename ) == 0 ) ) {
                fatStruct->start = dirStruct.romfat.top;
                fatStruct->end   = dirStruct.romfat.bottom;
                break;
            }
        }

        nitroFSDirClose( &dre, &dirState );

        if( fatStruct->start ) {
            nitroSubSeek( &fatStruct->pos, fatStruct->start, SEEK_SET ); // seek to start of file
            return 0;                                                    // woot!
        }
    }
    return -1;
}

static int nitroFSClose( struct _reent *, void * ) {
    return 0;
}

static int nitroFSRead( struct _reent *, void *p_fd, char *p_data, size_t p_length ) {
    struct nitroFSStruct *fatStruct = (struct nitroFSStruct *) p_fd;
    unsigned int *        npos      = &fatStruct->pos;
    if( *npos + p_length > fatStruct->end ) {
        p_length = fatStruct->end - *npos; // dont read past the end
    }
    if( *npos > fatStruct->end ) {
        return 0; // hit eof
    }
    return ( nitroSubRead( npos, p_data, p_length ) );
}

static off_t nitroFSSeek( struct _reent *, void *p_fd, off_t p_pos, int p_dir ) {
    // need check for eof here...
    struct nitroFSStruct *fatStruct = (struct nitroFSStruct *) p_fd;
    unsigned int *        npos      = &fatStruct->pos;
    if( p_dir == SEEK_SET ) {
        p_pos += fatStruct->start; // add start from .nds file offset
    } else if( p_dir == SEEK_END ) {
        p_pos += fatStruct->end; // set start to end of file
    }
    if( p_pos > fatStruct->end ) {
        return -1; // dont read past the end
    }
    nitroSubSeek( npos, p_pos, p_dir );
    return *npos - fatStruct->start;
}

static int nitroFSFstat( struct _reent *, void *p_fd, struct stat *p_st ) {
    struct nitroFSStruct *fatStruct = (struct nitroFSStruct *) p_fd;
    p_st->st_size                   = fatStruct->end - fatStruct->start;
    return 0;
}

static int nitroFSstat( struct _reent *p_r, const char *p_file, struct stat *p_st ) {
    struct nitroFSStruct  fatStruct;
    struct nitroDIRStruct dirStruct;
    DIR_ITER              dirState;

    if( nitroFSOpen( NULL, &fatStruct, p_file, 0, 0 ) >= 0 ) {
        p_st->st_mode = S_IFREG;
        p_st->st_size = fatStruct.end - fatStruct.start;
        return 0;
    }

    dirState.dirStruct = &dirStruct;
    if( nitroFSOpen( NULL, &fatStruct, p_file, 0, 0 ) >= 0 ) {
        p_st->st_mode = S_IFREG;
        p_st->st_size = fatStruct.end - fatStruct.start;
        return 0;
    }
    if( ( nitroFSDirOpen( p_r, &dirState, p_file ) != NULL ) ) {
        p_st->st_mode = S_IFDIR;
        nitroFSDirClose( p_r, &dirState );
        return 0;
    }

    return -1;
}

static int nitroFSChdir( struct _reent *p_r, const char *p_name ) {
    struct nitroDIRStruct dirStruct;
    DIR_ITER              dirState;
    dirState.dirStruct = &dirStruct;
    if( ( p_name != NULL ) && ( nitroFSDirOpen( p_r, &dirState, p_name ) != NULL ) ) {
        chdirpathid = dirStruct.cur_dir_id;
        nitroFSDirClose( p_r, &dirState );
        return 0;
    } else {
        return -1;
    }
}
