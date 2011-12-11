#ifndef QARCHIVE_H
#define QARCHIVE_H

#include "qtcompat.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qobject.h>
#include <qdatetime.h>

#if QT_VERSION >= 0x040000
#	define ARCREADER_QT4 1
#else
#	define ARCREADER_QT4 0
#endif
/*
#if defined(Q_OS_WIN)
#  undef S_IFREG
#  define S_IFREG 0100000
#  ifndef S_IFDIR
#	define S_IFDIR 0040000
#  endif
#  ifndef S_ISDIR
#	define S_ISDIR(x) ((x) & S_IFDIR) > 0
#  endif
#  ifndef S_ISREG
#	define S_ISREG(x) ((x) & 0170000) == S_IFREG
#  endif
#  define S_IFLNK 020000
#  define S_ISLNK(x) ((x) & S_IFLNK) > 0
#  ifndef S_IRUSR
#	define S_IRUSR 0400
#  endif
#  ifndef S_IWUSR
#	define S_IWUSR 0200
#  endif
#  ifndef S_IXUSR
#	define S_IXUSR 0100
#  endif
#  define S_IRGRP 0040
#  define S_IWGRP 0020
#  define S_IXGRP 0010
#  define S_IROTH 0004
#  define S_IWOTH 0002
#  define S_IXOTH 0001
#endif

#if QT_VERSION >= 0x040000
extern unsigned int permissionsToMode(QFile::Permissions perms);
extern QFile::Permissions modeToPermissions(unsigned int mode);
#endif
*/

#define USE_SLOT 1

typedef struct
{
  void (*Progress)(const QString&, size_t current_size, size_t processed, size_t total_size, int speed, int time_elapsed, int time_remain);
} IProgressHandler;

namespace Archive {

	enum Error {
		OpenError, ReadError, WriteError, ChecksumError, End, NoError
	};

class QArchivePrivate;
class QArchive :
#if !(QT_VERSION >= 0x040000)
	public QObject,
#endif
	public QFile
{
	Q_OBJECT
public:
	QArchive(const QString& file="");
	virtual ~QArchive()=0;

	void setProgressHandler(IProgressHandler* ph);
	void setInterval(unsigned int interval);
	void setOutDir(const QString& odir);
	QString outDir() const;
	void setArchive(const QString& name);

	virtual Error extract(const QString& archive,const QString& dir=".")=0;
	virtual Error extract();
	virtual bool verifyChecksum(const char*)=0;

	virtual uint unpackedSize();

	void updateMessage();
	void finishMessage();
	//since the gui will be blocked while writng files, we must call forceShowMessage() to show text every interval ms.
	void forceShowMessage(int interval=1000);
	void checkTryPause();

	void createDir(const QString& pathname, int mode); //move to parent
	void createFile(const QString& pathname, int mode); //move to parent

signals:
	void byteProcessed(int size); //size_t
	void textChanged(const QString&);
	void finished();

public slots:
	void pauseOrContinue();
	void terminate();

protected:
	Q_DECLARE_PRIVATE(QArchive)
#if !INHERIT_PRIVATE || (QT_VERSION < 0x040000)
	QArchivePrivate* d_ptr;
#endif
	virtual void timerEvent(QTimerEvent *);

	IProgressHandler *progressHandler;
};

}

#endif // QARCHIVE_H
