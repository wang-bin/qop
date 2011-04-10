#include "qop.h"
#include <qprocess.h>

#if CONFIG_QT4
#define FLAG Qt::WindowStaysOnTopHint
#else
#define FLAG Qt::WStyle_StaysOnTop
//using X::QApplication;
#endif

Qop::Qop():archive(0),parser(0),process(0)
  #ifndef EZPROGRESS
	  ,progress(new EZ_ProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,"UTIL_ProgressDialog",false,FLAG))
  #else
	  ,progress(new EZProgressDialog(QObject::tr("Calculating..."),QObject::tr("Cancel"),0,100,0,FLAG))
  #endif //EZPROGRESS
  ,buildin(false)
{
    initGui();
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

void Qop::setBuildinMethod(bool bi)
{
    buildin=bi;
    //if(buidin)
	//initArchive();
}

void Qop::setArchive(const QString &archive_path)
{
    arc_path=archive_path;
}
