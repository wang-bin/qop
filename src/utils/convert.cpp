/******************************************************************************
	Name: description
	Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>

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

#include "convert.h"
#include <stdio.h>
#include <memory.h>
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
//#include <stdlib.h>
#if !__GNUC__
#undef snprintf
#define snprintf _snprintf
#endif
char* size2str(size_t a)
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
	//char *ss = (char*)malloc(11);
	static char ss[11];
	//memset(ss, 0, sizeof(ss));
	snprintf(ss, 11, "%d.%03d%-2s", a>>iShift[i], r, unit[i]);
	return ss;
}

//ISO 8601
char* msec2str(int pMsec)
{
	int ms = pMsec%1000/100; //(pMsec%1000)/100 //hh:mm:ss.x
	pMsec/=1000;
	int sec = pMsec%60;
	int min =(pMsec/60)%60;
	int hour = pMsec/3600;
	static char time[11];
	//memset(time, ' ', sizeof(time));
	snprintf(time, sizeof(time), "%02d:%02d:%02d.%01d", hour, min, sec, ms);
	return time;
}

char* msec2secstr(int pMsec)
{
	//int ms = pMsec%1000; //(pMsec%1000)/100 //sec.x
	//pMsec/=1000;
	static char time[10];
	snprintf(time, sizeof(time), "%.1f", pMsec/1000.0);
	return time;
}

char* sec2str(int pSec)
{
	int sec = pSec%60;
	int min = (pSec/60)%60;
	int hour = pSec/3600;
	static char time[9];
	//memset(time, ' ', sizeof(time));
	snprintf(time, sizeof(time), "%02d:%02d:%02d", hour, min, sec);
	return time;
}


time_format_converter g_time_convert = msec2str;

