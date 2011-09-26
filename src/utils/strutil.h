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

#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>
#include <cstring>

extern const char* reverse_string(const char *string);


inline bool str_starts_with(const std::string& str, const std::string& substr)
{
	size_t pos = str.find(substr);
	if (pos==std::string::npos || pos!=0)
		return false;
	return true;
}

inline bool str_ends_with(const std::string& str, const std::string& substr)
{
	size_t pos = str.find(substr);
	if (pos==std::string::npos)
		return false;
	return str.compare(pos, str.size()-pos, substr)==0;
}

inline bool cstr_starts_with(const char* str, const char* substr)
{
	const char * pos = strstr(str, substr);
	if (pos==NULL || pos!=str)
		return false;
	return true;
}

inline bool cstr_ends_with(const char* str, const char* substr)
{
	const char * pos = strstr(str, substr);
	if (pos==NULL)
		return false;
	return strcmp(pos, substr)==0;
}


#endif // STRUTIL_H
