/******************************************************************************
	zprocess_p.h: description
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


#ifndef ZPROCESS_P_H
#define ZPROCESS_P_H

#include <qfeatures.h>

#ifndef QT_NO_PROCESS
#include "qtcompat.h"
#include <qmap.h>

class ZProcessPrivate;

class EZProgressDialog;
class ZProcess;
class ZProcessPrivate
{
	//Q_DECLARE_PUBLIC(ZProcess)
public:
	ZProcessPrivate();
	char level;
	EZProgressDialog *progress;
	//ProcessControl *control;
	QString password, out_path;
	QStringList filelist;
	typedef QMap<int, QString> ToolOptionMap;
	ToolOptionMap pack_options, unpack_options;
	ToolOptionMap &options; //to pack or unpack. split
	//ZProcess *q_ptr;
};

#endif //QT_NO_PROCESS
#endif // ZPROCESS_P_H
