/******************************************************************************
	QOP: Qt Output Parser for tar, zip etc with a compression/extraction progress indicator
	Copyright (C) 2011 Wangbin <wbsecg1@gmail.com>
	(aka. nukin in ccmove & novesky in http://forum.motorolafans.com)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/

#include "commandparser.h"
#include <qfileinfo.h>

#include "qarchive/arcreader.h"
#include "qtcompat.h"
#include "qcounterthread.h"
#include "utils/convert.h"

//skip whitespace?
#if !CONFIG_QT4
#define indexOf find
#define lastIndexOf findRev
#endif

class CommandParserPrivate
{
public:
	CommandParserPrivate(const QString& command = "")
		:counter(new QCounterThread),cmd(command),archive(""),files(QStringList())
		,compress_mode(false),count_type(CommandParser::Size)
	{
		parse();
	}
	virtual ~CommandParserPrivate() {
		if(counter) {
			delete counter;
			counter=0;
		}
	}

	QCounterThread *counter;
	QString cmd;
	QString archive;
	QStringList files;
	bool compress_mode;
	CommandParser::CountType count_type;

private:
	virtual void parse() {}
};

#define DECLARE_COMMANDPARSER_PRIVATE(T) \
	class T##CommandParserPrivate : public CommandParserPrivate { \
	public: \
		T##CommandParserPrivate(const QString& cmd):CommandParserPrivate(cmd) {parse();} \
	private: \
		virtual void parse(); \
	};

DECLARE_COMMANDPARSER_PRIVATE(Tar)
DECLARE_COMMANDPARSER_PRIVATE(Zip)
DECLARE_COMMANDPARSER_PRIVATE(Unzip)
DECLARE_COMMANDPARSER_PRIVATE(Unrar)


CommandParser::CommandParser(const QString &cmd)
	:d_ptr(0)
{
#if COUNTER_THREAD
	//counter(new QCounterThread);
#endif
	setCommand(cmd);
}
/*
CommandParser::CommandParser(const QString &program, const QStringList &argv)
{
	_cmd=program;
}
*/
CommandParser::~CommandParser()
{
	if(d_ptr) {
		delete d_ptr;
		d_ptr = 0;
	}
}

void CommandParser::setCommand(const QString &cmd)
{
	if(d_ptr) {
		delete d_ptr;
		d_ptr = 0;
	}
	if(cmd.startsWith("tar")) {
		d_ptr = new TarCommandParserPrivate(cmd);
	} else if(cmd.startsWith("zip")) {
		d_ptr = new ZipCommandParserPrivate(cmd);
	} else if(cmd.startsWith("unzip")) {
		d_ptr = new UnzipCommandParserPrivate(cmd);
	} else if (cmd.startsWith("unrar")) {
		d_ptr = new UnrarCommandParserPrivate(cmd);
	}
}

QStringList CommandParser::files()
{
	Q_D(CommandParser);
	return d->files;
}

CommandParser::CountType CommandParser::countType()
{
	Q_D(CommandParser);
	return d->count_type;
}

QString CommandParser::archive()
{
	Q_D(CommandParser);
	return d->archive;
}

bool CommandParser::isCompressMode()
{
	Q_D(CommandParser);
	return d->compress_mode;
}

QString CommandParser::program() const
{
	Q_D(const CommandParser);
	return d->cmd.left(d->cmd.indexOf(" "));
}

size_t CommandParser::filesCount() const
{
	Q_D(const CommandParser);
	if(d) {
//		counter=new QCounterThread(cmd_parser->files());
		return d->counter->numOfFiles(d->files); //zip no dir, virtual
	}
	return 0;
}

size_t CommandParser::filesSize() const
{
	Q_D(const CommandParser);
	if(d) {
	//	counter=new QCounterThread(d->files);
		return d->counter->sizeOfFiles(d->files); //zip no dir, virtual
	}
	return 0;
}

size_t CommandParser::archiveSize() const
{
	Q_D(const CommandParser);
	if(d)
		return QFileInfo(d->archive).size();
	return 0;
}

size_t CommandParser::archiveUnpackSize() const
{
	Q_D(const CommandParser);
	if(!d)
		return 0;
	uint unx_size = Archive::ArcReader(qPrintable(d->archive)).uncompressedSize();
	ZDEBUG("Archive unpacked size: %db == %s",unx_size,size2str(unx_size));
	return unx_size;
}

QCounterThread* CommandParser::counterThread() const
{
	Q_D(const CommandParser);
	return d->counter;
}


