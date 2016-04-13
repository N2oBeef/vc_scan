/**************************************************************
 * FileName                     : seed.cpp
 * Description                  :
 * Version                      : 1.0
 * Author                       : chenmingxiang
 * History                      : 1. 2009-11-17 Create this file.
**************************************************************/
#include   "stdafx.h"
#include   "seed.h"
#include   <algorithm> 
#include   <iostream> 
#include   <string> 
#include   <time.h>
#include   <math.h>
#include   <sys/types.h>
#include   <sys/stat.h>
#include   <stdio.h>
#include   <errno.h>

#if 1   // becode编码函数组
size_t bencode_begin_dict(FILE *fp)
{
    return (EOF == fputc('d',fp)) ? 0 : 1;
}

size_t bencode_begin_list(FILE *fp)
{
    return (EOF == fputc('l',fp)) ? 0 : 1;
}

size_t bencode_end_dict_list(FILE *fp)
{
    return (EOF == fputc('e',fp)) ? 0 : 1;
}

size_t bencode_buf(const char *buf, size_t len, FILE *fp)
{
    fprintf(fp, "%u:", (UINT32)len);
    return fwrite(buf, len, 1, fp) == 1 ? 1 : 0;
}

size_t bencode_blk(const char *buf, size_t len, FILE *fp)
{
    return fwrite(buf, len, 1, fp) == 1 ? 1 : 0;
}

size_t bencode_str(const char *str, FILE *fp)
{
    return bencode_buf(str, strlen(str), fp);
}

size_t bencode_int(const int integer, FILE *fp)
{
    if( EOF == fputc('i', fp)) return 0;
    fprintf(fp, "%u", (UINT32)integer);
    return (EOF == fputc('e', fp)) ? 0: 1;
}
size_t bencode_int64(const UINT64 integer, FILE *fp)
{
    if( EOF == fputc('i', fp)) return 0;
    fprintf(fp, "%lld", (UINT64)integer);
    return (EOF == fputc('e', fp)) ? 0: 1;
}

#endif

/****************************************
 * Function             : WrapUnrecogChar
 * Description          : 转换字符串中的不可识别字符为'_'
 * Parameters           : 
 * Return               : null
****************************************/
void WrapUnrecogChar(std::string& strpath)
{
	std::replace(strpath.begin(),strpath.end(),'?'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'/'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'|'  ,'_');
	std::replace(strpath.begin(),strpath.end(),':'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'*'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'<'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'>'  ,'_');
	std::replace(strpath.begin(),strpath.end(),'\\' ,'_');
	std::replace(strpath.begin(),strpath.end(),'\"' ,'_');
	std::replace(strpath.begin(),strpath.end(),'\'' ,'_');
}

/****************************************
 * Function             : CTORRENT::__initialize
 * Description          : 初始化成员变量(清空)
 * Parameters           : null
 * Return               : null
****************************************/
void CTORRENT::__initialize()
{
    _m_single       = true;
    _m_pathlevel    = 0;
    _m_encoding     = T_ENCODING_GBK;
    
    _m_filenum      = 0;
	_m_filelength   = 0;
    _m_piecenum     = 0;
    _m_piecelength  = 0;
    _m_filename     = "";
    _m_tracker      = "";
}

/****************************************
 * Function             : CTORRENT::__clearup
 * Description          : 释放成员变量
 * Parameters           : null
 * Return               : null
****************************************/
void CTORRENT::__clearup()
{
    if(_m_vct_piece.size() > 0)     _m_vct_piece.clear();
    if(_m_vct_filelist.size() > 0)  _m_vct_filelist.clear();
    if(_m_vct_filesize.size() > 0)  _m_vct_filesize.clear();
}

/****************************************
 * Function             : CTORRENT::__create
 * Description          : 从源文件创建种子文件属性
 * Parameters           : file      - 源文件名
 *                        filesz    - 源文件大小
 * Return               : 成功      - true
 *                        失败      - false
****************************************/
bool CTORRENT::__create(const char* file, UINT64 filesz)
{
    if(file == NULL)        return false;
    if(filesz == 0 )
    {
        printf("Error. file(%s) size zero\r\n", file);
        return false;       // 不处理大小为0的文件
    }

    _m_tracker      = TORRENT_DEFAULT_TRACKER;
    _m_filename     = file;
    _m_filenum      = 1;
    _m_single       = true;
    
	_m_filelength   = filesz;
    _m_vct_filelist.push_back(file);
    _m_vct_filesize.push_back(filesz);
    
    _m_piecelength   = __calc_piecesize(_m_filelength);
    __calc_hash(file);
    _m_piecenum      = _m_vct_piece.size();
    
    return true;
}

