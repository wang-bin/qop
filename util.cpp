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

#include "util.h"
#include "global.h"
#include <stdio.h>

/*!
	a=h*b+r, b=2^(10*i)=2^n, r=a-h*b=a-(h<<n)
	(int)r*10^k/b,t=(100*(a-(h<<n)))>>n;
*/
//slower
const QString unit[3]={"b","Kb","Mb"};
QString size2Str(uint a)
{
	uint h=a;
	int n=0,i=0;
	while(h>=1024) {
		h>>=10;
		n+=10;
		++i;
	}
	return QString("%1.%2 ").arg(h).arg((100*(a-(h<<n)))>>n)+unit[i];
}
//faster



//Endian
/*!
int isBigEndian()
{
	union {
		unsigned short a;
		char b;
	} t;
	t.a=1;
	return t.b!=1;
}

int isLittleEndian()
{
	//int i = 0;
	//((char *)(&i))[0] = 1;
	//return (i == 1);

	int x=1;
	return (*(char*)&x==1);
}
*/
void swap_endian(char* buf,int bytes)
{
	int c = bytes>>1;
	for(int i = 0; i < c; i++) {
		char t =buf[i];
		buf[i]=buf[c-1-i];
		buf[c-1-i]=t;
	}
}



bool checkHexData(unsigned int magic,unsigned char* data,size_t offset)
{
	bool eq=true;
	unsigned int mask=0xff;
	size_t len=0;
	unsigned int magic_t=magic;
	while(magic_t>>=8) {
		len+=8;
	}
	printf("magic:0x%x length:%d\n",magic, len);
	data+=offset;
	while(len) {
		mask=0xff<<len; //mask<<=8;
		if(*data!= (magic&mask)>>len ) eq=false;
		len-=8;
	}
	return eq;
}


//template<typename T> reverse(T* str)
//template<typename T> reverse(const T* str) { T* t=new T[], copy, reverse<T>(t)>
//reverse(T*,int begin,int end)
const char* reverse_string(const char *str)
{
	char* tmp = new char[strlen(str) + 1]; //remove it
	memcpy(tmp,str,strlen(str));
	strcpy(tmp,str);
	char* ret = tmp;
	char* p = tmp + strlen(str) - 1;
	while (p > tmp) {
		*p ^= *tmp;
		*tmp ^= *p;
		*p ^= *tmp;
		/*!
		*p = *p + *tmp;
		*tmp = *p - *tmp;
		*p = *p - *tmp;
		*/
		--p; ++tmp;
	}
	return ret;

/*
	char *ptr = str;
	while(*ptr!= '\0') ptr++;
	ptr--;
	//while(str < ptr){
	//	temp = *str;
	//	*str++ = *ptr;
	//	*ptr-- = temp;
	//}
	char temp;
	for(; str < ptr; ptr--,str++){
		temp = *str;
		*str = *ptr; //shit happens; *str++ = *ptr;
		*ptr = temp; // *ptr-- = temp;
	}
	return ptr;
	*/
}


//dir and filename

#if defined(_OS_WIN32_) || defined(_OS_MSDOS_)
#define DELIMITER '\\'
#else
#define DELIMITER '/'
#endif
const char* getFileName(const char* path)
{
	char *fileName=new char[strlen(path)+1];//(char*)malloc((strlen(path)+1)*sizeof(char));
	strcpy(fileName,path);
	if(strchr(path,DELIMITER)!=0) {
		while((*fileName++!='\0'));
		while((*fileName--!=DELIMITER));
		fileName+=2;
	}
	return fileName;
}

const char* getFileDir(const char* path)
{
	if(!strchr(path,DELIMITER)) return ".";
	char* dir=new char[strlen(path)+1];//(char*)malloc((strlen(path)+1)*sizeof(char));
	strcpy(dir,path);
	char* q=dir;
	while((*q++!='\0'));
	q--;
	while((*q--!=DELIMITER));
	q++;
	*q = '\0', q=dir;
	return q;
}


//#if defined(WIN32)
//#include <windef.h>
//#include <WinBase.h>
//#endif
/*
char* get_exe_path()
{
	static char buff[256];
	char *p;
#if defined(WIN32)
	//::GetModuleFileName(NULL, buff, sizeof(buff));
	//p = strrchr(buff, '\\');
#else
	sprintf(buff, "/proc/%d/exe", getpid());
	readlink(buff, buff, sizeof(buff));
	p = strrchr(buff, '/');
#endif
	*p = 0;
	return buff;
}
*/

/* Parse an octal number, ignoring leading and trailing nonsense. */
int parseOct(const char *p, size_t n)
{
	int i = 0;
	while (*p < '0' || *p > '7') {
		++p; --n;
	}
	while (*p >= '0' && *p <= '7' && n > 0) {
		i <<=3;//i *= 8; //
		i += *p - '0';
		++p; --n;
	}
	return (i);
}

#include "global.h"
#if EZXT_QT4
#include <QtCore/QElapsedTimer>
#else
#include <qtimer.h>
typedef QTimer QElapsedTimer;
#endif
#ifdef Q_OS_WIN
//#include <windows.h>
#include <QtTest/QTest>
#endif
namespace UTIL {
//sleep和usleep都已经obsolete，建议使用nanosleep代替
void qSleep(int ms)
{
#ifdef Q_OS_WIN
	QTest::qSleep(ms);
	//Sleep(uint(ms));
#else
	struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
	nanosleep(&ts, NULL);
#endif
}
//inline static
void qWait(int ms)
{
	QElapsedTimer timer;
	timer.start();
	do {
		QApplication::processEvents(QEventLoop::AllEvents, ms);
		qSleep(10);
	} while (timer.elapsed() < ms);
}
} //nameespace UTIL
