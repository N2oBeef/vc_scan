/**************************************************************
 * FileName                     : seed.h
 * Description                  :
 * Version                      : 1.0
 * Author                       : chenmingxiang
 * History                      : 1. 2009-11-17 Create this file.
**************************************************************/
#ifndef _M_SEED_H
#define _M_SEED_H
#include <vector>
#include    "sha1.h"

#pragma pack(push, 1)

#define SHA1_HASH_SIZE      20                      // hash码长20字节
#define SHA1_BLOCK_SIZE     64

#define TORRENT_ERR_OK              0
#define TORRENT_ERR_PARANUM         -1
#define TORRENT_ERR_SCRIPT          -2
#define TORRENT_ERR_OUTDIR          -3
#define TORRENT_ERR_FOPEN           -4
#define TORRENT_ERR_FREAD           -5
#define TORRENT_ERR_TRAVERSE        -6
#define TORRENT_ERR_FILETYPE        -7
#define TORRENT_ERR_INTERNAL        -8

#define TORRENT_MAX_SIZE            1024 * 200      // 4G文件生产的种子文件大约160K
#define TORRENT_DEFAULT_TRACKER     "http://tk.greedland.com/announce"

typedef struct _piece
{
    char hash[20];
}Piece;
typedef std::vector<Piece>          VCTPIECE;
typedef std::vector<std::string>    VCTSTR;
typedef std::vector<UINT32>            VCTINT;
typedef std::vector<UINT64>            VCTINT64;


enum T_ENCODING_TYPE
{
        T_ENCODING_GBK = 0,
        T_ENCODING_UTF8,
        T_ENCODING_BUTT
};

class  CTORRENT
{
public:
    CTORRENT() { __initialize(); }
    ~CTORRENT(){ __clearup(); }

    bool    LoadTorrent(const char* trnfile);
    bool    LoadTorrent(const char* buffer, UINT32 length);
    bool    CreateTorrent(const char* srcfile, const char* optdir);
    bool    CreateTorrent(const char* srcdir, VCTSTR& vtrlist, VCTINT64& vtrsize, const char* optdir);
    bool    CalcTorrentHash(const char* srcfile);
    void    ShowTorrentInfo();
    #if 0
    UINT64     GetFileLength() { return _m_filelength; }
    UINT32     GetPieceNum() { return _m_piecenum; }
    UINT32     GetPieceLength() { return _m_piecelength; }
    UINT32     GetFileNum() { return _m_filenum; }    
    const char*   GetFileName() { return _m_filename.c_str(); }
    const char*   GetTracker() { return _m_tracker.c_str(); }
    #endif
private:
    bool            _m_single;
    UINT32          _m_pathlevel;
    UINT32          _m_encoding;
    
    UINT32          _m_filenum;
    UINT64          _m_filelength;
    UINT32          _m_piecenum;
    UINT32          _m_piecelength;
    std::string     _m_filename;
    std::string     _m_tracker;
    
    VCTPIECE        _m_vct_piece;
    VCTSTR          _m_vct_filelist;
    VCTINT64        _m_vct_filesize;
        
private:
    void            __initialize();
    void            __clearup();
    bool            __create(const char* file, UINT64 filesz);
    bool            __create(const char* dir, VCTSTR& vtrlist, VCTINT64& vtrsize);
    bool            __output(const char* trnfile);
    
    char*           __parse_dictionary(const char* start);
    char*           __parse_list(const char* start);

    bool            __calc_hash(const char* file);
    UINT32          __calc_piecesize(UINT64 filesz);
    UINT64          __calc_filesize(const char* file);
};

#pragma pack(pop)

extern bool P2SP_Utf2Char(const char* src, char* dst, UINT32 length);

#endif