/****************************************
 * Function             : CTORRENT::__create
 * Description          : 从源目录创建种子文件属性
 * Parameters           : dir       - 源目录名
 *                        vtrlist   - 源目录下的文件名称列表
 *                        vtrsize   - 源目录下的文件大小列表
 * Return               : 成功      - true
 *                        失败      - false
****************************************/
bool CTORRENT::__create(const char* dir, VCTSTR& vtrlist, VCTINT64& vtrsize)
{
    if(dir == NULL)        return false;

    _m_tracker      = TORRENT_DEFAULT_TRACKER;
    _m_filename     = dir;
    _m_filenum      = 0;
    _m_single       = false;

    VCTINT64::iterator it_size = vtrsize.begin(); 
    VCTSTR::iterator it_list = vtrlist.begin(); 
    while(it_size !=  vtrsize.end())
    {
        if(*it_size != 0)   // 不处理大小为0的文件
        {           
            _m_filenum++;
            _m_filelength += *it_size;
            _m_vct_filelist.push_back(*it_list);
            _m_vct_filesize.push_back(*it_size);
        }
        it_size++;
        it_list++;
    }

    if(_m_filelength == 0)        return false; // 不处理大小为0的目录
    
    _m_piecelength   = __calc_piecesize(_m_filelength);
    VCTSTR::iterator it = _m_vct_filelist.begin(); 
    while(it !=  _m_vct_filelist.end())
    {
        __calc_hash((*it).c_str());     // 每个文件产生独立hash，不采用文件拼接
        it++;
    }
    _m_piecenum      = _m_vct_piece.size();

    return true;
}

