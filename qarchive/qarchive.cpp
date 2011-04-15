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

QArchive::QArchive(const QString &file,IODev idev,IODev odev)
	:_output(odev),_input(idev),_outDir("."),_totalSize(0),_processedSize(0),size(0),file("") \
	,_out(""),_extra(tr("Calculating...")),_elapsed(0),_left(0),_numFiles(0),_pause(false)
{
	setArchive(file);
	_time.start();
}

QArchive::~QArchive()
{
}


void QArchive::create_dir(char *pathname, int mode)
{
	/* Strip trailing '/' */
	if (pathname[strlen(pathname) - 1] == '/')
		pathname[strlen(pathname) - 1] = '\0';
#if defined(linux) || defined(__linux) || defined(__linux__)
	char *p;
	int r;
	/* Try creating the directory. */
	r = mkdir(pathname, mode);
	if (r != 0) {
		/* On failure, try creating parent directory. */
		p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			r = mkdir(pathname, mode);
		}
	}
	//if (r != 0)
	//	fprintf(stderr, "Could not create directory %s\n", pathname);
#else
	QDir dir(_outDir);
	dir.mkdir(pathname);
#endif

}

/* Create a file, including parent directory as necessary. */
FILE * QArchive::create_file(char *pathname, int mode)
{
	Q_UNUSED(mode);
	FILE *f;
	f = fopen(pathname, "w+");
	if (f == NULL) {
		char *p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			f = fopen(pathname, "w+");
		}
	}
	return (f);
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
	_out=file+"\n"+QObject::tr("Size: ")+size2Str<float>(size)+"\n"+QObject::tr("Processed: ")+size2Str<float>(_processedSize)+max_str+"\n";
	_extra=QObject::tr("Speed: ")+size2Str<float>(_speed)+"/s\n"+QObject::tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	emit textChanged(_out+_extra);
	qApp->processEvents();
}

void QArchive::finishMessage()
{
	estimate();
	_out=QObject::tr("Finished: ")+QString::number(_numFiles)+" "+QObject::tr("files")+"\n"+size2Str<float>(_processedSize)+max_str+"\n";
	_extra=QObject::tr("Speed: ")+size2Str<float>(_processedSize/(1+_elapsed)*1000)+"/s\n"+QObject::tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	killTimer(tid);
	emit finished();
	emit textChanged(_out+_extra);
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
	max_str=" / "+size2Str<float>(_totalSize);
}

uint QArchive::unpackedSize()
{
	return _totalSize;
}

Error QArchive::extract()
{
	_time.restart();
	tid=startTimer(300); //startTimer(0) error in ezx
	return NoError;
}
}
//}
