/******************************************************************************
	ZProcess: archive process with gui progress
	Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
	
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


#ifndef ZPROCESS_H
#define ZPROCESS_H

#include <qprocess.h>
#include "gui/ezprogressdialog.h"

/*!
 * \brief The ZProcess class
 * Used by Qop.
 */

/*
	remove Qop::readStdOut(), Qop::readStdErr(), Qop::initProcess() and implement Qop::execute()
	with ZProcess.
*/
class ZProcess : public QProcess
{
	Q_OBJECT
public:
	enum ArchiveTool {
		Zip, Unzip, Tar, Rar, Unrar, Gzip, Bzip2, SevenZip, Lzip, Upx
		, TGz = Tar | Gzip, TBz2 = Tar | Bzip2, T7z = Tar | SevenZip, TLz = Tar | Lzip
	};

	explicit ZProcess(QObject *parent = 0);
	/*
		If not setted, guess among availabeArchiveTools() before pack() or unpack().
		Return wether the tool is available.
	*/
	bool setArchiveTool(ArchiveTool tool);
	//compress into one or many(gz, lz), or extract some
	void setFiles(const QStringList& files);
	//can be dir
	void setOutputPath();
	void setPassword(const QString& pwd);
	void setLevel(int level);
	void addOptions(const QStringList& opts);
	void setOptions(const QStringList& opts);
	QStringList options() const;

	void pack();
	void unpack();
signals:
	
public slots:
	void resume(); //connected to the progress
	void pause(); //connecte to the progress
	
private:
	EZProgressDialog *progress;
	//ProcessControl *control;

	QString password, out_path;
	QStringList optionlist, filelist;
};

#endif // ZPROCESS_H
