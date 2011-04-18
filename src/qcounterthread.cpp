/******************************************************************************
	QCounterThread: counting thread. It's a part of QOP.
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
#include "qcounterthread.h"
#include <qdir.h>

QCounterThread::QCounterThread(const QStringList &f)
	:files(f),size(0),num(0),ct(Size)
{}

QCounterThread::~QCounterThread() {}

#ifndef QT_THREAD_SUPPORT
void QCounterThread::start() { run();}
#endif //QT_THREAD_SUPPORT
void QCounterThread::setFiles(const QStringList &f) { files=f;}

void QCounterThread::setCountType(CountType t) { ct=t;}

void QCounterThread::run()
{
	size=0, num=0;
	if(ct==Size) sizeOfFiles(files);
	else if(ct==Num) numOfFiles(files);
	else numOfFilesNoDir(files);
	emit done();
}

uint QCounterThread::numOfFilesNoDir(const QStringList& list)
{
	QString name;
	for(QStringList::ConstIterator it = list.begin();it != list.end(); ++it) {
		name=*it;
		if ((*it != ".") && (*it != "..")) {
			if(QFileInfo(name).isDir()) {
				QStringList pathList;
				QStringList nameList=QDir(name).entryList(QDir::Files|QDir::Dirs|QDir::NoSymLinks|QDir::Hidden|QDir::Readable);
				for(QStringList::ConstIterator it = nameList.begin();it != nameList.end(); ++it)
					if ((*it != ".") && (*it != ".."))	pathList.append(name+"/"+*it);   //can't be root,so override
						numOfFilesNoDir(pathList);
			} else ++num;
		}
	} emit counted(num); emit maxChanged(num);
	return num;
}

uint QCounterThread::numOfFiles(const QStringList& list)
{
	QString name;
	for(QStringList::ConstIterator it = list.begin();it != list.end(); ++it) {
		name=*it;
		if ((*it != ".") && (*it != "..")) {
			if(QFileInfo(name).isDir()) {
				QStringList pathList;
				QStringList nameList=QDir(name).entryList(QDir::Files|QDir::Dirs|QDir::NoSymLinks|QDir::Hidden|QDir::Readable);
				for(QStringList::ConstIterator it = nameList.begin();it != nameList.end(); ++it)
					if ((*it != ".") && (*it != ".."))	pathList.append(name+"/"+*it);   //can't be root,so override
						numOfFiles(pathList);
			} ++num;
		}
	} emit counted(num); emit maxChanged(num);
	return num;
}

uint QCounterThread::sizeOfFiles(const QStringList& list)
{
	QString name;
	for(QStringList::ConstIterator it = list.begin();it != list.end(); ++it) {
		name=*it;
		if ((*it != ".") && (*it != "..")) {
			if(QFileInfo(name).isDir()) {
				QStringList pathList;
				//filter is important. if list symlinks, the target size will be added
				QStringList nameList=QDir(name).entryList(QDir::Files|QDir::Dirs|QDir::NoSymLinks|QDir::Hidden|QDir::Readable);
				for(QStringList::ConstIterator it = nameList.begin();it != nameList.end(); ++it)
					if ((*it != ".") && (*it != ".."))	pathList.append(name+"/"+*it);   //can't be root,so override
						sizeOfFiles(pathList);
			} else {
				size+=QFileInfo(*it).size();  //QFileInfo()::isSymLink()
				//emit counted(size); //too frequent. app will abort in windows
			}
		}
	} emit counted(size); emit maxChanged(num);
	return size;
}


