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
//slower ?
const double K2Ki = 1000.0/1024.0;
const char* const unit[]={"b", "Kb", "Mb", "Gb"};
//static char ss[11]; //abcd.efg Mb
//snprintf is not supported in MSVC, use _snprintf
#define KiMask ((~0)^(0x400-1)) //0x400==1024, 0x400-1==111111111, KiMask==111...11000000000
const unsigned int kMask[] = {0, (~0)^0x3ff, (~0)^((1<<20)-1), (~0)^((1<<30)-1)};
const unsigned int iMask[] = { 0, (1<<10)-1, (1<<20)-1, (1<<30)-1};//, (1UL<<40)-1}; //To large
const unsigned int iShift[] = { 0, 10, 20, 30, 40};
/*!
	a = a0+a1*1024+a2*1024^2+...+an*1024^n
	q = an, r = a-an*1024^n
	rr = (a&iMask[n])/(1024^n)*1000 == (a&iMask[n])/(1024^(n-1))*(1000/1024) ==(a&iMask[n])>>iShift[n-1]*K2Ki;
*/

#if !__GNUC__
#undef snprintf
#define snprintf _snprintf
#endif
char* size2str(unsigned int a)
{
	int i=0;
#if 1
	//unsigned int q=a;
	//for(;q&KiMask;q>>=10,++i); //for(;q>=1024;q>>=10,++i);
	while (a&kMask[++i]) ; --i;
#else
	while (a>>iShift[++i]); --i;//q>>=iShift[--i];
#endif
	unsigned int r = ((a&iMask[i])>>iShift[i-1])*K2Ki; //(unsigned int)((a&iMask[i])*K2Ki)>>iShift[i-1];
	static char ss[11];
	snprintf(ss, 11, "%d.%03d%-2s", a>>iShift[i], r, unit[i]);
	return ss;
}



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
//todo use windows api
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
		while ((*fileName++!='\0')) ;
		while ((*fileName--!=DELIMITER)) ;
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
	while ((*q++!='\0')) ;
	q--;
	while ((*q--!=DELIMITER)) ;
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




#define SLEEP_QTIME 0
#define SLEEP_WAITCONDITION 1
#define  SLEEP_EVENTLOOP 0
#if QT_VERSION <= 0x040000
#define SLEEP_EVENTLOOP 0
#endif

#include "global.h"
#if QT_VERSION >= 0x040700
#include <QtCore/QElapsedTimer>
#else
#include <qdatetime.h>
typedef QTime QElapsedTimer;
#endif

#if SLEEP_QTIME
#include <qdatetime.h>
#elif SLEEP_EVENTLOOP
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#elif SLEEP_WAITCONDITION
#if CONFIG_QT2
#include <qthread.h>
#else
#include <qwaitcondition.h>
#include <qmutex.h>
#endif //CONFIG_QT2
static QMutex mutex;
#else
#ifdef Q_OS_WIN
#include <windows.h>  //compile error in mingw
#endif //Q_OS_WIN
#endif //SLEEP_QTIME

namespace UTIL {

//sleep和usleep都已经obsolete，建议使用nanosleep代替
void qSleep(int ms)
{
#if SLEEP_QTIME
	QTime dieTime = QTime::currentTime().addMSecs(ms);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#elif SLEEP_EVENTLOOP
	QEventLoop eventloop;
	QTimer::singleShot(ms, &eventloop, SLOT(quit()));
	eventloop.exec();
#elif SLEEP_WAITCONDITION
	mutex.lock();
	static QWaitCondition wait;
	wait.wait(&mutex,ms);
	mutex.unlock();
#else
#ifdef Q_OS_WIN
	Sleep(uint(ms));
#else
	struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
	nanosleep(&ts, NULL);
#endif //Q_OS_WIN
#endif //SLEEP_QTIME

}

//inline static
void qWait(int ms)
{
	QElapsedTimer timer;
	timer.start();
	do {
#if CONFIG_QT4
		//解决界面无法刷新的问题
		QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
#else
		qApp->processEvents(ms);
#endif
		qSleep(10); //解决程序CPU占用率过高的问题
	} while (timer.elapsed() < ms);
}

} //nameespace UTIL
