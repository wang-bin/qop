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

#include "strutil.h"


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

