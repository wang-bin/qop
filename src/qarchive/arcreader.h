/******************************************************************************
	ArcReader: Archive information reader
	Copyright (C) 2010 Wangbin <wbsecg1@gmail.com>
	(aka. nukin in ccmove & novesky in http://forum.motorolafans.com)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/
#ifndef ARCREADER_H
#define ARCREADER_H

//default is Qt
//#define ARCREADER_QT
//#define ARCREADER_STL
#if defined(ARCREADER_QT)
#	ifdef ARCREADER_STL
#		undef ARCREADER_STL
#	endif //ARCREADER_STL
#elif defined(ARCREADER_STL)
#	ifdef ARCREADER_QT
#		undef ARCREADER_QT
#	endif //ARCREADER_QT
#else
#	define ARCREADER_QT
#endif


#ifdef ARCREADER_STL
#include <string>
#include <string.h>
#include <map>
#else
#include <qstring.h>
#include <qmap.h>
#if QT_VERSION >= 0x040000
#	define ARCREADER_QT4 1
#else
#	define ARCREADER_QT4 0
#endif
#endif //ARCREADER_STL

/*! Endian: busybox/include/platform.h*/
//#if defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64) || defined(__i386__) || defined(__x86_64__)
#if __BYTE_ORDER == __LITTLE_ENDIAN
//#pragma message("little endian")
//#define LITTLE_ENDIAN 1
//#define BIG_ENDIAN 0
#endif
/*
#ifndef LITTLE_ENDIAN
#undef BIG_ENDIAN
#define BIG_ENDIAN 0
#define LITTLE_ENDIAN !(BIG_ENDIAN)
#endif
#undef BYTE_ORDER
#if ( BIG_ENDIAN>0 )
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif
*/
//#include <vector>
namespace Archive {

enum Format {
	FormatArj, FormatZip, FormatRar, FormatTar, FormatGzip, FormatBzip2, \
	Format7zip, FormatLzip, FormatLzop, FormatXz, FormatLzma, FormatLha, FormatUnknow
};

struct Magic {
	const char* name;
	Format format;
	unsigned char data[8]; //how to use container
	size_t offset;

	//a const object can only call const/static functions. checkData(const Magic&,uchar*);
	size_t size() const {	return strlen((const char*)data)/sizeof(unsigned char);}
};

/*!
typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef unsigned long UDWORD;
struct RAR20_archive_entry
{				//stored in RAR v2.0 archives
  char *Name;
  UWORD NameSize;
  UDWORD PackSize;
  UDWORD UnpSize;
  UBYTE HostOS;		// MSDOS=0,OS2=1,WIN32=2,UNIX=3
  UDWORD FileCRC;
  UDWORD FileTime;
  UBYTE UnpVer;
  UBYTE Method;
  UDWORD FileAttr;
};
*/
//extern const  Magic magics[];
const Magic magics[]={
	{"gzip",		FormatGzip,		{ 0x1f, 0x8b },									0},
	{"zip",			FormatZip,		{ 0x50, 0x4b, 0x03, 0x04 },						0}, //PK
	{"zip-outdated",FormatZip,		{ 0x50, 0x40, 0x30, 0x30 },						0},
	{"rar",			FormatRar,		{ 0x52, 0x61, 0x72, 0x21 },						0},//{"rar",Rar,{'R','a','r','!'},0},
	{"bzip2",		FormatBzip2,	{ 0x42, 0x5A, 0x68 },							0}, //{"bzip2",Bzip2,{'B','Z','h'},0},
	{"7-zip",		Format7zip,		{ 0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C },			0},//{"7-zip",_7zip,{'7','z','集','''},0}, //0x377ABCAF271CLL) //"7z...."
	{"xz",			FormatXz,		{ 0xfd, 0x37, 0x7A, 0x58, 0x5A },				0}, //{"xz",Xz,{0xfd,'7','z','X','Z'},0},
	{"lzip",		FormatLzip,		{'L','Z','I','P'},								0},
	//{"lzma",		FormatLzma,		{'L','Z','I','P'},								0},
	{"lzo",			FormatLzop,		{ 0x00, 0xe9, 0x4c, 0x5a, 0x4f, 0xff, 0x1a },	0},
	{"arj",			FormatArj,		{ 0x06, 0xEA },									0},
	{"lha-lh0",		FormatLha,		{'-','l','h','0','-'},							2},
	{"lha-lh1",		FormatLha,		{'-','l','h','1','-'},							2},
	{"lha-lh2",		FormatLha,		{'-','l','h','2','-'},							2},
	{"lha-lh3",		FormatLha,		{'-','l','h','3','-'},							2},
	{"lha-lh4",		FormatLha,		{'-','l','h','4','-'},							2},
	{"lha-lh5",		FormatLha,		{'-','l','h','5','-'},							2},
	{"lha-lh6",		FormatLha,		{'-','l','h','6','-'},							2},
	{"lha-lh7",		FormatLha,		{'-','l','h','7','-'},							2},
	{"lha-lz4",		FormatLha,		{'-','l','z','4','-'},							2},
	{"lha-lz5",		FormatLha,		{'-','l','z','5','-'},							2},
	{"lha-lzs",		FormatLha,		{'-','l','z','s','-'},							2},
	{"unknow",		FormatUnknow,	{},												0}

};


#ifdef ARCREADER_STL
typedef std::map<std::string,Format> FormatMap;

class ArcReader
{
public:
	ArcReader(const std::string& file=0);
	~ArcReader();

	void setFile(const std::string& file);
	Format formatByMagic();
	Format formatByName();

	size_t uncompressedSize();

private:
	FormatMap format_map;
	std::string fileName;
	Format format;

};

typedef ArcReader QArcReader;
/*struct extention_map {
	std::string ext;
	Format format;
};*/
#else
/*struct extention_map {
	QString ext;
	Format format;
};*/
typedef QMap<QString,Format> FormatMap;

class QArcReader
{
public:
	QArcReader(const QString& file=0);
	~QArcReader();

	void setFile(const QString& file);
	Format formatByMagic();
	Format formatByName();

	size_t uncompressedSize();

private:
	FormatMap format_map;
	QString fileName;
	Format format;
};

typedef QArcReader ArcReader;

#endif //ARCREADER_STL
} //namespace Archive

#endif // ARCREADER_H