/****************************************
 * Function             : CTORRENT::__output
 * Description          : 根据属性，生成bencode格式的种子文件
 * Parameters           : trnfile - 种子文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::__output(const char* trnfile)
{
    FILE* fp = fopen(trnfile, "wb");
    if(fp == NULL)
    {
        printf("Error! open trt file(%s) fail\r\n", trnfile);
        return false;
    }

    bencode_begin_dict(fp);             // Begin首字典序

	//bencode_str("announce",fp);         // Tracker Url
	//bencode_str(_m_tracker.c_str(),fp);

    bencode_str("encoding",fp);         // 编码格式
	bencode_str("GBK",fp);

	bencode_str("creation date",fp);    // 文件时间
	time_t tm;
	time(&tm);
	bencode_str(ctime(&tm),fp);
    
	bencode_str("info",fp);             // Info信息(嵌套字典)
	bencode_begin_dict(fp);                         // Info Begin字典序
	bencode_str("name",fp);                         // 文件名
	WrapUnrecogChar(_m_filename);
	bencode_str(_m_filename.c_str(),fp);
    
    if(_m_single)
    {
        bencode_str("length",fp);                   // 单文件长度
    	bencode_int64(_m_filelength,fp);
    }
    else
    {
        bencode_str("files",fp);                    // 多文件列表
        bencode_begin_list(fp);
        UINT32 idx;
        VCTSTR::iterator it_list = _m_vct_filelist.begin();
        VCTINT64::iterator it_size = _m_vct_filesize.begin();
        for(idx = 0; idx < _m_filenum; idx++)
        {
            bencode_begin_dict(fp);
            bencode_str("length", fp);
            bencode_int64(*it_size, fp);
            bencode_str("path", fp);
            bencode_begin_list(fp);
            WrapUnrecogChar(*it_list);
            bencode_str((*it_list).c_str(), fp);
            bencode_end_dict_list(fp);
            bencode_end_dict_list(fp);
            it_list++;
            it_size++;
        }
        bencode_end_dict_list(fp);
    }
    
	bencode_str("piece length",fp);                 // 分片长度
	bencode_int(_m_piecelength,fp);
    
    bencode_str("pieces",fp);                       // 分片hash列表
    char buf[256];
    sprintf(buf, "%lu:", _m_vct_piece.size() * 20);
    bencode_blk(buf, strlen(buf), fp);
    VCTPIECE::iterator it = _m_vct_piece.begin();
    while(it != _m_vct_piece.end())
    {
        bencode_blk((*it).hash, 20, fp);
        it++;
    }
	bencode_end_dict_list(fp);          // Info End字典序。
	
	bencode_end_dict_list(fp);          // End首字典序。

    fclose(fp);
    return true;
}

/****************************************
 * Function             : CTORRENT::__parse_dictionary
 * Description          : 
 * Parameters           : 
 * Return               :
****************************************/
char* CTORRENT::__parse_dictionary(const char* start)
{
    UINT64 num;
    char* buf = (char*)start;
    if (*buf++ != 'd') return NULL;
    
    while(*buf != 'e')
    {
        //嵌套字典
        if (buf[0] == 'd'||buf[0] == 'l'||buf[0]=='i')  return NULL;
        
        std::string paraname;
		std::string paravalue;
		char* pcolon = buf;
		while((*buf != ':') && (*buf!='e')) buf++;
        if (*buf == 'e')     return NULL;    //结束

        num = atoi(pcolon);
        paraname.append(++buf, num);
        buf += num;

        if (buf[0] == 'd')
        {
            if((buf = __parse_dictionary(buf)) == NULL) return NULL;
        }
        else if(buf[0] == 'l')
        {
            if (paraname == "files") _m_single  = false;
            if (paraname == "path")  _m_pathlevel++;
            if((buf = __parse_list(buf)) == NULL) return NULL;
            if (paraname == "path")  _m_pathlevel--;
        }
        else if (buf[0] == 'i')//数值
        {
			num = atoi(++buf);
            while( *buf != 'e' ) buf++;
            buf++;
            if (paraname == "piece length")
            {
                _m_piecelength  = num;
            }
            if (paraname == "length")
            {
                if(_m_single)
                {
                    _m_filelength  = num; 
                    _m_filenum = 1; 
                }
                else
                { 
                    _m_filelength += num; 
                    _m_filenum++; 
                    _m_vct_filesize.push_back(num); 
                }
            }
        }
        else
        {
            pcolon = buf;
            while(*buf != ':' && *buf!='e') buf++;
            if (*buf == 'e')    return NULL;   //结束
            num = atoi(pcolon);
            paravalue.append(++buf, num);
            buf += num;
            if (paraname == "pieces")
            {
                if ((_m_piecenum = num/20) == 0)    return NULL;
            }
            if (paraname == "announce")
            {
                _m_tracker = paravalue;
            }
            if (paraname == "name")
            {
                _m_filename = paravalue;
            }
            if (paraname == "encoding")
            {
                if(paravalue == "GBK")  _m_encoding = T_ENCODING_GBK;
                else if(paravalue == "UTF-8")  _m_encoding = T_ENCODING_UTF8;
                else _m_encoding = T_ENCODING_BUTT;
            }
        }
    }
    return ++buf;
}

/****************************************
 * Function             : CTORRENT::__parse_list
 * Description          : 
 * Parameters           : 
 * Return               :
****************************************/
char* CTORRENT::__parse_list(const char* start)
{
    char* buf = (char*)start;
    if(*buf++ != 'l') return NULL;
    
    bool _b_multi_path = false;
    std::string path;
    
	while(*buf != 'e')
	{
		if(*buf == 'l')
		{
			buf = __parse_list(buf);
			if(!buf)    return NULL;
		}
		else if(*buf == 'd')
		{
			buf = __parse_dictionary(buf);
			if(!buf)    return NULL;
		}
		else if(*buf == 'i')
		{
			while(*buf!= 'e') buf++;
			buf++;
		}
		else
		{
			char* pcolon = buf;
			while(*buf != ':' && *buf!= 'e') buf++;
			if (*buf == 'e') return NULL;    //结束
            UINT32 num = atoi(pcolon);
            buf++;
            if(_m_pathlevel > 0)
            { 
                if(_b_multi_path) path += '\\'; 
                path.append(buf, num); 
                _b_multi_path = true; 
            }
			buf += num;
		}
	}

    if(_m_pathlevel > 0)    _m_vct_filelist.push_back(path);
	return ++buf;
}

