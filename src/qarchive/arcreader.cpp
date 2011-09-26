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
#include "../qarchive/arcreader.h"
#include "utils/util.h"
#include "qtcompat.h"
#include <stdio.h>
#include <qfile.h>

namespace Archive {

static bool checkData(const Magic& magic,const unsigned char* data)
{
	data+=magic.offset;
	//char *data_tmp=new char[strlen((char*)magic.data)+1];
	//memcpy(data_tmp,magic.data,strlen((char*)magic.data));
	//!= or == ?
#if ( __BYTE_ORDER != __LITTLE_ENDIAN)
#pragma message("little endian")
	reverse<char>((char*)magic.data,strlen((char*)magic.data));
#endif
	//printf("size: %d\n",magic.size());
	bool eq=strncmp((const char*)magic.data,(const char*)data,magic.size())==0;
#if ( __BYTE_ORDER != __LITTLE_ENDIAN)
	reverse<char>((char*)magic.data,strlen((char*)magic.data));
#endif
	return eq;
}


#if defined(ARCREADER_STL)
ArcReader::ArcReader(const std::string& file)
	:fileName(file),format(FormatUnknow)
{
	format_map.insert(std::pair<std::string,Format>(".tar",FormatTar));
	format_map.insert(std::pair<std::string,Format>(".tgz",FormatGzip));
	format_map.insert(std::pair<std::string,Format>(".gz",FormatGzip));
	format_map.insert(std::pair<std::string,Format>(".bz2",FormatBzip2));
	format_map.insert(std::pair<std::string,Format>(".zip",FormatZip));
	format_map.insert(std::pair<std::string,Format>(".rar",FormatRar));
	format_map.insert(std::pair<std::string,Format>(".xz",FormatXz));
	format_map.insert(std::pair<std::string,Format>(".7z",Format7zip));
	format_map.insert(std::pair<std::string,Format>(".lz",FormatLzip));
	format_map.insert(std::pair<std::string,Format>(".tlz",FormatLzip));
	format_map.insert(std::pair<std::string,Format>(".lzo",FormatLzop));
	format_map.insert(std::pair<std::string,Format>(".tlzo",FormatLzop));
	format_map.insert(std::pair<std::string,Format>(".lzma",FormatLzop));
	format_map.insert(std::pair<std::string,Format>(".tlzma",FormatLzop));
	format_map.insert(std::pair<std::string,Format>(".lha",FormatLha));
	format_map.insert(std::pair<std::string,Format>("",FormatUnknow));
}

ArcReader::~ArcReader()
{
}

void ArcReader::setFile(const std::string &file)
{
	fileName=file;
}

Format ArcReader::formatByMagic()
{
	format=FormatUnknow;
	FILE *file=NULL;
	if((file=fopen(fileName.c_str(),"r"))==NULL)
		return format;

	unsigned char flag[7];
	if(fread(flag,sizeof(unsigned char),7,file)!=sizeof(unsigned char)*7) return format;

	printf("header: %s\n",flag);
	for(int i=0;magics[i].format!=FormatUnknow;++i) {
		if(checkData(magics[i],flag)) {
			printf("%s archive\n",magics[i].name);
			fflush(stdout);
			format=magics[i].format;
			break;
		}
	}
	fclose(file);
	return format;
}

Format ArcReader::formatByName()
{
	size_t pos=fileName.find_last_of(".");
	std::string ext=fileName.substr(pos);
	format=format_map.find(ext)->second;
	return format;
}


size_t ArcReader::uncompressedSize()
{
	//if(formatByName()==Unknow)
		formatByMagic();

	FILE *file=NULL;
	size_t unx_size=0;
	if((file=fopen(fileName.c_str(),"r"))==NULL)
		goto error_exit;

	if(format==FormatGzip) {
		if(fseek(file,-4,SEEK_END)!=0)
			goto error_exit;
	} else if(format==FormatZip) {
		if(fseek(file,22,SEEK_SET)!=0)
			goto error_exit;
		//char bt[4];
		//fread(bt,1,4,file);
		//printf("Uncompressed zip=%s\n",bt);
		//fseek(file,22,SEEK_SET);
	} else if(format==FormatRar) {
		if(fseek(file,sizeof(char*)+sizeof(unsigned short)+sizeof(unsigned long),SEEK_SET)!=0)
			goto error_exit;
	}

	fread(&unx_size,sizeof(size_t),1,file);
	swap_endian((char*)&unx_size,sizeof(size_t));


	printf("Uncompressed size=%d\n",unx_size);
	fflush(stdout);
	error_exit:
	if(file)
		fclose(file);
	return unx_size;

}

#else

QArcReader::QArcReader(const QString& file)
	:fileName(file),format(FormatUnknow)
{
//SymToStr(s) #s
	//for in Format ..insert(SymToStr(s),s)
	format_map.insert(".tar",FormatTar);
	format_map.insert(".tgz",FormatGzip);
	format_map.insert(".gz",FormatGzip);
	format_map.insert(".bz2",FormatBzip2);
	format_map.insert(".zip",FormatZip);
	format_map.insert(".rar",FormatRar);
	format_map.insert(".xz",FormatXz);
	format_map.insert(".7z",Format7zip);
	format_map.insert(".lz",FormatLzip);
	format_map.insert(".tlz",FormatLzip);
	format_map.insert(".lzo",FormatLzop);
	format_map.insert(".tlzo",FormatLzop);
	format_map.insert(".lzma",FormatLzop);
	format_map.insert(".tlzma",FormatLzop);
	format_map.insert(".lzh",FormatLha);
	format_map.insert("",FormatUnknow);
}

QArcReader::~QArcReader()
{}

void QArcReader::setFile(const QString &file)
{
	fileName=file;
}

Format QArcReader::formatByMagic()
{
	format=FormatUnknow;
	QFile file(fileName);
#if ARCREADER_QT4
	if(!file.open(QIODevice::ReadOnly)) {
		file.error();
#else
	if(file.open(IO_ReadOnly)) {
		qDebug("open error");
#endif//ARCREADER_QT4
		return format;
	}

	unsigned char flag[7];
#if ARCREADER_QT4
	file.read((char*)flag,7);
#else
	file.readBlock((char*)flag,7);
#endif //ARCREADER_QT4
	printf("header: %s\n",flag);
	for(int i=0;magics[i].format!=FormatUnknow;++i) {
		if(checkData(magics[i],flag)) {
			printf("%s archive, %d\n",magics[i].name,i);
			fflush(stdout);
			format=magics[i].format;
			break;
		}
	}
	file.close();;
	return format;
}

Format QArcReader::formatByName()
{
	int pos=
#if ARCREADER_QT4
	fileName.lastIndexOf(".");
#else
	fileName.findRev(".");
#endif //size_t pos
	format=format_map[fileName.mid(pos)];
	return format;
}

size_t QArcReader::uncompressedSize()
{
	formatByMagic();
	QFile file(fileName);
	size_t unx_size=file.size();
#if ARCREADER_QT4
	if(!file.open(QIODevice::ReadOnly)) {
		file.error();
#else
	if(file.open(IO_ReadOnly)) {
		qDebug("open error");
#endif//ARCREADER_QT4
		return unx_size;
	}

	uint pos=0;
	if(format==FormatGzip) pos=file.size()-4;
	else if(format==FormatZip) pos=22;
	else if(format==FormatRar) {
		//printf("ch* %d us %d ul %d ",sizeof(char*),sizeof(unsigned short),sizeof(unsigned long));
		pos=sizeof(char*)+sizeof(unsigned short)+sizeof(unsigned long);
	} else if(format==FormatLha) pos=11;

#if ARCREADER_QT4
	if(!file.seek(pos)) {
#else
	if(!file.at(pos)) {
#endif //ARCREADER_QT4
		file.close();
		return unx_size;
	}

#if ARCREADER_QT4
	file.read((char*)&unx_size,4);
#else
	file.readBlock((char*)&unx_size,4);
#endif //ARCREADER_QT4
	file.close();
#if BYTE_ORDER == LITTLE_ENDIAN
	if(format==FormatZip) reverse<char>(reinterpret_cast<char*>(&unx_size),4);//swap_endian(reinterpret_cast<char*>(&unx_size),4);//
#endif
	ZDEBUG("Uncompressed size=%d",unx_size);

	return unx_size;
}

#endif //ARCREADER_STL

}
