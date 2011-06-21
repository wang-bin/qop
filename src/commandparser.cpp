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
#include "qcounterthread.h"
#include "util.h"

//skip whitespace?
#if !CONFIG_QT4
#define indexOf find
#define lastIndexOf findRev
#endif


CommandParser::CommandParser()
	:counter(new QCounterThread),_cmd(""),_archive(""),_files(QStringList()),_compress_mode(false),_count_type(Size),cmd_parser(0)
{
#if COUNTER_THREAD
	//counter(new QCounterThread);
#endif
}

CommandParser::CommandParser(const QString &cmd)
	:counter(new QCounterThread),_archive(""),_files(QStringList()),_compress_mode(false),_count_type(Size),cmd_parser(0)
{
#if COUNTER_THREAD
	//counter(new QCounterThread);
#endif
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
	if(counter) {
		delete counter;
		counter=0;
	}
}

void CommandParser::setCommand(const QString &cmd)
{
	_cmd=cmd;
	if(cmd_parser) {
		delete cmd_parser;
		cmd_parser=0;
	}
	if(_cmd.startsWith("tar")) {
		cmd_parser=new TarCommandParser();
	} else if(_cmd.startsWith("zip")) {
		cmd_parser=new ZipCommandParser;
	} else if(_cmd.startsWith("unzip")) {
		cmd_parser=new UnzipCommandParser;
	}
	if(cmd_parser) {
		//cmd_parser->_cmd=_cmd;
		ZDEBUG("command: %s",qPrintable(_cmd));
		cmd_parser->setCommand(_cmd);
	}
}

QStringList CommandParser::files()
{
	if(cmd_parser)
		_files=cmd_parser->files();
	return _files;
}

CommandParser::CountType CommandParser::countType()
{
	if(cmd_parser)
		_count_type=cmd_parser->countType();
	return _count_type;
}

QString CommandParser::archive()
{
	if(cmd_parser)
		_archive=cmd_parser->archive();
	return _archive;
}

bool CommandParser::isCompressMode()
{
	if(cmd_parser)
		_compress_mode=cmd_parser->isCompressMode();
	return _compress_mode;
}

QString CommandParser::program() const
{
	return _cmd.left(_cmd.indexOf(" "));
}

size_t CommandParser::filesCount() const
{
	if(cmd_parser) {
//		counter=new QCounterThread(cmd_parser->files());
		return counter->numOfFiles(cmd_parser->files()); //zip no dir, virtual
	}
	return 0;
}

size_t CommandParser::filesSize() const
{
	if(cmd_parser) {
	//	counter=new QCounterThread(cmd_parser->files());
		return counter->sizeOfFiles(cmd_parser->files()); //zip no dir, virtual
	}
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
	uint unx_size=Archive::ArcReader(qPrintable(cmd_parser->archive())).uncompressedSize();
	ZDEBUG("Archive unpacked size: %db == %s",unx_size,qPrintable(size2str(unx_size)));
	return unx_size;
}


/*!
  Tar
*/
void TarCommandParser::init()
{
	int index_start=_cmd.indexOf(".tar "); //"tar "
	if(index_start<0) index_start=_cmd.indexOf(".tgz ");
	if(index_start<0) index_start=_cmd.indexOf(".tbz2 ");
	if(index_start<0) index_start=_cmd.indexOf(".tlz ");
	if(index_start<0) index_start=_cmd.indexOf(".tlzma ");
	if(index_start<0) index_start=_cmd.indexOf(".tlzo ");
	if(index_start<0) index_start=_cmd.indexOf(".txz ");

	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ZDEBUG("Archive is: %s",qPrintable(_archive));

	int idx_archive=_cmd.indexOf(_archive);
	//files();
	if(!_archive.isEmpty()) {

		int idx=_cmd.indexOf(" ",idx_archive);
		QString fs=_cmd.mid(idx);
		fs=fs.mid(0,fs.indexOf("-C")); //??? What i was thinking about? -C change dir

#if CONFIG_QT4
		_files=fs.split(" ");
#else
		_files=QStringList::split(" ",fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num=_cmd.mid(0,idx_archive).count("v");
#else
	int v_num=_cmd.mid(0,idx_archive).contains("v");
#endif
	if(v_num==1) {
		_count_type=Num;
		counter->setCountType(QCounterThread::Num);
	}
	else if(v_num==2) {
		_count_type=Size;
		counter->setCountType(QCounterThread::Size);
	}

// compress mode: c true x false
	//ezDebug(_cmd.mid(0,idx_archive));
	if(_cmd.mid(0,idx_archive).contains("c")) {

		_compress_mode=true;
	}
	ZDEBUG("Compress Mode: %d",_compress_mode);

}


/*!
  Zip
*/

void ZipCommandParser::init()
{
	int index_start=_cmd.indexOf(".zip ");

	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ZDEBUG("Archive is: %s",qPrintable(_archive));

	int idx_archive=_cmd.indexOf(_archive);
	//files();
	if(!_archive.isEmpty()) {

		int idx=_cmd.indexOf(" ",idx_archive);
		QString fs=_cmd.mid(idx);
		fs=fs.mid(0,fs.indexOf("-C")); //-C change dir

#if CONFIG_QT4
		_files=fs.split(" ");
#else
		_files=QStringList::split(" ",fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num=_cmd.mid(0,idx_archive).count("v");
#else
	int v_num=_cmd.mid(0,idx_archive).contains("v");
#endif
	if(v_num==0) {
		_count_type=Num;
		counter->setCountType(QCounterThread::Num);
	} else {
		_count_type=Size;
		counter->setCountType(QCounterThread::Size);
	}
	_compress_mode=true;
	ZDEBUG("Compress Mode: %d",_compress_mode);

}

/*!
  Unzip: unzip -ov file.zip
*/

void UnzipCommandParser::init()
{
	//lastIndexOf(" ");
	int index_start=_cmd.indexOf(".zip ");

	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ZDEBUG("Archive is: %s",qPrintable(_archive));

	int idx_archive=_cmd.indexOf(_archive);
	//files();
	if(!_archive.isEmpty()) {

		int idx=_cmd.indexOf(" ",idx_archive);
		QString fs=_cmd.mid(idx);
		//-x xlist(inside zip file) -d exdir
#if CONFIG_QT4
		_files=fs.split(" ");
#else
		_files=QStringList::split(" ",fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num=_cmd.mid(0,idx_archive).count("v");
#else
	int v_num=_cmd.mid(0,idx_archive).contains("v");
#endif
	if(v_num==0) {
		_count_type=Num;
		counter->setCountType(QCounterThread::Num);
	} else {
		_count_type=Size;
		counter->setCountType(QCounterThread::Size);
	}
	_compress_mode=false;
	ZDEBUG("Compress Mode: %d",_compress_mode);
}

/*!
  Unrar: unrar x -o+ -p- -y rar.rar destdir
  //-o- not overwrite, -or rename auto, -p[pw]
*/

void UnrarCommandParser::init()
{
	int index_start=_cmd.indexOf(".rar ");

	index_start=_cmd.lastIndexOf(" ",index_start)+1;
	int index_end=_cmd.indexOf(" ",index_start);

	_archive=_cmd.mid(index_start,index_end-index_start);
	ZDEBUG("Archive is: %s",qPrintable(_archive));

	_count_type=Num;
	counter->setCountType(QCounterThread::Num);

	_compress_mode=false;
	ZDEBUG("Compress Mode: %d",_compress_mode);
}
