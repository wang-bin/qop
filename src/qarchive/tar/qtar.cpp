#include "qtar.h"
#include "TarHeader.h"
#include <sys/stat.h>  /* For mkdir() */
#include <qfileinfo.h>
#include "../qarchive_p.h"
#include "utils/util.h"

namespace Archive {
namespace Tar {

QTar::QTar(const QString &archive)
	:QArchive(archive)
{
}

QTar::~QTar()
{
	if(isOpen()) close();
}
/*
  //slower ?
static bool IsRecordLast(const char *buf)
{
  for (int i = 0; i < NFileHeader::kRecordSize; i++)
	if (buf[i] != 0)
	  return false;
  return true;
}
*/
bool QTar::isEndBuff(const char *buff)
{
	int n=511;
	for (; n >= 0; --n)
		if (buff[n] != '\0')
			return false;
	return true;
}

bool QTar::verifyChecksum(const char* p)
{
	int n, u = 0;
	for (n = 0; n < 512; ++n) {
		if (n < 148 || n > 155)
			u += ((unsigned char *)p)[n];
		else
			u += 0x20;
	}
	return (u == parseOct(p + 148, 8));
}

uint QTar::unpackedSize()
{
	Q_D(QArchive);
	return d->totalSize;
}

Error QTar::extract()
{
	//ifstream ofstream to seekg()
	QArchive::extract();

	if(!exists()) return Archive::OpenError;
	char buff[Header::RecordSize];
	//QFile outFile;
	//FILE* f;
	size_t bytes_read;
	unsigned int filesize;

#if ARCREADER_QT4
	if(!open(QIODevice::ReadOnly)) {
		error();
#else
	if(open(IO_ReadOnly)) {
		qDebug("open error");
#endif //ARCREADER_QT4
		return Archive::OpenError;
	}
	Q_D(QArchive);
	for (;;) {
#if ARCREADER_QT4
		bytes_read = read(buff,Header::RecordSize);
#else
		bytes_read = readBlock(buff,Header::RecordSize);
#endif //ARCREADER_QT4
		//put them here
		emit byteProcessed(d->processedSize+=Header::RecordSize);
		d->current_fileName=QFileInfo(buff).fileName();

		if (bytes_read < Header::RecordSize) {
			fprintf(stderr,"Short read. expected 512, got %d\n", bytes_read);
			return Archive::ReadError;
		}
		if (isEndBuff(buff)) {
#if USE_SLOT
			emit byteProcessed(d->processedSize+=Header::RecordSize);  //header;
#else
			estimate();
			progressHandler->Progress(d->current_fileName, d->size, d->processedSize+=Header::RecordSize, d->totalSize, d->speed, d->elapsed, d->left);
#endif
			finishMessage();
			return End;
		}
		if (!verifyChecksum(buff)) {
			fprintf(stderr, "Checksum failure\n");
			return ChecksumError;
		}

		switch (buff[156]) {
		case Header::LinkFlag::kLink :			printf(" Ignoring hardlink %s\n", buff); break;
		case Header::LinkFlag::kSymbolicLink :	printf(" Ignoring symlink %s\n", buff); break; /////////////////////////
		case Header::LinkFlag::kCharacter:		printf(" Ignoring character device %s\n", buff); break;
		case Header::LinkFlag::kBlock:			printf(" Ignoring block device %s\n", buff); break;
		case Header::LinkFlag::kDirectory:
			createDir(QString::fromLocal8Bit(buff), parseOct(buff + 100, 8));
			filesize = 0;
			break;
		case Header::LinkFlag::kFIFO:			printf(" Ignoring FIFO %s\n", buff); break;
		default:
			createFile(QString::fromLocal8Bit(buff), parseOct(buff + 100, 8));
			break;
		}

		++d->numFiles;
		filesize = parseOct(buff + 124, 12);
		d->size = filesize;
#if USE_SLOT
		updateMessage();
#endif
		while (filesize > 0) {
			checkTryPause();
#if ARCREADER_QT4
			bytes_read = read(buff,Header::RecordSize);
#else
			bytes_read = readBlock(buff,Header::RecordSize);
#endif //ARCREADER_QT4
			if (bytes_read < Header::RecordSize) {
				fprintf(stderr,"Short read. Expected 512, got %d\n",bytes_read);
				return Archive::ReadError;
			}
			if (filesize < Header::RecordSize) bytes_read = filesize;
			if (d->outFile.isOpen()) {
#if CONFIG_QT4
				if(d->outFile.write(buff,bytes_read)!=bytes_read) {
					fprintf(stderr, "[%s] %s @%d: Failed to write %s\n",__FILE__,__PRETTY_FUNCTION__,__LINE__,qPrintable(d->outFile.fileName()));
#else
				if(d->outFile.writeBlock(buff,bytes_read)!=bytes_read) {
					fprintf(stderr, "[%s] %s @%d: Failed to write %s\n",__FILE__,__PRETTY_FUNCTION__,__LINE__,qPrintable(d->outFile.name()));
#endif
					d->outFile.close();
				}
				/*if (fwrite(buff, 1, bytes_read, f)!= bytes_read) {
					fprintf(stderr, "Failed write\n");
					fclose(f);
					f = NULL;
				}*/
			}
#if USE_SLOT
			forceShowMessage(1000);
			emit byteProcessed(d->processedSize+=Header::RecordSize);//bytes_read);
#else
			estimate();
			progressHandler->Progress(d->current_fileName, d->size, d->processedSize+=Header::RecordSize, d->totalSize, d->speed, d->elapsed, d->left);
#endif
			filesize -= bytes_read;
		}
		//emit byteProcessed(processedSize+=size);
		if(d->outFile.isOpen()) d->outFile.close();
	}
	close();
}

Archive::Error QTar::extract(const QString& archive,const QString& dir)
{
	setArchive(archive);
	setOutDir(dir);

	return extract();
}
}
}
