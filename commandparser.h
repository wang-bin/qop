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

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <qstringlist.h>
class CommandParser
{
public:
    CommandParser();
	CommandParser(const QString& cmd);
	//CommandParser(const QString& program, const QStringList& argv);

	virtual ~CommandParser();

	void setCommand(const QString& cmd);

	virtual QStringList files()=0;
	virtual QString archive()=0;

	/*!
		CommandParser(cmd).unpackSize();
	*/
	size_t filesCount() const;
	size_t filesSize() const;
	size_t archiveSize() const;
	size_t archiveUnpackSize() const;

protected:
	QString _cmd;
	QString _archive;
	QStringList _files;

};

class TarCommandParser : public CommandParser
{
public:
	TarCommandParser(const QString& cmd);
	//TarCommandParser(const QString& program, const QStringList& argv);

	virtual ~TarCommandParser();

	virtual QStringList files();
	virtual QString archive();
};

#endif // COMMANDPARSER_H
