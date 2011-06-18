#include "qarchive.h"
#include <qdir.h>
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sys/stat.h>
#include <stdlib.h>
#endif
#include "util.h"
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

QArchive::QArchive(const QString &archive,IODev idev,IODev odev)
	:_output(odev),_input(idev),_outDir("."),_totalSize(0),_processedSize(0),size(0),_current_fileName("") \
	,_out_msg(""),_extra_msg(tr("Calculating...")),_elapsed(0),_left(0),_numFiles(0),_pause(false)
#if !USE_SLOT
	,progressHandler(new IProgressHandler)
#endif
{
	setArchive(archive);
	_time.start();
}

QArchive::~QArchive()
{
}

#if !USE_SLOT
void QArchive::setProgressHandler(IProgressHandler *ph)
{
	progressHandler=ph;
}
#endif
void QArchive::createDir(const QString& pathname, int mode)
{
	QDir(_outDir).mkdir(pathname);
}

/* Create a file, including parent directory as necessary. */
void QArchive::createFile(const QString& pathname, int /*mode*/)
{
#if CONFIG_QT4
	_outFile.setFileName(_outDir+"/"+pathname);
	if(!_outFile.open(QIODevice::ReadWrite)) {
#else
	_outFile.setName(_outDir+"/"+pathname);
	if(!_outFile.open(IO_ReadWrite)) {
#endif
		ezDebug(_outDir+"/"+pathname);
		if(pathname.left(1)=="/") {
			QDir(_outDir).mkdir(pathname.mid(1));
		} else	QDir(_outDir).mkdir(pathname);
	} else {
#if CONFIG_QT4
		//_outFile.setPermissions();
#endif
	}
}

void QArchive::timerEvent(QTimerEvent *)
{
	updateMessage();//estimate();
	checkTryPause();
}

void QArchive::estimate()
{
	_elapsed=_time.elapsed()+1;
	_speed=_processedSize/(1+_elapsed)*1000; //>0
	_left= (_totalSize-_processedSize)/(1+_speed);
#ifndef NO_EZX
	qApp->processEvents();
#endif //NO_EZX
}

void QArchive::terminate()
{
	printf("terminated!\n");
	fflush(stdout);
	exit(0);
}

void QArchive::pauseOrContinue()
{
	_pause=!_pause;
}

void QArchive::updateMessage()
{
	estimate();
	_out_msg=_current_fileName+"\n"+QObject::tr("Size: ")+size2Str<double>(size)+"\n"+QObject::tr("Processed: ")+size2Str<double>(_processedSize)+max_str+"\n";
	_extra_msg=QObject::tr("Speed: ")+size2Str<double>(_speed)+"/s\n"+QObject::tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	emit textChanged(_out_msg+_extra_msg);
	qApp->processEvents();
}

void QArchive::finishMessage()
{
	estimate();
	_out_msg=QObject::tr("Finished: ")+QString::number(_numFiles)+" "+QObject::tr("files")+"\n"+size2Str<double>(_processedSize)+max_str+"\n";
	_extra_msg=QObject::tr("Speed: ")+size2Str<double>(_processedSize/(1+_elapsed)*1000)+"/s\n"+QObject::tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	killTimer(tid);
	emit finished();
	emit textChanged(_out_msg+_extra_msg);
	qApp->processEvents();
}

void QArchive::forceShowMessage(int interval)
{
	if(_time.elapsed()-time_passed>interval) {
		time_passed=_time.elapsed();
		updateMessage();
	}
}

void QArchive::checkTryPause()
{
	while(_pause) {
		UTIL::qWait(100);
	}
}

void QArchive::setInput(IODev idev)
{
	_input=idev;
}

void QArchive::setOutput(IODev odev)
{
	_output=odev;
}

void QArchive::setOutDir(const QString &odir)
{
	_outDir=odir;
	if(!QDir(_outDir).exists()) {
		ZDEBUG("out dir %s doesn't exist. creating...",qPrintable(_outDir));
		QDir().mkdir(odir);
	}
}

QString QArchive::outDir() const
{
	return _outDir;
}

void QArchive::setArchive(const QString &name)
{
	_archiveName=name;//QFileInfo(name).absoluteFilePath();
	if(_archiveFile.isOpen()) _archiveFile.close();
#if (QT_VERSION >= 0x040000)
	_archiveFile.setFileName(_archiveName);
#else
	_archiveFile.setName(_archiveName);
#endif
	_totalSize=_archiveFile.size();
	max_str=" / "+size2Str<double>(_totalSize);
}

uint QArchive::unpackedSize()
{
	return _totalSize;
}

Archive::Error QArchive::extract()
{
	_time.restart();
	tid=startTimer(300); //startTimer(0) error in ezx
	return Archive::NoError;
}
}
//}
