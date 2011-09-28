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

#ifndef QOP_H
#define QOP_H

#include <qprocess.h>
#include "QOutParser.h"
#include "qarchive/arcreader.h"
#include "qarchive/qarchive.h"
#include "qarchive/tar/qtar.h"
#ifndef EZPROGRESS
#define EZPROGRESS
#endif
#ifdef EZPROGRESS
#include "gui/ezprogressdialog.h"
#endif

class Qop :public QObject
{
	Q_OBJECT
public:
	Qop();
	~Qop();

	void setUpdateAllMessage(bool all);
	void extract(const QString& archive,const QString& outDir);
	void execute(const QString& cmd); //execute(const QString& program,const QStringList& arg)
	//void parseOutput();
	//void pipeView();

	void setInternal(bool);
	void setInterval(unsigned int interval);
	void setArchive(const QString& archive_path);

	void setTimeFormat(const QString& format);
//union!!
/*!
	for build-in method
*/
	Archive::QArchive* archive;

/*!
	for tools in PATH with qop executable. In Qt2.x, copy QProcess
*/
	QOutParser *parser;
#ifndef QT_NO_PROCESS
	QProcess *process;
#endif //QT_NO_PROCESS
/*!
	common gui for all
*/
#ifndef EZPROGRESS
	UTIL_ProgressDialog *progress;
#else
	EZProgressDialog *progress;
#endif //EZPROGRESS
	int steps;
	const char* parser_type;

	void initArchive();
	void initParser();
	void initProcess();

private slots:
	void readStdOut();
	void readStdErr();

private:
	void initGui();

	bool all_msg;
	bool internal;
	unsigned int interval;
	QString arc_path;
};

#endif // QOP_H
