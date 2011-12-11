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

#include "qop.h"
#include <qdir.h>

#include "commandparser.h"
#include "utils/convert.h"

#if CONFIG_QT4
#define FLAG Qt::WindowStaysOnTopHint
#else
#define FLAG Qt::WStyle_StaysOnTop
//using X::QApplication;
#endif

#if !USE_SLOT
static EZProgressDialog *progressDlg;
void DoProgress(const QString& _current_fileName, size_t current_size, size_t processed, size_t total_size, int speed, int time_elapsed, int time_remain)
{
	QString _out_msg=_current_fileName+"\n"+QObject::tr("Size: ")+size2Str<qreal>(current_size)+"\n"+QObject::tr("Processed: ")+size2Str<qreal>(processed)+" / "+size2Str<qreal>(total_size)+"\n";
	QString _extra_msg=QObject::tr("Speed: ")+size2Str<qreal>(speed)+"/s\n"+QObject::tr("Elapsed: %1s Remaining: %2s").arg(time_elapsed/1000.,0,'f',1).arg(time_remain,0,'f',1);
	progressDlg->setValue(processed);
	progressDlg->setLabelText(_out_msg+_extra_msg);
	//qApp->processEvents();
}

static IProgressHandler progressHandler = { &DoProgress };

#endif

// vv :archiveSize(). v: filesCount()
Qop::Qop()
	:archive(0),parser(0)
  #ifndef EZPROGRESS
	  ,progress(new EZ_ProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,"UTIL_ProgressDialog",false,FLAG))
  #else
	  ,progress(new EZProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,FLAG))
  #endif //EZPROGRESS
	,steps(-1),parser_type("tar"),all_msg(false),internal(false),interval(300)
{
#ifndef QT_NO_PROCESS
	process = 0;
#endif //QT_NO_PROCESS
	initGui();
#if !USE_SLOT
	progressDlg=progress;
#endif
}

Qop::~Qop()
{
	delete progress;
	progress = 0;
	if(archive) {
		delete archive;
		archive = 0;
	}
#ifndef QT_NO_PROCESS
	if(process) {
		delete process;
		process = 0;
	}
#endif //QT_NO_PROCESS
	if(parser) {
		delete parser;
		parser = 0;
	}
}


void Qop::setUpdateAllMessage(bool all)
{
	all_msg = all;
}

void Qop::extract(const QString& arc, const QString& outDir)
{
	setInternal(true);
	setArchive(arc);
	initArchive();
	archive->setInterval(interval);
	archive->setOutDir(outDir);
	archive->extract();
}

#if !CONFIG_QT4
#define currentPath() current().absPath()
#endif
void Qop::execute(const QString &cmd)
{
	CommandParser cmdParser(cmd);
	if(cmdParser.program()==QLatin1String("tar") && !cmdParser.isCompressMode())
		parser_type="untar";
	else if(cmdParser.program()==QLatin1String("zip"))
		parser_type="zip";
	else if(cmdParser.program()==QLatin1String("unzip"))
		parser_type="unzip";
	else if(cmdParser.program()==QLatin1String("unrar"))
		parser_type="unrar";

	initParser();
	initProcess();
	//progress->setLabelText(QDir::currentPath());//QApplication::applicationDirPath();
#ifndef QT_NO_PROCESS
	if(cmdParser.countType()==CommandParser::Num)
		parser->setCountType(QCounterThread::Num);
	else
		parser->setCountType(QCounterThread::Size);
#if COUNTER_THREAD
	if(cmdParser.isCompressMode()) {
		connect(cmdParser.counterThread(),SIGNAL(maximumChanged(int)),progress,SLOT(setMaximum(int)));
		connect(cmdParser.counterThread(),SIGNAL(maximumChanged(int)),parser,SLOT(setTotalSize(int)));
		cmdParser.counterThread()->start();
	} else {
		progress->setMaximum(cmdParser.archiveUnpackSize());
	}
#else
	if(!cmdParser.isCompressMode()) {
		progress->setMaximum(cmdParser.archiveUnpackSize());
	} else {
		if(cmdParser.countType()==CommandParser::Size)
			progress->setMaximum(cmdParser.filesSize());
		else
			progress->setMaximum(cmdParser.filesCount());
	}
#endif
	if(progress->maximum()) {
		parser->setTotalSize(progress->maximum());
	}
	process->setWorkingDirectory(QDir::currentPath());
	process->start(cmd);
	while(!process->waitForFinished(1000)) {
		if(process->state()==QProcess::Starting) qDebug("Starting process...Program has not yet been invoked");
		//if(process->state()==QProcess::NotRunning) qDebug("Not running!"); //true in linux, why?
		qApp->processEvents();
	}
	if(process->exitCode()!=0) {
		ZDEBUG("Run error!");
	} else if(process->exitStatus()==QProcess::CrashExit) {
		ZDEBUG("Crashed exit!");
	} else {
		ZDEBUG("Normal exit");
	}
#endif //QT_NO_PROCESS
}

void Qop::initGui()
{
	progress->addButton(QObject::tr("Hide"),0,1,Qt::AlignRight);
	QObject::connect(progress->button(0),SIGNAL(clicked()),progress,SLOT(hide())); //Hide the widget will be faster. not showMinimum
	progress->setAutoClose(false);
	progress->setAutoReset(false);
#if CONFIG_EZX
	QFont f;
	f.setPointSize(14);
	progress->setLabelFont(0,f);
	progress->bar()->setCenterIndicator(true);
	progress->bar()->setIndicatorFollowsStyle(false);
#endif //CONFIG_EZX
#if CONFIG_QT4
	progress->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
	progress->setObjectName("EZProgressDialog");
#else
	progress->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
	progress->setName("EZProgressDialog");
#endif //CONFIG_QT4
}

