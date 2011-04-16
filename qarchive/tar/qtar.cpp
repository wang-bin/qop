#include "qtar.h"
#include "TarHeader.h"
#include <sys/stat.h>  /* For mkdir() */

namespace Archive {
namespace Tar {

QTar::QTar(const QString &archive, IODev idev, IODev odev)
	:QArchive(archive,idev,odev)
{
}

QTar::~QTar()
{
	if(_archiveFile.isOpen()) _archiveFile.close();
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

int QTar::verifyChecksum(const char* p)
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
	return _totalSize;
}

Error QTar::extract()
{
	QArchive::extract();

	if(!_archiveFile.exists()) return OpenError;
	char buff[Header::RecordSize];
	//QFile _outFile;
	//FILE* f;
	size_t bytes_read;
	unsigned int filesize;

#if ARCREADER_QT4
	if(!_archiveFile.open(QIODevice::ReadOnly)) {
		_archiveFile.error();
#else
	if(_archiveFile.open(IO_ReadOnly)) {
		qDebug("open error");
#endif //ARCREADER_QT4
		return OpenError;
	}
	for (;;) {
#if ARCREADER_QT4
		bytes_read = _archiveFile.read(buff,Header::RecordSize);
#else
		bytes_read = _archiveFile.readBlock(buff,Header::RecordSize);
#endif //ARCREADER_QT4
		//put them here
		emit byteProcessed(_processedSize+=Header::RecordSize);
		_current_fileName=QFileInfo(buff).fileName();

		if (bytes_read < Header::RecordSize) {
			fprintf(stderr,"Short read. expected 512, got %d\n", bytes_read);
			return ReadError;
		}
		if (isEndBuff(buff)) {
			emit byteProcessed(_processedSize+=Header::RecordSize);  //header;
			finishMessage();
			return End;
		}
		if (!verifyChecksum(buff)) {
			fprintf(stderr, "Checksum failure\n");
			return ChecksumError;
		}

		filesize = parseOct(buff + 124, 12);
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

		++_numFiles;
		size=filesize;
		updateMessage();

		while (filesize > 0) {
			checkTryPause();
#if ARCREADER_QT4
			bytes_read = _archiveFile.read(buff,Header::RecordSize);
#else
			bytes_read = _archiveFile.readBlock(buff,Header::RecordSize);
#endif //ARCREADER_QT4
			if (bytes_read < Header::RecordSize) {
				fprintf(stderr,"Short read. Expected 512, got %d\n",bytes_read);
				return ReadError;
			}
			if (filesize < Header::RecordSize) bytes_read = filesize;
			if (_outFile.isOpen()) {
#if CONFIG_QT4
				if(_outFile.write(buff,bytes_read)!=bytes_read) {
#else
				if(_outFile.writeBlock(buff,bytes_read)!=bytes_read) {
#endif
				fprintf(stderr, "[%s] %s @%d: Failed to write %s\n",__FILE__,__PRETTY_FUNCTION__,__LINE__,UTIL::qstr2cstr(_outFile.fileName()));
				_outFile.close();
			    }
				/*if (fwrite(buff, 1, bytes_read, f)!= bytes_read) {
					fprintf(stderr, "Failed write\n");
					fclose(f);
					f = NULL;
				}*/
			}

			forceShowMessage(1000);
			emit byteProcessed(_processedSize+=Header::RecordSize);//bytes_read);
			filesize -= bytes_read;
		}
		//emit byteProcessed(_processedSize+=size);
		if(_outFile.isOpen()) _outFile.close();
		/*if (f != NULL) {
			fclose(f);
			f = NULL;
		}*/
	}
	_archiveFile.close();
}

Error QTar::extract(const QString& archive,const QString& dir)
{
	setArchive(archive);
	if(dir.isEmpty()) {
		setOutput(StdOut);
	} else {
		setOutDir(dir);
	}
	return extract();
}
}
}
