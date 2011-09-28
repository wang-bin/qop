/******************************************************************************
	version.h: version information
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

#ifndef VERSION_H
#define VERSION_H

template<int major, int minor, int patch>
char* version_str()
{
	char *ver_str = new char[9];
	sprintf(ver_str, "%d.%d.%d", major, minor, patch);
	return ver_str;
}

template<int ver>
struct version {
	enum{
		value = ver,
		major = ((ver&0xff0000)>>16),
		minor = ((ver&0xff00)>>8),
		patch = (ver&0xff)
	};
	//static char* string;
};

#define APP_NAME "qop"

#undef APP_VERSION //0x000300

#define MAJOR 0	//((APP_VERSION&0xff0000)>>16)
#define MINOR 3	//((APP_VERSION&0xff00)>>8)
#define PATCH 5	//(APP_VERSION&0xff)

#define VERSION_CHK(major, minor, patch) \
	(((major&0xff)<<16) | ((minor&0xff)<<8) | (patch&0xff))

#define APP_VERSION VERSION_CHK(MAJOR, MINOR, PATCH)

//#define APP_VERSION_STR version_str<MAJOR, MINOR, PATCH>()
//#define APP_VERSION_STR version<APP_VERSION>::string


/*! Stringify \a x. */
#define _TOSTR(x)   #x
/*! Stringify \a x, perform macro expansion. */
#define TOSTR(x)  _TOSTR(x)

const char* const version_string = TOSTR(MAJOR)"."TOSTR(MINOR)"."TOSTR(PATCH)"(" \
		__DATE__", "__TIME__")";

#define APP_VERSION_STR version_string

#endif // VERSION_H


