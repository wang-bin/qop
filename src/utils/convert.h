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

#ifndef CONVERT_H
#define CONVERT_H

#include <cstddef>

/*!
  TODO: optimize size2str. now template is called on x64
*/
//extern "C" {
#if 0
template<typename IntT>
extern char* size2str(IntT size);
#else
char* size2str(size_t size);
#endif
char* msec2str(int msec);
char* msec2secstr(int msec);
char* sec2str(int sec);

typedef char*  (*size_format_converter)(int);
typedef char*  (*time_format_converter)(int);

extern time_format_converter g_time_convert;

//}
#if 0
//char* size2str(unsigned int a);
//to speed up!
#define KInv 1./1024
//const float KInv=1./1024;
template<typename T, typename U>
inline
char* size2str(U s)
{
    char a[-1];
	static const char *unit = "b";
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
	static char ss[10];
	//if(typeid(T)==typeid(double) || typeid(T)==typeid(float)) //typeof
		sprintf(ss, "%.2f%s", v, unit);
	//else
		//sprintf(ss, "%d%s", v, unit);
	return ss;
}
#endif

#endif // CONVERT_H
