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

#if CONFIG_QT4
#define FLAG Qt::WindowStaysOnTopHint
#else
#define FLAG Qt::WStyle_StaysOnTop
//using X::QApplication;
#endif

Qop::Qop()
	:archive(0),parser(0),process(0)
  #ifndef EZPROGRESS
	  ,progress(new EZ_ProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,"UTIL_ProgressDialog",false,FLAG))
  #else
	  ,progress(new EZProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,FLAG))
  #endif //EZPROGRESS
  ,steps(-1),parser_type("tar"),internal(false)
{
    initGui();
}

Qop::~Qop()
{
	delete progress;
	if(archive) delete archive;
	if(process) delete process;
	if(parser)  delete parser;
}

void Qop::extract(const QString& arc, const QString& outDir)
{
	setInternal(true);
	setArchive(arc);
	initArchive();
	archive->extract();
}

void Qop::execute(const QString &cmd)
{
	initParser();
	initProcess();
	//progress->setLabelText(QDir::currentPath());//QApplication::applicationDirPath();
	process->setWorkingDirectory(QDir::currentPath());
	process->start(cmd);
	while(!process->waitForFinished(500)) {
		if(process->state()==QProcess::NotRunning)
			qDebug("Not running!");
		qApp->processEvents();
	}
	if(process->exitCode()!=0) {
		qDebug("Run error!");
	} else if(process->exitStatus()==QProcess::CrashExit) {
		qDebug("Crashed exit!");
	} else {
		qDebug("Normal exit");
	}

}

void Qop::initGui()
{
    progress->addButton(QObject::tr("Hide"),0,1,Qt::AlignRight);
    QObject::connect(progress->button(0),SIGNAL(clicked()),progress,SLOT(hide())); //Hide the widget will be faster. not showMinimum
    progress->setAutoClose(false);
    progress->setAutoReset(false);  //true: 最大值时变为最小

#if CONFIG_EZX
    QFont f;
    f.setPointSize(14);
    progress->setLabelFont(0,f);
    progress->bar()->setCenterIndicator(true);
    progress->bar()->setIndicatorFollowsStyle(false);
#endif //CONFIG_EZX
#if CONFIG_QT4
    progress->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
    progress->setObjectName("QProgressDialog");
#else
    progress->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
#endif //CONFIG_QT4
}

void Qop::initArchive()
{
    if(archive!=0) {
		delete archive;
		archive=0;
    }
    archive=new Archive::Tar::QTar(arc_path);
	ezDebug("archive: "+arc_path);
    progress->setMaximum(archive->unpackedSize());
    progress->addButton(QObject::tr("Pause"),1);
    QObject::connect(progress->button(1),SIGNAL(clicked()),archive,SLOT(pauseOrContinue()));
    QObject::connect(archive,SIGNAL(byteProcessed(int)),progress,SLOT(setValue(int)));
    QObject::connect(archive,SIGNAL(textChanged(const QString&)),progress,SLOT(setLabelText(const QString&)));
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
	    Archive::ArcReader ar(arc_path);
	    switch(ar.formatByBuf()) {
	    case Archive::FormatRar:	parser_type="unrar";	break;
	    case Archive::FormatZip:	parser_type="unzip";	break;
		case Archive::Format7zip:	parser_type="7z";		break;
		default:					parser_type="untar";	break;
	    }
		steps=ar.uncompressedSize();
    }//omit -T

    parser=getParser(parser_type);
#if CONFIG_QT4
	//progress->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//progress->setObjectName("QProgressDialog");
	QObject::connect(parser,SIGNAL(valueChanged(int)),progress,SLOT(setValue(int)));
	//QObject::connect(progress,SIGNAL(canceled()),qApp,SLOT(quit())); //does not work. Will send sig aboutToQuit()
	QObject::connect(progress,SIGNAL(canceled()),parser,SLOT(terminate()));
#else
	//progress->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//a.setMainWidget(progress);
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

	if(steps>0)
		parser->setTotalSize(steps);

}

void Qop::initProcess()
{
	process=new QProcess(this);
	process->setWorkingDirectory(QDir::currentPath());
	//ZDEBUG("working dir: %s",process->workingDirectory().toLocal8Bit().constData());
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(readStdOut()));
	connect(process,SIGNAL(readyReadStandardError()),SLOT(readStdErr()));
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), SIGNAL(finished(int,QProcess::ExitStatus)));
	connect(process,SIGNAL(started()),parser,SLOT(initTimer()));
}

void Qop::setInternal(bool bi)
{
	internal=bi;
}

void Qop::setArchive(const QString& archive_path)
{
	arc_path=archive_path;
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
    while(process->canReadLine()) {
		const char* line = process->readLine().constData();
		parser->parseLine(line);
		qDebug("stdout: %s",line);

    }
}

void Qop::readStdErr()
{
	const char* all_error_msg = process->readAllStandardError().constData();
	qDebug("stderr: %s",all_error_msg);
	qDebug("Read form stderr End...");
}
