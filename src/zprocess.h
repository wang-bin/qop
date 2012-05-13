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

#include "config.h"
#include <qfeatures.h>
#ifndef QT_NO_PROCESS
#include <qprocess.h>
/*!
 * \brief The ZProcess class
 * Used by Qop.
 */

/*
	remove Qop::readStdOut(), Qop::readStdErr(), Qop::initProcess() and implement Qop::execute()
	with ZProcess.
*/
class ZProcessPrivate;
class Q_EXPORT ZProcess : public QProcess
{
	Q_OBJECT //move to another class? moc sucks
public:
	enum ArchiveTool {
        Tar = 0x1, Gzip = 0x2, Bzip2 = 0x4, SevenZip = 0x6, Xz = 0x8
        , Lzma = 0x10, Lzip = 0x12, Lzop = 0x14
        , TGz = Tar | Gzip, TBz2 = Tar | Bzip2, T7z = Tar | SevenZip, TXz = Tar | Xz
        , TLzma = Tar | Lzma, TLzo = Tar | Lzop, TLz = Tar | Lzip
        , Zip, Unzip, Rar, Unrar, Upx
	};

	explicit ZProcess(QObject *parent = 0);
	//void setCommand(const QString& cmd);
	/*
		If not setted, guess among availabeArchiveTools() before pack() or unpack().
		Return wether the tool is available.
	*/
	bool setArchiveTool(ArchiveTool tool);
	//compress into one or many(gz, lz), or extract some
	void setFiles(const QStringList& files);
	//can be dir
	void setOutputPath(const QString& path);
	void setPassword(const QString& pwd);
	void setLevel(int level);
    //void addOptions(const QStringList& opts);
    //void setOptions(const QStringList& opts);
    //QStringList options() const;

    ArchiveTool archiveTool() const;

	void pack();
	void unpack();

    QString packCommand() const;
    QString unpackCommand() const;

signals:
	
public slots:
	void resume(); //connected to the progress
	void pause(); //connecte to the progress
	
private:
	bool toolAvailable(ArchiveTool tool) const;

    Q_DECLARE_PRIVATE(ZProcess)
    ZProcessPrivate * d_ptr;

	ArchiveTool archive_tool;
};

#endif //QT_NO_PROCESS
#endif // ZPROCESS_H
