/******************************************************************************
	QOP: Qt Output Parser for tar, zip etc with a compression/extraction progress indicator
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
#include "QOutParser.h"
#include "qarchive/arcreader.h"
#include "option.h"
#include "util.h"
#include "qarchive/qarchive.h"
#include "qarchive/tar/qtar.h"
#ifndef EZPROGRESS
#define EZPROGRESS
#endif
#ifdef EZPROGRESS
#include "gui/ezprogressdialog.h"
#endif
#ifdef _OS_LINUX_
#include <sys/types.h>
#include <signal.h>
#endif
#include <qtimer.h>
#include <qlabel.h>
#ifndef NO_EZX
#include <ZApplication.h>
#include <ZLanguage.h>
#endif
//#include <qdir.h>
//#include <qfileinfo.h>
#include <qtranslator.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define VERSION "0.1.3"
static const char *appName=(char*)malloc(64);
//static char appName[64]={};
const QString program="qop";

void printHelp()
{
	fprintf(stderr,	"%s %s (%s, %s)\n"
					"Usage: %s [-t parserFor] [-n|s] [-hmc] [-x archieve|-T totalSteps] [files...]\n"
					"  -t, --parser[=TYPE]  parser(tar,untar,zip,unzip,unrar,lzip.upx)\n"
					"  -n, --number         count number of files as total steps\n"
					"  -s, --size           count size of files as total steps\n"
					"  -T, --steps=STEPS    specify total steps.\n"
					"  -h, --help           help. print me\n"
					"  -m, --multi-thread   multi-thread counting steps while (de)compressing\n"
					"  -c, --auto-close     auto close when finished\n"
					"  -x, --extract=FILE   indicates extracting progress\n"

					"\nCopyright (C) 2010 Wangbin(nukin CCMOVE, aka novesky in motorolafans)\n"
					"This program comes with ABSOLUTELY NO WARRANTY, to the extent permitted by low.\n"
					"This is free software, and you are welcome to redistribute it "
					"under the terms of the GNU General Public Licence version 2\n<http://www.gnu.org/licenses/gpl-2.0.html>.\n"
					"\n"
					"Project:\n"
					"    http://sourceforge.net/projects/qop/files\n"
					"    http://qt-apps.org/content/show.php/qop?content=132430\n"
					"Send bugreports to <wbsecg1@gmail.com>\n\n",appName,VERSION,__DATE__,__TIME__,appName);
}

int main(int argc, char *argv[])
{
	appName=getFileName(argv[0]);
	printf("%s %s (%s, %s)\n",appName,VERSION,__DATE__,__TIME__);
	opts_t options=opts_parse(argc,argv);

#ifdef EZXT_QT4
#define FLAG Qt::WindowStaysOnTopHint
#else
#define FLAG Qt::WStyle_StaysOnTop
//using X::QApplication;
#endif
	ZApplication a(argc, argv, QApplication::GuiClient);
	printf("Qt %s\n",qVersion());
#if QT_VERSION >= 0x040000
	QString dirname=QCoreApplication::applicationDirPath();
	qDebug()<<dirname;
#else
	QString dirname=QFileInfo(argv[0]).dirPath();
	//QString dirname=getFileDir(argv[0]);//
#endif
	//QDir::setCurrent(dirname); //bad in windows cygwin

	QTranslator appTranslator(0);
#ifndef NO_EZX
	appTranslator.load(program+"_"+ZLanguage::getSystemLanguageCode(),dirname+"/i18n");
	//QString(dirname)+"/i18n" will load fail, 乱码
#else
	QString sysLang=QLocale::system().name();
	qDebug()<<sysLang;
	appTranslator.load(program+"-"+sysLang,dirname+"/i18n");
#endif //NO_EZX
	a.installTranslator(&appTranslator);
#ifndef EZPROGRESS
	UTIL_ProgressDialog *msg=new EZ_ProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,options->steps,0,"UTIL_ProgressDialog",false,FLAG);
#else
	EZProgressDialog *msg=new EZProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,options->steps,0,FLAG);
#endif //EZPROGRESS
	msg->addButton(QObject::tr("Hide"),0,1,Qt::AlignRight);
	QObject::connect(msg->button(0),SIGNAL(clicked()),msg,SLOT(hide())); //Hide the widget will be faster. not showMinimum
	msg->setAutoClose(false);
	msg->setAutoReset(false);  //true: 最大值时变为最小
#ifndef NO_EZX
	QFont f;
	f.setPointSize(14);
	msg->setLabelFont(0,f);
	msg->bar()->setCenterIndicator(true);
	msg->bar()->setIndicatorFollowsStyle(false);
#endif //NO_EZX
#ifdef EZXT_QT4
	msg->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
	msg->setObjectName("QProgressDialog");
#else
	msg->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
	a.setMainWidget(msg);
#endif //EZXT_QT4
	if(!options->hide) msg->show();

	if(options->diy || argc<2) {
		Archive::QArchive* qarc=new Archive::Tar::QTar(options->x_file);
		printf("archive: %s\n",options->x_file);
		msg->setMaximum(qarc->unpackedSize());
		msg->addButton(QObject::tr("Pause"),1);
		QObject::connect(msg->button(1),SIGNAL(clicked()),qarc,SLOT(pauseOrContinue()));
		QObject::connect(qarc,SIGNAL(byteProcessed(int)),msg,SLOT(setValue(int)));
		QObject::connect(qarc,SIGNAL(textChanged(const QString&)),msg,SLOT(setLabelText(const QString&)));
		QObject::connect(msg,SIGNAL(canceled()),qarc,SLOT(terminate()));
		QObject::connect(qarc,SIGNAL(finished()),msg,SLOT(showNormal()));
#ifndef NO_EZX
	//msg->exec(); //NO_MODAL
		a.processEvents();
//#else
	//msg->show();
#endif
		qarc->extract();
	} else {
		if(options->x_file!=NULL) {
			if(!QFile(options->x_file).exists()) {
				printf("file:%s does not exists\n",options->x_file);
				fflush(stdout);
				exit(0);
			}
			Archive::ArcReader ar(options->x_file);
			switch(ar.formatByBuf()) {
			case Archive::FormatRar:	options->parser_type="unrar";	break;
			case Archive::FormatZip:	options->parser_type="unzip";	break;
			case Archive::Format7zip:	options->parser_type="7z";	break;
			default: break;
			}
			options->steps=ar.uncompressedSize();
		}//omit -T
#if OP_TEMPLATE
	QOutParser *parser=new QOutParser;
	parser->setLineFormat(options->parser_type);
#else
		QOutParser *parser=getParser(options->parser_type);
#endif //OP_TEMPLATE
#ifdef EZXT_QT4
	//msg->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//msg->setObjectName("QProgressDialog");
		QObject::connect(parser,SIGNAL(valueChanged(int)),msg,SLOT(setValue(int)));
	//QObject::connect(msg,SIGNAL(canceled()),qApp,SLOT(quit())); //does not work. Will send sig aboutToQuit()
		QObject::connect(msg,SIGNAL(canceled()),parser,SLOT(terminate()));
#else
	//msg->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//a.setMainWidget(msg);
		QObject::connect(parser,SIGNAL(valueChanged(int)),msg,SLOT(setProgress(int)));
		QObject::connect(msg,SIGNAL(cancelled()),parser,SLOT(terminate())); //to canceled
#endif //NO_EZX
		QObject::connect(parser,SIGNAL(textChanged(const QString&)),msg,SLOT(setLabelText(const QString&)));
		QObject::connect(parser,SIGNAL(maximumChanged(int)),msg,SLOT(setMaximum(int)));
		QObject::connect(parser,SIGNAL(finished()),msg,SLOT(showNormal()));
#ifndef NO_EZX
	//msg->exec(); //NO_MODAL
		a.processEvents();
//#else
	//msg->show();
#endif
		if(options->unit) parser->setCountType(Num);
		if(options->steps>0) {
			parser->setTotalSize(options->steps);
		} else {
			QStringList files=QStringList();
		//why is optind?
			for(int i=options->optind;i<argc;++i) files<<argv[i];
			parser->setFiles(files);
			parser->setMultiThread(options->multi_thread);
			parser->startCounterThread();
		}
		parser->start();
	}
#ifndef NO_EZX
	a.processEvents();
#endif
	//msg->exec();
	if(options->auto_close) exit(0);

	return a.exec();
}

