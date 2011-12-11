#ifndef QARCHIVE_P_H
#define QARCHIVE_P_H

#include <qobject.h>
#include <qfile.h>
#include <qapplication.h>

namespace Archive {

class QArchivePrivate
{
public:
	QArchivePrivate():outDir("."),totalSize(0),processedSize(0),size(0),extra_msg(QObject::tr("Calculating...")) \
	,last_elapsed(1),elapsed(0),time_passed(0),pause(false),numFiles(0),left(0)
	{
		interval = 300;
	/*last_elapsed=1 ensures that elpased = last_elapsed+time.elapsed>0*/
	}
	~QArchivePrivate() {
		if(outFile.isOpen())
			outFile.close();
	}

	void estimate() {
		if(!pause) elapsed = last_elapsed+time.elapsed();
		speed = processedSize/(1+elapsed)*1000; //>0
		left = (qreal)(totalSize-processedSize)/(qreal)(1+speed);
#ifndef NO_EZX
		qApp->processEvents();
#endif //NO_EZX
	}

	QString outDir;
	QFile outFile;
	uint totalSize, processedSize;
	uint size, interval;

	QString current_fileName;
	QString out_msg, extra_msg;
	QString max_str;
	uint last_elapsed, elapsed, speed; //ms
	int time_passed;
	volatile bool pause;
	int numFiles;
	qreal left;

	int tid;
	QTime time;
};

} //namespace Archve
#endif // QARCHIVE_P_H