void Qop::initArchive()
{
	if(archive!=0) {
		delete archive;
		archive=0;
	}
	archive=new Archive::Tar::QTar(arc_path);
#if !USE_SLOT
	archive->setProgressHandler(&progressHandler);
#endif
	ZDEBUG("archive: %s",qPrintable(arc_path));
	progress->setMaximum(archive->unpackedSize());
	progress->addButton(QObject::tr("Pause"),1);
#if CONFIG_QT4
	progress->button(1)->setCheckable(true);
#else
	progress->button(1)->setToggleButton(true);
#endif
	QObject::connect(progress->button(1),SIGNAL(clicked()),archive,SLOT(pauseOrContinue()));
#if USE_SLOT
	QObject::connect(archive,SIGNAL(byteProcessed(int)),progress,SLOT(setValue(int)));
	QObject::connect(archive,SIGNAL(textChanged(const QString&)),progress,SLOT(setLabelText(const QString&)));
#endif
	QObject::connect(progress,SIGNAL(canceled()),archive,SLOT(terminate()));
	QObject::connect(archive,SIGNAL(finished()),progress,SLOT(showNormal()));

#if CONFIG_EZX
	//progress->exec(); //NO_MODAL
		qApp->processEvents();
//#else
	//progress->show();
#endif
}

void Qop::initParser()
{
	//char* parser_type="tar";
	if(!arc_path.isEmpty()) {
		if(!QFile(arc_path).exists()) {
			ezDebug("file: "+arc_path+ " does not exists\n");
			fflush(stdout);
		}
		Archive::ArcReader ar(qPrintable(arc_path));
		switch(ar.formatByMagic()) {
		case Archive::FormatRar:	parser_type="unrar";	break;
		case Archive::FormatZip:	parser_type="unzip";	break;
		case Archive::Format7zip:	parser_type="7z";		break;
		default:					parser_type="untar";	break;
		}
		steps=ar.uncompressedSize();
	}//omit -T

	parser=getParser(parser_type);
	parser->setInterval(interval);
	parser->setUpdateMsgOnChange(all_msg);
	if(!arc_path.isEmpty()) {
		parser->setRecount(false);
	}
	ZDEBUG("steps: %d",steps);
	if(steps>0) {
		progress->setMaximum(steps);
		parser->setTotalSize(steps);
	}
#if CONFIG_QT4
	QObject::connect(parser,SIGNAL(valueChanged(int)),progress,SLOT(setValue(int)));
	QObject::connect(progress,SIGNAL(canceled()),parser,SLOT(terminate()));
#else
	QObject::connect(parser,SIGNAL(valueChanged(int)),progress,SLOT(setProgress(int)));
	QObject::connect(progress,SIGNAL(cancelled()),parser,SLOT(terminate())); //to canceled
#endif //CONFIG_EZX
	QObject::connect(parser,SIGNAL(textChanged(const QString&)),progress,SLOT(setLabelText(const QString&)));
	QObject::connect(parser,SIGNAL(maximumChanged(int)),progress,SLOT(setMaximum(int)));
	QObject::connect(parser,SIGNAL(finished()),progress,SLOT(showNormal()));
#if CONFIG_EZX
	//progress->exec(); //NO_MODAL
	qApp->processEvents();
//#else
	//progress->show();
#endif

}

void Qop::initProcess()
{
#ifndef QT_NO_PROCESS
	process=new QProcess(this);
	process->setWorkingDirectory(QDir::currentPath());
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(readStdOut()));
	connect(process,SIGNAL(readyReadStandardError()),SLOT(readStdErr()));
	//connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), SIGNAL(finished(int,QProcess::ExitStatus)));
	connect(process,SIGNAL(started()),parser,SLOT(initTimer()));  //Maemo5 seems not work
	connect(process, SIGNAL(finished(int)), parser, SIGNAL(finished()));
	//connect(process, SIGNAL(finished(int)), parser, SLOT(slotFinished()));
	connect(progress, SIGNAL(canceled()), process, SLOT(kill()));
	connect(progress, SIGNAL(cancelled()), process, SLOT(kill()));
#endif //QT_NO_PROCESS
}

void Qop::setInternal(bool bi)
{
	internal=bi;
}

void Qop::setInterval(unsigned int interval)
{
	this->interval = interval;
}

void Qop::setArchive(const QString& archive_path)
{
	arc_path=archive_path;
}

void Qop::setTimeFormat(const QString &format)
{
#if CONFIG_QT4
	if (format.toUpper() == QLatin1String("UTC") || format.toUpper() == QLatin1String("ISO")) {
#else
	if (format.upper() == QLatin1String("UTC" || format.upper() == QLatin1String("ISO"))) {
#endif //CONFIG_QT4
		g_time_convert = msec2str;
	} else {
		g_time_convert = msec2secstr;
	}
}

/*!
  parse the last line, find the index in filelist;
  or read all then parse line by line
  or read line by line
 */
void Qop::readStdOut()
{/*
	char* line = process->readAllStandardOutput();
	parser->parseLine(line);
	QString line = data.constData();
	ZDEBUG("stdout: %s",line);
*/
#ifndef QT_NO_PROCESS
	while(process->canReadLine()) {
		parser->parseLine(process->readLine().constData());
		//qDebug("stdout: %s",line);
	}
#endif //QT_NO_PROCESS
}

void Qop::readStdErr()
{
#ifndef QT_NO_PROCESS
	const char* all_error_msg = process->readAllStandardError().constData();
	qDebug("Read from stderr: \n%s",all_error_msg);
	qDebug("Read form stderr End...");
#endif //QT_NO_PROCESS
}
