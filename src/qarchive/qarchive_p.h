#ifndef QARCHIVE_P_H
#define QARCHIVE_P_H

#include <qobject.h>
#include <qfile.h>

namespace Archive {

class QArchivePrivate
{
public:
	QArchivePrivate():outDir("."),totalSize(0),processedSize(0),size(0) \
	,extra_msg(QObject::tr("Calculating...")),elapsed(0),left(0),numFiles(0),pause(false)
	{}
	~QArchivePrivate() {
		if(outFile.isOpen())
			outFile.close();
	}

	void init();

	QString outDir;
	//QStringList _inFiles;
	QFile outFile;
	uint totalSize, processedSize;
	uint size;

	QString current_fileName;
	QString out_msg, extra_msg;
	QString max_str;
	uint elapsed, speed; //ms
	double  left;

	int numFiles;
	int time_passed;
	volatile bool pause;

	int tid;
	QTime time;
};

} //namespace Archve
#endif // QARCHIVE_P_H
