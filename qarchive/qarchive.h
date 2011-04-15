#ifndef QARCHIVE_H
#define QARCHIVE_H

#include "../global.h"
#include "../util.h"
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
namespace Archive {

	enum IODev {
		File, StdOut, StdIn, StdErr
	};

	enum Error {
		OpenError, ReadError, WriteError, ChecksumError, End, NoError
	};

class QArchive :public QObject
{
	Q_OBJECT
public:
	QArchive(const QString& file="",IODev idev=File,IODev odev=File);
	virtual ~QArchive()=0;

	void setInput(IODev idev);
	void setOutput(IODev odev);
	void setOutDir(const QString& odir);
	QString outDir() const;
	void setArchive(const QString& name);

	virtual Error extract(const QString& archive,const QString& dir=".")=0;
	virtual Error extract();
	virtual int verifyChecksum(const char*)=0;

	virtual uint unpackedSize();

	void estimate();
	void updateMessage();
	void finishMessage();
	//since the gui will be blocked while writng files, we must call forceShowMessage() to show text every interval ms.
	void forceShowMessage(int interval=1000);
	void checkTryPause();

	void create_dir(char *pathname, int mode); //move to parent
	FILE *create_file(char *pathname, int mode); //move to parent

signals:
	void byteProcessed(int size); //size_t
	void textChanged(const QString&);
	void finished();

public slots:
	void pauseOrContinue();
	void terminate();

protected:
	virtual void timerEvent(QTimerEvent *);

	IODev _output, _input;
	QString _archiveName, _outDir;
	//QStringList _inFiles;
	QFile _archiveFile;
	uint _totalSize, _processedSize;
	uint size;

	QString file;
	QTime _time;
	QString _out, _extra;
	uint _elapsed, _speed; //ms
	double  _left;
	QString max_str;
	int tid;

	int _numFiles;
	int time_passed;

	volatile bool _pause;
};

}

#endif // QARCHIVE_H