/****************************************
 * Function             : CTORRENT::__calc_hash
 * Description          : 根据文件，初始化成员变量
 * Parameters           : file - 源文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::__calc_hash(const char* file)
{
    FILE* fp = fopen(file, "rb");
    if(fp == NULL)  return false;
    
	char readbuf[512*1024];     // piece最大为512KB
    while(1)
    {
        Piece piece;
        UINT32 size = fread(readbuf, 1, _m_piecelength, fp);
        if(size == 0)   break;
        //CSha1 sha(readbuf, size);
        //sha.read(piece.hash);

		hash_function::CSha1::HashBuffer(readbuf, size, (BYTE *)&piece);

        _m_vct_piece.push_back(piece);
    }
    fclose(fp);
    return true;
}

/****************************************
 * Function             : CTORRENT::__calc_piecesize
 * Description          : 根据文件大小计算分片长度
 *   文件大小              分片长度
 *  ----------            ----------
 * 0      - 4194303        16384
 * 4194304-16777215        32768
 * 16777216-67108863       65536
 * 67108864-268435455      131072
 * 268435456-1073741823    262144
 * 1073741824-4G           524288
 * 4G --                         524288
 *  ----------            ----------
 * Parameters           : filesz - 文件大小
 * Return               : 分片长度
****************************************/
UINT32 CTORRENT::__calc_piecesize(UINT64 filesz)
{
    if (filesz <= 2 * 1024 * 1024)  return 16 * 1024;

	if (filesz >= (UINT64)4 * 1024 * 1024 * 1024)  return 512 * 1024;

	float f = sqrt( (float)filesz/4096);
	unsigned short n = (unsigned short)f;
	int i = 3;
	while(n && i < 16)
	{
		if( n & (0x8000>>i))    return (0x8000 >> i) * 1024;
		i++;
	}

	return 16 * 1024;
}

/****************************************
 * Function             : CTORRENT::__get_filesz
 * Description          : 计算文件大小
 * Parameters           : file - 文件名
 * Return               : 文件大小(Bytes)
****************************************/
UINT64 CTORRENT::__calc_filesize(const char* file)
{
    struct _stati64 sb;
	if(_stati64(file, &sb) != 0)    return 0;
    return sb.st_size;
}

/****************************************
 * Function             : CTORRENT::ShowTorrentInfo
 * Description          : 显示种子文件信息
 * Parameters           : null
 * Return               : null
****************************************/
void CTORRENT::ShowTorrentInfo()
{
    printf("------------ global info ------------\r\n");

    if(_m_single) printf("file name   : %s \r\ntracker list: %s\r\n", _m_filename.c_str(), _m_tracker.c_str());
    else          printf("directory   : %s \r\ntracker list: %s\r\n", _m_filename.c_str(), _m_tracker.c_str());
    printf("total length: %u KB\r\ntotal number: %u\r\n", (int)(_m_filelength/1024), _m_filenum);    
    printf("piece length: %u KB\r\npiece number: %u\r\n", _m_piecelength/1024, _m_piecenum);
    
    if(!_m_single)
    {
        printf("------------ detail info ------------\r\n");
        VCTSTR::iterator it_list = _m_vct_filelist.begin();
        VCTINT64::iterator it_size = _m_vct_filesize.begin();
        for(UINT32 idx = 0; idx < _m_filenum; idx++, it_list++, it_size++)
        {
            UINT32 size = *it_size;
            if(size > 4096) printf("[%03d] file length: %u KB\r\n", idx + 1, size/1024);
            else            printf("[%03d] file length: %u B\r\n", idx + 1, size);
            switch(_m_encoding)
            {
            case T_ENCODING_GBK:
                printf("      file name  : %s\r\n", (*it_list).c_str());
                break;
            default:
                break;
            }
        }
    }
}