/*!
  Tar
*/
void TarCommandParserPrivate::parse()
{
	int index_start = cmd.indexOf(".tar "); //"tar "
	if(index_start<0) index_start = cmd.indexOf(".tgz ");
	if(index_start<0) index_start  =cmd.indexOf(".tbz2 ");
	if(index_start<0) index_start = cmd.indexOf(".tlz ");
	if(index_start<0) index_start = cmd.indexOf(".tlzma ");
	if(index_start<0) index_start = cmd.indexOf(".tlzo ");
	if(index_start<0) index_start = cmd.indexOf(".txz ");

	index_start = cmd.lastIndexOf(" ", index_start) + 1;
	int index_end = cmd.indexOf(" ", index_start);

	archive = cmd.mid(index_start, index_end - index_start);
	ZDEBUG("Archive is: %s", qPrintable(archive));

	int idx_archive = cmd.indexOf(archive);
	//files();
	if(!archive.isEmpty()) {

		int idx = cmd.indexOf(" ", idx_archive);
		QString fs = cmd.mid(idx);
		fs = fs.mid(0, fs.indexOf("-C")); //??? What i was thinking about? -C change dir

#if CONFIG_QT4
		files=fs.split(" ");
#else
		files=QStringList::split(" ", fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num = cmd.mid(0, idx_archive).count("v");
#else
	int v_num = cmd.mid(0, idx_archive).contains("v");
#endif
	if(v_num == 1) {
		count_type = CommandParser::Num;
		counter->setCountType(QCounterThread::Num);
	}
	else if(v_num==2) {
		count_type = CommandParser::Size;
		counter->setCountType(QCounterThread::Size);
	}

// compress mode: c true x false
	//ezDebug(_cmd.mid(0,idx_archive));
	if(cmd.mid(0,idx_archive).contains("c")) {
		compress_mode = true;
	}
	ZDEBUG("Compress Mode: %d", compress_mode);

}


/*!
  Zip
*/

void ZipCommandParserPrivate::parse()
{
	int index_start = cmd.indexOf(".zip ");

	index_start = cmd.lastIndexOf(" ", index_start) + 1;
	int index_end = cmd.indexOf(" ", index_start);

	archive = cmd.mid(index_start, index_end-index_start);
	ZDEBUG("Archive is: %s", qPrintable(archive));

	int idx_archive = cmd.indexOf(archive);
	//files();
	if(!archive.isEmpty()) {
		int idx = cmd.indexOf(" ", idx_archive);
		QString fs = cmd.mid(idx);
		fs = fs.mid(0, fs.indexOf("-C")); //-C change dir

#if CONFIG_QT4
		files = fs.split(" ");
#else
		files = QStringList::split(" ", fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num = cmd.mid(0,idx_archive).count("v");
#else
	int v_num = cmd.mid(0,idx_archive).contains("v");
#endif
	if(v_num==0) {
		count_type = CommandParser::Num;
		counter->setCountType(QCounterThread::Num);
	} else {
		count_type = CommandParser::Size;
		counter->setCountType(QCounterThread::Size);
	}
	compress_mode = true;
	ZDEBUG("Compress Mode: %d", compress_mode);

}

/*!
  Unzip: unzip -ov file.zip
*/

void UnzipCommandParserPrivate::parse()
{
	//lastIndexOf(" ");
	int index_start = cmd.indexOf(".zip ");

	index_start = cmd.lastIndexOf(" ",index_start)+1;
	int index_end = cmd.indexOf(" ",index_start);

	archive = cmd.mid(index_start, index_end-index_start);
	ZDEBUG("Archive is: %s", qPrintable(archive));

	int idx_archive = cmd.indexOf(archive);
	//files();
	if(!archive.isEmpty()) {

		int idx = cmd.indexOf(" ", idx_archive);
		QString fs = cmd.mid(idx);
		//-x xlist(inside zip file) -d exdir
#if CONFIG_QT4
		files = fs.split(" ");
#else
		files = QStringList::split(" ", fs);
#endif
	}

	//count type
#if CONFIG_QT4
	int v_num = cmd.mid(0,idx_archive).count("v");
#else
	int v_num = cmd.mid(0,idx_archive).contains("v");
#endif
	if(v_num==0) {
		count_type = CommandParser::Num;
		counter->setCountType(QCounterThread::Num);
	} else {
		count_type = CommandParser::Size;
		counter->setCountType(QCounterThread::Size);
	}
	compress_mode = false;
	ZDEBUG("Compress Mode: %d", compress_mode);
}

/*!
  Unrar: unrar x -o+ -p- -y rar.rar destdir
  //-o- not overwrite, -or rename auto, -p[pw]
*/

void UnrarCommandParserPrivate::parse()
{
	int index_start = cmd.indexOf(".rar ");

	index_start = cmd.lastIndexOf(" ", index_start) + 1;
	int index_end = cmd.indexOf(" ", index_start);

	archive = cmd.mid(index_start, index_end - index_start);
	ZDEBUG("Archive is: %s",qPrintable(archive));

	count_type = CommandParser::Num;
	counter->setCountType(QCounterThread::Num);

	compress_mode = false;
	ZDEBUG("Compress Mode: %d", compress_mode);
}
