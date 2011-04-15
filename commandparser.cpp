/******************************************************************************
	QOP: Qt Output Parser for tar, zip etc with a compression/extraction progress indicator
	Copyright (C) 2011 Wangbin <wbsecg1@gmail.com>
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

#include "commandparser.h"
#include <qfileinfo.h>

#include "qarchive/arcreader.h"
#include "global.h"
#include "util.h"

CommandParser::CommandParser()
	:_cmd(""),_archive(""),_files(QStringList()),cmd_parser(0)
{
}

CommandParser::CommandParser(const QString &cmd)
	:_archive(""),_files(QStringList()),cmd_parser(0)
{
	setCommand(cmd);
}
/*
CommandParser::CommandParser(const QString &program, const QStringList &argv)
{
	_cmd=program;
}
*/
CommandParser::~CommandParser()
{
	if(cmd_parser) {
		delete cmd_parser;
		cmd_parser=0;
	}
}

void CommandParser::setCommand(const QString &cmd)
{
	_cmd=cmd;
	if(cmd_parser) {
		delete cmd_parser;
		cmd_parser=0;
	}
	if(_cmd.startsWith("tar"))
		cmd_parser=new TarCommandParser();

	if(cmd_parser) {
		cmd_parser->_cmd=_cmd;
		files();
		archive();
	}
}

QStringList CommandParser::files()
{
	if(cmd_parser)
		_files=cmd_parser->files();
	return _files;
}

QString CommandParser::archive()
{
	if(cmd_parser)
		_archive=cmd_parser->archive();
	return _archive;
}

size_t CommandParser::filesCount() const
{
	if(cmd_parser)
		return cmd_parser->filesCount();
	return 0;
}

size_t CommandParser::filesSize() const
{
	if(cmd_parser)
		return cmd_parser->filesSize();
	return 0;
}

size_t CommandParser::archiveSize() const
{
	if(cmd_parser) return QFileInfo(cmd_parser->archive()).size();
	return 0;
}

size_t CommandParser::archiveUnpackSize() const
{
	if(cmd_parser==0) return 0;
	uint unx_size=Archive::QArcReader(cmd_parser->archive()).uncompressedSize();
	qDebug("Archive unpacked size: %db == %s",unx_size,UTIL::qstr2cstr(size2Str<double>(unx_size)));
	return unx_size;
}



TarCommandParser::TarCommandParser()
	:CommandParser()
{
}

TarCommandParser::TarCommandParser(const QString &cmd)
	:CommandParser(cmd)
{
	files();
	archive();
}

TarCommandParser::~TarCommandParser()
{}

QStringList TarCommandParser::files()
{
	return QStringList();
}

//skip whitespace?
#if !CONFIG_QT4
#define indexOf find
#define lastIndexOf findRev
#endif
QString TarCommandParser::archive()
{
	int index_start=_cmd.indexOf(".tar"); //"tar "
	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ezDebug("Archive is: "+_archive);
	return _archive;
}
