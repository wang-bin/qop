#include "qarchive.h"
#include "qarchive_p.h"
#include <qdir.h>
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sys/stat.h>
#include <stdlib.h>
#endif
#include "utils/convert.h"
#include "utils/qt_util.h"
#include "msgdef.h"

//namespace Archive {
/*
#if (QT_VERSION >= 0x040000)
QFile::Permissions modeToPermissions(unsigned int mode)
{
	QFile::Permissions ret;
	if (mode & S_IRUSR)
		ret |= QFile::ReadOwner;
	if (mode & S_IWUSR)
		ret |= QFile::WriteOwner;
	if (mode & S_IXUSR)
		ret |= QFile::ExeOwner;
	if (mode & S_IRUSR)
		ret |= QFile::ReadUser;
	if (mode & S_IWUSR)
		ret |= QFile::WriteUser;
	if (mode & S_IXUSR)
		ret |= QFile::ExeUser;
	if (mode & S_IRGRP)
		ret |= QFile::ReadGroup;
	if (mode & S_IWGRP)
		ret |= QFile::WriteGroup;
	if (mode & S_IXGRP)
		ret |= QFile::ExeGroup;
	if (mode & S_IROTH)
		ret |= QFile::ReadOther;
	if (mode & S_IWOTH)
		ret |= QFile::WriteOther;
	if (mode & S_IXOTH)
		ret |= QFile::ExeOther;
	return ret;
}

unsigned int permissionsToMode(QFile::Permissions perms)
{
	quint32 mode = 0;
	if (perms & QFile::ReadOwner)
		mode |= S_IRUSR;
	if (perms & QFile::WriteOwner)
		mode |= S_IWUSR;
	if (perms & QFile::ExeOwner)
		mode |= S_IXUSR;
	if (perms & QFile::ReadUser)
		mode |= S_IRUSR;
	if (perms & QFile::WriteUser)
		mode |= S_IWUSR;
	if (perms & QFile::ExeUser)
		mode |= S_IXUSR;
	if (perms & QFile::ReadGroup)
		mode |= S_IRGRP;
	if (perms & QFile::WriteGroup)
		mode |= S_IWGRP;
	if (perms & QFile::ExeGroup)
		mode |= S_IXGRP;
	if (perms & QFile::ReadOther)
		mode |= S_IROTH;
	if (perms & QFile::WriteOther)
		mode |= S_IWOTH;
	if (perms & QFile::ExeOther)
		mode |= S_IXOTH;
	return mode;
}

#endif //ARCREADER_QT4
*/
namespace Archive {

QArchive::QArchive(const QString &archive)
:d_ptr(new QArchivePrivate)
#if !USE_SLOT
	progressHandler(new IProgressHandler)
#endif
{
	initTranslations();
	setArchive(archive);
	Q_D(QArchive);
	d->time.start();
}

QArchive::~QArchive()
{
	if(d_ptr) {
		delete d_ptr;
		d_ptr = 0;
	}
	if(isOpen()) close();
}

#if !USE_SLOT
void QArchive::setProgressHandler(IProgressHandler *ph)
{
	progressHandler=ph;
}
#endif
void QArchive::createDir(const QString& pathname, int mode)
{
	Q_D(QArchive);
	QDir(d->outDir).mkdir(pathname);
}

/* Create a file, including parent directory as necessary. */
void QArchive::createFile(const QString& pathname, int /*mode*/)
{
	Q_D(QArchive);
#if CONFIG_QT4
	d->outFile.setFileName(d->outDir+"/"+pathname);
	if(!d->outFile.open(QIODevice::ReadWrite)) {
#else
	d->outFile.setName(d->outDir+"/"+pathname);
	if(!d->outFile.open(IO_ReadWrite)) {
#endif
		ezDebug(d->outDir+"/"+pathname);
		if(pathname.left(1)=="/") {
			QDir(d->outDir).mkdir(pathname.mid(1));
		} else	QDir(d->outDir).mkdir(pathname);
	} else {
#if CONFIG_QT4
		//d->outFile.setPermissions();
#endif
	}
}

void QArchive::timerEvent(QTimerEvent *)
{
	updateMessage();//estimate();
	checkTryPause();
}

void QArchive::terminate()
{
	ZDEBUG("terminated!");
	exit(0);
}

void QArchive::pauseOrContinue()
{
	Q_D(QArchive);
	d->pause = !d->pause;
	if(!d->pause) {
		d->last_elapsed = d->elapsed;
		d->time.restart();
	}
}

void QArchive::updateMessage()
{
	Q_D(QArchive);
	d->estimate();
	d->out_msg = g_BaseMsg_Detail(d->current_fileName, d->size, d->processedSize, d->max_str);
	d->extra_msg = g_ExtraMsg_Detail(d->speed, d->elapsed, d->left);
	emit textChanged(d->out_msg+d->extra_msg);
	qApp->processEvents();
}

void QArchive::finishMessage()
{
	Q_D(QArchive);
	d->estimate();
	d->out_msg=QObject::tr("Finished: ") + QString::number(d->numFiles)+ QLatin1String(" ") +QObject::tr("files") + QLatin1String("\n") + QString(size2str(d->processedSize))+d->max_str + QLatin1String("\n");
	d->extra_msg=QObject::tr("Speed: ") + QString(size2str(d->processedSize/(1+d->elapsed)*1000)) + QLatin1String("/s\n") + QObject::tr("Elapsed: %1s Remaining: %2s").arg(g_time_convert(d->elapsed)).arg(g_time_convert(d->left));
	killTimer(d->tid);
	emit finished();
	emit textChanged(d->out_msg+d->extra_msg);
	qApp->processEvents();
}

void QArchive::forceShowMessage(int interval)
{
	Q_D(QArchive);
	if(d->time.elapsed()-d->time_passed>interval) {
		d->time_passed = d->time.elapsed();
		updateMessage();
	}
}

void QArchive::checkTryPause()
{
	Q_D(QArchive);
	while(d->pause) {
		QT_UTIL::qWait(100);
	}
}

void QArchive::setInterval(unsigned int interval)
{
	Q_D(QArchive);
	d->interval = interval;
}

void QArchive::setOutDir(const QString &odir)
{
	Q_D(QArchive);
	d->outDir = odir;
	if(!QDir(d->outDir).exists()) {
		ZDEBUG("out dir %s doesn't exist. creating...",qPrintable(d->outDir));
		QDir().mkdir(d->outDir);
	}
}

QString QArchive::outDir() const
{
	//Q_D(QArchive);
	return d_ptr->outDir;
}

void QArchive::setArchive(const QString &name)
{
	//QFileInfo(name).absoluteFilePath();
	if(isOpen())
		close();
#if (QT_VERSION >= 0x040000)
		setFileName(name);
#else
		this->QFile::setName(name);
#endif
	Q_D(QArchive);
	d->totalSize = size();
	d->max_str=QString(" / %1").arg(size2str(d->totalSize));
}

uint QArchive::unpackedSize()
{
	Q_D(QArchive);
	return d->totalSize;
}

Archive::Error QArchive::extract()
{
	Q_D(QArchive);
	d->time.restart();
	d->tid = startTimer(d->interval); //startTimer(0) error in ezx
	return Archive::NoError;
}
}
//}