/****************************************
 * Function             : CTORRENT::LoadTorrentFromFile
 * Description          : 解析.torrent种子文件
 * Parameters           : trnfile - 种子文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::LoadTorrent(const char* trnfile)
{
    if(trnfile == NULL)     return false;
    
    FILE* fp = fopen(trnfile, "rb");
    if(fp == NULL)          return false;
    char buf[TORRENT_MAX_SIZE];
    int totallen = fread(buf, sizeof(char), TORRENT_MAX_SIZE, fp);
    fclose(fp);
    if((totallen <= 0) || (totallen >= TORRENT_MAX_SIZE))   return false;

    return LoadTorrent(buf, totallen);
}

/****************************************
 * Function             : CTORRENT::LoadTorrentFromBuffer
 * Description          : 解析.torrent种子文件
 * Parameters           : buffer - torrent文件内容
 *                        length - torrent文件长度
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::LoadTorrent(const char* buffer, UINT32 length)
{
    if(buffer == NULL)     return false;
    if(length == 0)        return false;
    if((buffer[0] != 'd') || (buffer[length - 1] != 'e')) return false;
    return __parse_dictionary(buffer) == NULL ? false : true;
}

/****************************************
 * Function             : CTORRENT::CreateTorrent
 * Description          : 制作.torrent种子文件
 * Parameters           : srcfile - 源文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::CreateTorrent(const char* srcfile, const char* optdir)
{
    if(srcfile == NULL)         return false;
    if(optdir  == NULL)         return false;
    if(strlen(optdir) == 0)     return false;
    
    if(__create(srcfile, __calc_filesize(srcfile)) != true)   return false;    

    char* pbuf = (char*)malloc(_m_piecenum * SHA1_HASH_SIZE);
    memset(pbuf, 0, _m_piecenum * SHA1_HASH_SIZE);
    char* pcur = pbuf;
    VCTPIECE::iterator it = _m_vct_piece.begin();
    while(it != _m_vct_piece.end())
    {
        memcpy(pcur, (*it).hash, SHA1_HASH_SIZE);
        pcur += SHA1_HASH_SIZE;
        it++;
    }

    char hash[SHA1_HASH_SIZE];
	//CSha1 sha(pbuf, _m_piecenum * SHA1_HASH_SIZE);
 //   sha.read(hash);
	hash_function::CSha1::HashBuffer(pbuf, _m_piecenum * SHA1_HASH_SIZE, (BYTE *)&hash);

    free(pbuf);

    std::string str = optdir;
    if(optdir[strlen(optdir) - 1] != '\\') str += '\\';
    for(UINT32 idx = 0; idx < SHA1_HASH_SIZE; idx++)
    {
        char code[16];
        sprintf(code, "%02X", (UINT8)hash[idx]);
        str += code;
    }
    str += ".trt";      // 文件后缀trt

    bool _i_res =  __output(str.c_str());
    if(_i_res == true)  printf("%s\r\n", strrchr(str.c_str(), '\\') + 1);
    return _i_res;
}

/****************************************
 * Function             : CTORRENT::CreateTorrent
 * Description          : 制作.torrent种子文件
 * Parameters           : srcfile - 源文件名
                          trnfile - 种子文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::CreateTorrent(const char* srcdir, VCTSTR& vtrlist, VCTINT64& vtrsize, const char* optdir)
{
    if(srcdir == NULL)     return false;

    VCTINT64::iterator it_size = vtrsize.begin(); 
    VCTSTR::iterator it_list = vtrlist.begin(); 
    while(it_size !=  vtrsize.end())
    {
        if(*it_size != 0)   // 不处理大小为0的文件
        {
            __clearup();
            __initialize();
            if(CreateTorrent((*it_list).c_str(), optdir) != true) return false;
        }
        it_size++;
        it_list++;
    }
    return true;
}

/****************************************
 * Function             : CTORRENT::CalcTorrent
 * Description          : 显示文件Hash值
 * Parameters           : srcfile - 源文件名
 * Return               : 成功 - true
 *                        失败 - false
****************************************/
bool CTORRENT::CalcTorrentHash(const char* srcfile)
{
    if(srcfile == NULL)         return false;    
    if(__create(srcfile, __calc_filesize(srcfile)) != true)   return false;    

    char* pbuf = (char*)malloc(_m_piecenum * SHA1_HASH_SIZE);
    memset(pbuf, 0, _m_piecenum * SHA1_HASH_SIZE);
    
    char* pcur = pbuf;
    VCTPIECE::iterator it = _m_vct_piece.begin();
    while(it != _m_vct_piece.end())
    {
        memcpy(pcur, (*it).hash, SHA1_HASH_SIZE);
        pcur += SHA1_HASH_SIZE;
        it++;
    }
    char hash[SHA1_HASH_SIZE];
    //CSha1 sha(pbuf, _m_piecenum * SHA1_HASH_SIZE);
    //sha.read(hash);
	hash_function::CSha1::HashBuffer(pbuf, _m_piecenum * SHA1_HASH_SIZE, (BYTE *)&hash);

    free(pbuf);

    std::string str = "";
    for(UINT32 idx = 0; idx < SHA1_HASH_SIZE; idx++)
    {
        char code[16];
        sprintf(code, "%02X", (UINT8)hash[idx]);
        str += code;
    }
    printf("%s\r\n", str.c_str());
    return true;
}
