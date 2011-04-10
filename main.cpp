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
#include "qop.h"
#ifdef _OS_LINUX_
#include <sys/types.h>
#include <signal.h>
#endif
#if CONFIG_EZX
#include <ZApplication.h>
#include <ZLanguage.h>
#endif
//#include <qdir.h>
//#include <qfileinfo.h>
#include <qtranslator.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define VERSION "0.1.4"
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

#if CONFIG_QT4
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
#if CONFIG_EZX
	appTranslator.load(program+"_"+ZLanguage::getSystemLanguageCode(),dirname+"/i18n");
	//QString(dirname)+"/i18n" will load fail, 乱码
#else
	QString sysLang=QLocale::system().name();
	qDebug()<<sysLang;
	appTranslator.load(program+"-"+sysLang,dirname+"/i18n");
#endif //CONFIG_EZX
	a.installTranslator(&appTranslator);

	Qop *qop=new Qop;
#if !CONFIG_QT4
	a.setMainWidget(qop->progress);
#endif //CONFIG_QT4
	if(!options->hide) qop->progress->show();

	if(options->diy || argc<2) {
	    qop->setBuildinMethod(true);
	    qop->setArchive(options->x_file);
	    qop->initArchive();
	    /*
		Archive::QArchive* qarc=new Archive::Tar::QTar(options->x_file);
		printf("archive: %s\n",options->x_file);
		qop->progress->setMaximum(qarc->unpackedSize());
		qop->progress->addButton(QObject::tr("Pause"),1);
		QObject::connect(qop->progress->button(1),SIGNAL(clicked()),qarc,SLOT(pauseOrContinue()));
		QObject::connect(qarc,SIGNAL(byteProcessed(int)),qop->progress,SLOT(setValue(int)));
		QObject::connect(qarc,SIGNAL(textChanged(const QString&)),qop->progress,SLOT(setLabelText(const QString&)));
		QObject::connect(qop->progress,SIGNAL(canceled()),qarc,SLOT(terminate()));
		QObject::connect(qarc,SIGNAL(finished()),qop->progress,SLOT(showNormal()));
		*/
#if CONFIG_EZX
	//progress->exec(); //NO_MODAL
		a.processEvents();
//#else
	//progress->show();
#endif
		qop->archive->extract();
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
#if CONFIG_QT4
	//progress->setWindowTitle("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//progress->setObjectName("QProgressDialog");
		QObject::connect(parser,SIGNAL(valueChanged(int)),qop->progress,SLOT(setValue(int)));
	//QObject::connect(progress,SIGNAL(canceled()),qApp,SLOT(quit())); //does not work. Will send sig aboutToQuit()
		QObject::connect(qop->progress,SIGNAL(canceled()),parser,SLOT(terminate()));
#else
	//progress->setCaption("qop "+QObject::tr("Compression/Extraction progress dialog"));
	//a.setMainWidget(progress);
		QObject::connect(parser,SIGNAL(valueChanged(int)),progress,SLOT(setProgress(int)));
		QObject::connect(progress,SIGNAL(cancelled()),parser,SLOT(terminate())); //to canceled
#endif //CONFIG_EZX
		QObject::connect(parser,SIGNAL(textChanged(const QString&)),qop->progress,SLOT(setLabelText(const QString&)));
		QObject::connect(parser,SIGNAL(maximumChanged(int)),qop->progress,SLOT(setMaximum(int)));
		QObject::connect(parser,SIGNAL(finished()),qop->progress,SLOT(showNormal()));
#if CONFIG_EZX
	//progress->exec(); //NO_MODAL
		a.processEvents();
//#else
	//progress->show();
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
#if CONFIG_EZX
	a.processEvents();
#endif
	//progress->exec();
	if(options->auto_close) exit(0);

	return a.exec();
}

