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
{
}

CommandParser::CommandParser(const QString &cmd)
	:_cmd(cmd)
{

}
/*
CommandParser::CommandParser(const QString &program, const QStringList &argv)
{
	_cmd=program;
}
*/
CommandParser::~CommandParser()
{}

QStringList CommandParser::files()
{
	return _files;
}

QString CommandParser::archive()
{
	return _archive;
}

size_t CommandParser::filesCount() const
{
	return 0;
}

size_t CommandParser::filesSize() const
{
	return 0;
}

size_t CommandParser::archiveSize() const
{
	return QFileInfo(_archive).size();
}

size_t CommandParser::archiveUnpackSize() const
{
	uint unx_size=Archive::QArcReader(_archive).uncompressedSize();
	qDebug("Archive unpacked size: %db == %s",unx_size,size2Str<double>(unx_size).toLocal8Bit().constData());
	return unx_size;
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

}

//skip whitespace?
QString TarCommandParser::archive()
{
	int index_start=_cmd.indexOf(".tar"); //"tar "
	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ezDebug("Archive is: "+_archive);

	return _archive;
}
