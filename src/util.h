/******************************************************************************
	Utils
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
#ifndef UTIL_H
#define UTIL_H

#define ROUND512(x) (((x)+0x1ff) & (~0x1ff))  // ((((x)+511)>>9)<<9)
#define ROUND2P(x,p) ((((x)+(2<<(p))-1)>>p)<<p)

#include <qstring.h>
#include <stdio.h>
#include "global.h"

#if CONFIG_QT4
#define QFILENAME(path) QString(path).mid(QString(path).lastIndexOf('/')+1)
#else
#define QFILENAME(path) QString(path).mid(QString(path).findRev('/')+1)
#endif


//to speed up!
#define KInv 1./1024
//const float KInv=1./1024;
extern QString size2Str(unsigned int a);
template<typename T> QString size2Str(unsigned int s)
{
	QString unit("b");
	T v=(T)s;
	if(v>(T)1024) {
		v*=KInv; //v/=1024;
		unit="Kb";
	}
	if(v>(T)1024) {
		v*=KInv;
		unit="Mb";
	}
	if(v>(T)1024) {
		v*=KInv;
		unit="G";
	}
	return QString().sprintf("%.2f",(double)v)+unit;
}

//extern int isBigEndian();
//extern int isLittleEndian();
extern void swap_endian(char* buf,int type_bytes);

extern bool checkHexData(unsigned int magic,unsigned char* data,size_t offset=0);

extern const char* reverse_string(const char *string);

//template<typename T> T reverse(T src,int len)
template<typename T> T* reverse(T* src,int len)
{
	T* tmp = src;
	T* ret = tmp;
	T* p = tmp + len - 1;
	while (p > tmp) {
		*p ^= *tmp ^= *p ^= *tmp;
		// *p += *tmp; *tmp = *p - *tmp; *p -= *tmp;
		--p; ++tmp;
	}
	return ret;
}

extern const char* getFileName(const char* path);
extern const char* getFileDir(const char* path);

extern int parseOct(const char *p, size_t n);


inline unsigned int readUInt(const unsigned char *data)
{
	return (data[0]) + (data[1]<<8) + (data[2]<<16) + (data[3]<<24);
}

inline unsigned short readUShort(const unsigned char *data)
{
	return (data[0]) + (data[1]<<8);
}

inline void writeUInt(unsigned char *data, unsigned int i)
{
	data[0] = i & 0xff;
	data[1] = (i>>8) & 0xff;
	data[2] = (i>>16) & 0xff;
	data[3] = (i>>24) & 0xff;
}

inline void writeUShort(unsigned char *data, unsigned short i)
{
	data[0] = i & 0xff;
	data[1] = (i>>8) & 0xff;
}

inline void copyUInt(unsigned char *dest, const unsigned char *src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

inline void copyUShort(unsigned char *dest, const unsigned char *src)
{
	dest[0] = src[0];
	dest[1] = src[1];
}

namespace UTIL {
/*
inline
const char* qstr2cstr(const QString& qstr){
#if CONFIG_QT4
	return qstr.toLocal8Bit().constData();
#else
	return qstr.local8Bit().data();
#endif
}
*/
#if CONFIG_QT4
#define qstr2cstr(qstr) qstr.toLocal8Bit().constData()
#else
#define qstr2cstr(qstr) qstr.local8Bit().data()
#endif //CONFIG_QT4

//sleep和usleep都已经obsolete，建议使用nanosleep代替
void qSleep(int ms);
void qWait(int ms);
} //nameespace UTIL
#endif // UTIL_H
