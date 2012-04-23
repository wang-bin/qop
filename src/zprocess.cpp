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


#include <qfeatures.h>

#ifndef QT_NO_PROCESS
#include "zprocess_p.h"
#include "zprocess.h"

ZProcessPrivate::ZProcessPrivate()
	:options(pack_options)
{
	pack_options.insert(ZProcess::Tar, "tar cvvf %out_pat% %in%");
	pack_options.insert(ZProcess::TGz, "tar zcvvf  %out_pat% %in%");

}

ZProcess::ZProcess(QObject *parent)
	:QProcess(parent),d_ptr(new ZProcessPrivate)
{
}

bool ZProcess::setArchiveTool(ArchiveTool tool)
{
	if (!toolAvailable(tool))
		return false;
	archive_tool = tool;
	return true;
}

void ZProcess::setFiles(const QStringList &files)
{
	Q_D(ZProcess);
	d->filelist = files;
}

void ZProcess::setOutputPath(const QString &path)
{
	Q_D(ZProcess);
	d->out_path = path;
}

void ZProcess::setPassword(const QString &pwd)
{
	Q_D(ZProcess);
	d->password = pwd;
}

void ZProcess::setLevel(int level)
{
	Q_D(ZProcess);
	d->level = level;
}


void ZProcess::pack()
{
	//replace %keywords%, split to stringlist, startDetached()
}

void ZProcess::unpack()
{

}

//Process control
void ZProcess::resume()
{

}

void ZProcess::pause()
{

}



bool ZProcess::toolAvailable(ArchiveTool tool) const
{
	//PATH
	return true;
}

#endif //QT_NO_PROCESS
