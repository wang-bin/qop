/******************************************************************************
	QOutParser: Achieve tools' output parser. It's a part of QOP.
	Copyright (C) 2010 Wangbin <wbsecg1@gmail.com>
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

#include "QOutParser.h"
#include <algorithm>
#include <limits>

#if !NO_SOCKET
#include <qsocketnotifier.h>
#endif //NO_SOCKET
#ifndef QT_NO_TEXTSTREAM
#include <qtextstream.h>
#endif //QT_NO_TEXTSTREAM

#include "utils/util.h"
#include "utils/convert.h"
#include "msgdef.h"

QOutParser* getParser(const QString& type)
{
#if CONFIG_QT4
	QString t=type.toLower();
#else
	QString t=type.lower();
#endif // CONFIG_QT4
	if(t==QLatin1String("tar")) {
		return new QTarOutParser;
	} else if(t==QLatin1String("zip")) {
		return new QZipOutParser;
	} else if(t==QLatin1String("untar")) {
		return new QUntarOutParser;
	} else if(t==QLatin1String("unzip")) {
		return new QUnzipOutParser;
	} else if(t==QLatin1String("unrar")) {
		return new QUnrarOutParser;
	} else if(t==QLatin1String("7z")) {
		QOutParser *qop=new Q7zOutParser;
		qop->setCountType(QCounterThread::NumNoDir);
		return qop;
	} else if(t==QLatin1String("lzip")) {
		return new QLzipOutParser;
	} else if(t==QLatin1String("upx")) {
		return new QUpxOutParser;
	} else {
		return new QOutParser;
	}
}


class QOutParserPrivate {
public:
	QOutParserPrivate() {
		file = out = "";
		extra = QObject::tr("Calculating...");
		size = compressed = value = elapsed = left = max_value = 0;
		interval = 300;
		max_value = std::numeric_limits<uint>::max();
		res_tmp = res = Unknow;
		count_type = QCounterThread::Size;
		multi_thread =  update_msg_on_change = false;
		recount = true;

		//time.start();
	}

	void estimate();

	//use QLatin1String
	QString file; //file just compressed/extracted
	QString line; //char line[LINE_LENGTH_MAX]; //add char name[256], ratio[4], int s? they are frequently used in parse
	uint size, compressed, value; //outSize numbers-->value
	QString ratio; //ratio: zip
	QTime time;
	QString out, extra;
	uint elapsed, speed; //ms
	qreal left;
	volatile uint max_value;
	QString max_str;
	Format res, res_tmp;
	int tid, interval;

	QCounterThread counter;
#if CONFIG_QT4
	//QThread paserThread;
#endif //CONFIG_QT4
	QCounterThread::CountType count_type;
	bool multi_thread;
	static int detail_freq, simple_freq, detail_ratio_freq;
	bool recount;
	bool update_msg_on_change;
};

int QOutParserPrivate::detail_freq=0;
int QOutParserPrivate::simple_freq=0;
int QOutParserPrivate::detail_ratio_freq=0;


const qreal KInvMath=1./1000; //to speed up
void QOutParserPrivate::estimate()
{
	/*
	_elapsed=_time.elapsed()+1;
	//assert(_elapsed>0);
	//if(count_type==Size) _speed=value/_elapsed*1000; //>0  _speed=value/(1+_elapsed)*1000
	else _speed=value*1000/_elapsed; //>0
	*/
	elapsed=time.elapsed();
	speed=value/(elapsed*KInvMath+1);
	left= (qreal)(max_value-value)/(qreal)(1+speed);
	/*
	printf("t=%d, v=%d\n",_elapsed,_speed);
	_left= (qreal)(max_value-value)*_elapsed/(qreal)(value*1000+1); //a/b ~ a/(b+1)
	fflush(stdout);*/
#ifndef NO_EZX
	qApp->processEvents();
#endif //NO_EZX
}



QOutParser::QOutParser()
	:QObject(0),d_ptr(new QOutParserPrivate)
{
	initTranslations();

	Q_D(QOutParser);
#if CONFIG_QT4
	//moveToThread(&d->paserThread);
	//d->paserThread.start();
#endif //CONFIG_QT4
	connect(this, SIGNAL(finished()), SLOT(slotFinished()));
#if CONFIG_QT4
	connect(&d->counter, SIGNAL(maximumChanged(int)), SLOT(setTotalSize(int)), Qt::DirectConnection);
#else
	connect(&d->counter, SIGNAL(maximumChanged(int)), SLOT(setTotalSize(int)));
#endif //CONFIG_QT4
	connect(this, SIGNAL(unitChanged()), SLOT(slotResetUnit()));
#if !NO_SOCKET
	//QSocketNotifier *stdin_notifier=new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read,this);
	//connect(stdin_notifier,SIGNAL(activated(int)),SLOT(readFromSocket(int)));
	//initTimer();
#endif
}

QOutParser::~QOutParser()
{
	if (d_ptr) {
		delete d_ptr;
		d_ptr = 0;
	}
}

void QOutParser::parseLine(const QString& line)
{
	Q_D(QOutParser);
	Format res_old = d->res;
	d->res = parse(line);
	if(d->res == Detail) {
		d->detail_freq++;
	} else if(d->res == Simple) {
		d->simple_freq++;
	} else if(d->res == DetailWithRatio) {
		d->detail_ratio_freq++;
	}
	if(std::max(d->detail_freq,d->simple_freq) == d->detail_freq) {
		if(std::max(d->detail_freq, d->detail_ratio_freq) == d->detail_freq)
			d->res = Detail;
		else
			d->res = DetailWithRatio;
	} else {
		if(std::max(d->simple_freq, d->detail_ratio_freq) == d->simple_freq)
			d->res = Simple;
		else
			d->res = DetailWithRatio;
	}
	/*!
		qApp->processEvents() will down the performance and may cause crash. We do it every 0x10 times.
		if we do not call qApp->processEvents(), multi-thread counting information can't display right.
	*/
	if(d->res != res_old || ((d->detail_freq + d->simple_freq + d->detail_ratio_freq)&0x10) || (d->detail_freq + d->simple_freq + d->detail_ratio_freq<4))
		qApp->processEvents();

	if(d->res == Detail) {
		//qDebug("%s==%s", size2str(d->size), size2str(d->value));
		d->out = g_BaseMsg_Detail(d->file, d->size, d->value, d->max_str);
		d->extra = g_ExtraMsg_Detail(d->speed, d->elapsed, d->left);
	} else if(d->res == Simple) {
		d->out = g_BaseMsg_Simple(d->file, ++d->value, d->max_str);
		d->extra = g_ExtraMsg_Simple(d->speed, d->elapsed, d->left);
	} else if(d->res == DetailWithRatio) {
		d->out = g_BaseMsg_Ratio(d->file, d->size, d->ratio, d->value, d->max_str);
		d->extra = g_ExtraMsg_Ratio(d->speed, d->elapsed, d->left);
	} else if(d->res == Unknow) {
		qDebug("%s", qPrintable(line));
		return;
	} else if(d->res == Error) {
		qDebug("%s", qPrintable(line));
		d->out=tr("Password Error!");
		d->extra="";
	} else {
		d->out = line;
		d->extra = "";
	}
	emit valueChanged(d->value);
	if (d->update_msg_on_change)
		emit textChanged(d->out + d->extra);
}


void QOutParser::startCounterThread()
{
	Q_D(QOutParser);
	if(!d->recount)
		return;
	emit textChanged(tr("Calculating..."));
	qApp->processEvents();
	if(d->multi_thread)
		d->counter.start();
	else
		d->counter.run();
}


void QOutParser::setInterval(unsigned int interval)
{
	Q_D(QOutParser);
	d->interval = interval;
}

void QOutParser::setUpdateMsgOnChange(bool on)
{
	Q_D(QOutParser);
	d->update_msg_on_change = on;
}

void QOutParser::initTimer()
{
	Q_D(QOutParser);
	d->time.restart();
	d->tid = startTimer(d->interval); //startTimer(0) error in ezx
}
#if !NO_SOCKET
/*
#include <qfile.h>
void QOutParser::readFromSocket(int socket)
{
	ZDEBUG();
	QSocketNotifier *sn = qobject_cast<QSocketNotifier*>(sender());
	sn->setEnabled(false);
	QFile f;
	f.open(socket, QIODevice::ReadOnly);
	f.readLine(line, LINE_LENGTH_MAX);
	sn->setEnabled(true);
	parseLine(line);
}
*/
#endif

void QOutParser::start() {
	Q_D(QOutParser);
	initTimer();
#ifndef QT_NO_TEXTSTREAM
#if CONFIG_QT4
	QTextStream stream(stdin, QIODevice::ReadOnly);
#else
	QTextStream stream(stdin, IO_ReadOnly);
#endif //CONFIG_QT4
	do {
		d->line = stream.readLine();
		d->res_tmp = d->res;
		parseLine(d->line);
		if(d->res != d->res_tmp)
			emit unitChanged();
	} while (!d->line.isNull());
#else
	char line_tmp[LINE_LENGTH_MAX];
	while(fgets(line_tmp, LINE_LENGTH_MAX, stdin)) {
		line = QString(line_tmp);
		res_tmp=res;
		parseLine(line);
		if(res!=res_tmp)
			emit unitChanged();
	}
#endif //QT_NO_TEXTSTREAM
	emit finished();
}

void QOutParser::setCountType(QCounterThread::CountType ct)
{
	Q_D(QOutParser);
	d->count_type = ct;
	d->counter.setCountType(d->count_type);
}

void QOutParser::setRecount(bool rc)
{
	Q_D(QOutParser);
	d->recount = rc;
}

void QOutParser::setMultiThread(bool mt)
{
	Q_D(QOutParser);
#if !CONFIG_QT4 && !defined(QT_THREAD_SUPPORT)
	if(mt) {
		ZDEBUG("Multi-threading is not support!");
		d->multi_thread = false;
		return;
	}
#endif
	d->multi_thread = mt;
	ZDEBUG("Multi-threading enabled=%d", d->multi_thread);
}

void QOutParser::setFiles(const QStringList &f)
{
	Q_D(QOutParser);
	d->counter.setFiles(f);
}

Format QOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	d->out = line; //can remove
	return All;
}

void QOutParser::timerEvent(QTimerEvent *)
{
	Q_D(QOutParser);
	d->estimate();
	if (!d->update_msg_on_change)
		emit textChanged(d->out + d->extra);
}

void QOutParser::slotFinished()
{
	Q_D(QOutParser);
	killTimer(d->tid);
	d->estimate();
	if (!d->update_msg_on_change)
		emit textChanged(d->out + d->extra);

	printf("\nOut: %d b, Time: %.1f s, Speed: %d Kb/s\n", d->value, d->elapsed/1000., d->value/(1+d->elapsed));
	fflush(NULL);
	//value*1000/_elapsed is not correct, why?
	if(d->res == Simple || d->res == End7z) {
		d->out = tr("Finished: ") + QString("%1").arg(d->value) + d->max_str + tr("files")+QLatin1String("\n");
		d->extra = tr("Speed: ") + QString::number(d->speed) + QLatin1String("/s\n") + tr("Elapsed time: ") + QString("%1s").arg(g_time_convert(d->elapsed));
	} else if(d->res == Detail) {
		d->out = tr("Finished: ") + QLatin1String(size2str(d->value)) + d->max_str + QLatin1String("\n");
		d->extra = tr("Speed: ") + QLatin1String(size2str(d->value/(1+d->elapsed)*1000)) + QLatin1String("/s\n") + tr("Elapsed: %1s Remaining: %2s").arg(g_time_convert(d->elapsed)).arg(g_time_convert(d->left));
	} else if(d->res == DetailWithRatio) {
		d->out = tr("Finished: ") + QLatin1String(size2str(d->value)) + d->max_str + QLatin1String("\n");
		d->extra = tr("Speed: ") + QLatin1String(size2str(d->value/(1+d->elapsed)*1000)) + QLatin1String("/s\n") + tr("Elapsed: %1s Remaining: %2s").arg(g_time_convert(d->elapsed)).arg(g_time_convert(d->left));
	} else if(d->res == EndZip) { //zip
		d->out = tr("Finished: ") + QLatin1String(size2str(d->value)) + d->max_str + QLatin1String("\n") + tr("Compressed: ") + QLatin1String(size2str(d->compressed)) + QLatin1String("\n") + tr("Compression ratio: ") + d->ratio + QLatin1String("\n");
		d->extra = tr("Speed: ") + QLatin1String(size2str(d->value/(1+d->elapsed)*1000)) + QLatin1String("/s\n") + tr("Elapsed: %1s Remaining: %2s").arg(g_time_convert(d->elapsed)).arg(g_time_convert(d->left));
	} else if(d->res == Error) {
		d->out = tr("Password Error!");
		d->extra = "";
	}
	emit textChanged(d->out+d->extra);
}

void QOutParser::setTotalSize(int s)
{
	Q_D(QOutParser);
	if(!d->multi_thread) {
		if(d->count_type == QCounterThread::Size)
			emit textChanged(tr("Counting...") + QLatin1String(size2str(s)));
		else
			emit textChanged(tr("Counting...") + QString::number(s));
	}
	//ZDEBUG("Total size changes to %d", s);
	emit maximumChanged(d->max_value=s);
	//qApp->processEvents();
	d->estimate();
	if(d->count_type == QCounterThread::Size)
		d->max_str = QString(" / %1").arg(size2str(d->max_value));
	else
		d->max_str = " / " + QString::number(d->max_value) + " ";
}

void QOutParser::slotResetUnit()
{
	Q_D(QOutParser);
	if(d->res == Simple)	 {
		setCountType(QCounterThread::Num);
		if(d->recount)
			startCounterThread();
		else
			setTotalSize(d->max_value);
	}
		//max_str=" / "+QString::number(max_value)+" ";
	else {
		setCountType(QCounterThread::Size);
		if(d->recount)
			startCounterThread();
		else
			setTotalSize(d->max_value);
	}
		//max_str=" / "+size2str(max_value);
	ZDEBUG("steps: %d", d->max_value);
	emit maximumChanged(d->max_value);

}

#if defined(Q_OS_LINUX) || defined(Q_OS_CYGWIN)
#include <sys/types.h>
#include <signal.h>
#include <qfileinfo.h>
#include <qdir.h>
#endif
#include <stdlib.h>
#include <qthread.h>

//how to kill relative pids?
void QOutParser::terminate()
{
#if CONFIG_QT4
	char *senderName = sender()->objectName().toLocal8Bit().data();
#else
	const char *senderName = sender()->name();
#endif
	ZDEBUG("signal sender: %s", senderName);
#if defined(Q_OS_LINUX)
	ZDEBUG("OS LINUX");
	//bash: ppid!=pid==pgid==..., qpkg:pid!=ppid==pgid, tar,this,etc:ppid!=pid!=pgid
	pid_t pgid=getpgrp();
	char cmd[64];
	int pid, ppid, pgrp;
	//QDir *procdir = new QDir("/proc", 0, QDir::Name, QDir::Dirs);
	QDir procdir("/proc", 0, QDir::Name, QDir::Dirs);
#if CONFIG_QT4
	const QFileInfoList proclist =procdir.entryInfoList();
	//QFileInfoList *proclist = new QFileInfoList(procdir->entryInfoList());
	QListIterator<QFileInfo> it( proclist );
	QFileInfo f;
	if ( !proclist.isEmpty() ) {
#else
	const QFileInfoList *proclist =procdir.entryInfoList();
	QFileInfoListIterator it( *proclist );
	//QFileInfoList *proclist = new QFileInfoList(*(procdir->entryInfoList()));
	QFileInfo *f;
	if ( !proclist->isEmpty() ) {
#endif //CONFIG_QT4

#if CONFIG_QT4
		while(it.hasNext()) {
			f=it.next();
			QString pidNow=f.fileName();
#else
		while ( ( f = it.current() ) != 0 ) {
			++it;
			QString pidNow=f->fileName();
#endif //CONFIG_QT4
			if ( pidNow >= "0" && pidNow <= "99999" ) {
#if CONFIG_QT4
				FILE *procfile = fopen( QString("/proc/" + pidNow + "/stat").toLocal8Bit().constData(), "r");
#else
				FILE *procfile = fopen( ( QString ) ( "/proc/" + pidNow + "/stat"), "r");
#endif //CONFIG_QT4
				if ( procfile ) {
					fscanf( procfile,"%d %s %*c %d %d",&pid, cmd, &ppid, &pgrp);
					fclose( procfile );
					/*
						Send qpkg SIGTERM. If pgrp!=ppid isn't committed up, tar won't be terminated if the
						command is started in a terminal. Tell whether cmd contains tar, gzip etc is recommand
						, or it will kill other processes started by qpkg.
						pid: tar < qop < gzip, so gzip will not killed by qop
					*/
					if(pgid==pgrp /*&& pgrp!=ppid */&& pgid!=pid) {
						QString cmd_q(cmd);
						if(cmd_q.startsWith("(tar") || cmd_q.contains("7z") || cmd_q.startsWith("(zip") || cmd_q.startsWith("(unzip") || \
							cmd_q.startsWith("(rar") || cmd_q.startsWith("(unrar") || cmd_q.startsWith("(upx") || \
							cmd_q.startsWith("(bzip2") || cmd_q.startsWith("(unbzip2") || cmd_q.startsWith("(gzip") || \
							cmd_q.startsWith("(xz") || cmd_q.startsWith("(lzop") || cmd_q.startsWith("(lzip") || \
							cmd_q.startsWith("(lzma")) {
							printf("pid: %-5d ppid: %-5d pgrp: %-5d cmd:%s...recieves signal SIGTERM\n",pid,ppid,pgrp,cmd);
							kill(pid,SIGTERM);
						}
					}
				}
			}
		}
	}
	ZDEBUG("try kill(getpid(),SIGKILL)");
	kill(getpid(),SIGKILL);
	ZDEBUG("try kill(getpid(),SIGTERM)");
	kill(getpid(),SIGTERM);
#endif
#if defined(QT_THREAD_SUPPORT)
#	if CONFIG_QT4
	QThread::currentThread()->exit();
#else
	QThread::exit();
#	endif
#endif
	ZDEBUG("terminate by exit(1)");
	exit(1);
}


//subclasse

/*!
	command:
	tar --use-compress-program=lzip -cvvf core.tar.lz core
	tar --use=lzip -cvvf core.tar.lz core
	output:
	drwxr-xr-x wbin/wbin		 0 2010-06-27 12:56 core/
	-rwxr-xr-x wbin/wbin	  7137 2010-06-27 01:10 core/p

	command:
	tar --use=lzip -cvf core.tar.lz core
	output:
	core/
	core/p
*/
Format QTarOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	int s=0;
	char name[256];
	if(line.contains(" ")) {
		sscanf(qPrintable(line), "%*s%*s%d%*s%*s%s", &s, name);
		d->value += d->size= s;
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty()) { //"./a/"
			d->file = name;
		}
		return Detail;
	} else {
		//puts(line);
		d->file = QFILENAME(line);
		if(d->file.isEmpty()) {
			d->file = line;
		}
		return Simple;
	}
}



QUntarOutParser::QUntarOutParser()
	:QOutParser()
{
	Q_D(QOutParser);
	d->value = 512*2; //2*sizeof(tar_header)
}

/*!
  FIXME: QFileInfo(name).fileName() is slow and cost to much;
*/
Format QUntarOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	int s=0;
	char name[256];
	if(line.contains(" ")) {
		sscanf(qPrintable(line), "%*s%*s%d%*s%*s%s", &s, name);
		d->size = s;
		d->value += 512+ROUND512(s);//512+(s%512 ? s+512-s%512 :s); //sizeof(tar_header)+n*512: 512+s+512+(-s)%512
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty()) { //"./a/"
			d->file=name;
		}
		return Detail;
	} else {
		//puts(line);
		d->file = QFILENAME(line);
		if(d->file.isEmpty()) {
			d->file = line;
		}
		return Simple;
	}
}

/*!
	command:
	zip -ry -P pw test.zip test (-Ppw)
	output:
	updating: core/p (deflated 70%)

	command:
	zip -ryv -9 -FS -P pw -db -dc test.zip test //if exists a non-zip file with the same name, then error
	output:
	 0/ 12 [   0/133k]  deleting: core/a
	 1/ 11 [ 12k/121k]  updating: core/p	(in=7137) (out=2146) (deflated 70%)
	 2/ 10 [ 31k/ 90k]	adding: core/qq	(in=7137) (out=2146) (deflated 70%)
	total bytes=23479416, compressed=23432410 -> 0% savings

	[processed/remaining]
	zip 3.0 win32:
	2/ 10 [ 31k/ 90k]	adding: core/qq	(80 bytes security) (in=7137) (out=2146) (deflated 70%)
 */
Format QZipOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	int s=0;
	char name[256],r[4];
	if(line.contains("updating:") || (line.contains("adding:"))) {
		if(line.contains("in=")) {
			sscanf(qPrintable(line), "%*s%s\t(in=%d)%*s%*s%[^)]", name, &s, r);
			d->ratio = r;
			d->value += d->size = s;
			d->file = QFILENAME(QString(name));
			if(d->file.isEmpty()) { //"./a/"
				d->file = name;
			}
			return DetailWithRatio;
		}  else {
			sscanf(qPrintable(line), "%*s%s", name);
			d->file = QFILENAME(QString(name));
			if(d->file.isEmpty()) {
				d->file = name;
			}
			return Simple;
		}
	} else if(line.contains("->")) {//finished
		char in_s[32], out_s[32], r[3];
		sscanf(qPrintable(line), "%*[^=]=%[^,]%*[^=]=%[^ ]%*[^>]>%[^%]", in_s, out_s, r);
		d->value = QString(in_s).toInt();
		d->compressed = QString(out_s).toInt();
		d->ratio = QString(r) + "%";
		//ratio=QString().sprintf("%.2f",(qreal)compressed/(qreal)value*100.)+"%";
		return EndZip;
	} else if(line.contains("Archive is current")) {
		return All;
	}
	return Unknow;
}

/*!
	command: unzip -o zip.zip
	output:
	Archive:  core.zip
	   creating: core/
	  inflating: core/p
	 extracting: core/tt


	command: unzip -ov zip.zip -d exdir, do not extract
		-P pw , -P- skip pw
	output:
	Archive:  core.zip
	Length   Method	Size  Cmpr	Date	Time   CRC-32   Name
	--------  ------  ------- ---- ---------- ----- --------  ----
		7137  Defl:N	 2146  70% 2010-06-27 01:10 a601af6a  core/p
		   0  Stored		0   0% 2010-06-27 10:44 00000000  core/
	--------		  -------  ---							-------
	16611901		 16194567   3%							19 files

	unzip -Z -t file.zip:
	20 files, 9999 bytes uncompressed, 1111 bytes compressed:  10%
*/

//	get bytes: unzip -Z -t img.zip |sed 's/\(.*\), \(.*\) bytes \(.*\) bytes \(.*\)/\2/'
//	get number: unzip -Z -t img.zip |sed 's/\(.*\) files.*/\1/'
Format QUnzipOutParser::parse(const QString& line)
{
	if(line.contains("Archive")) return Unknow;
	if(line.contains("--------")) return Unknow;

	if((line.contains("incorrect"))) {
		return Error; //not work??
	}

	Q_D(QOutParser);
	int s=0;
	char name[256],r[4];
	if(line.contains("%")) {
		// change to if( extracting, inflating)
		if(!line.contains(":")) return Unknow;
		sscanf(qPrintable(line), "%d%*s%*d%s%*s%*s%*s%s", &s, r, name);
		d->value += d->size = s;
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty()) { //"./a/"
				d->file = name;
		}
		return Detail;
	} else {
		sscanf(qPrintable(line), "%*s%s", name);
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty()) {
				d->file=name;
		}
		return Simple;
	}
}
/*!
	command: unrar x -o+ -p- -y rar.rar destdir
		//-o- not overwrite, -or rename auto, -p[pw]
	output:

	UNRAR 3.90 beta 2 freeware	  Copyright (c) 1993-2009 Alexander Roshal


	Extracting from core.rar

	Extracting  core/p													OK
	All OK
 */
Format QUnrarOutParser::parse(const QString& line)
{
	if(line.contains("UNRAR ")) return Unknow;
	if(QString(line).isEmpty()) return Unknow;

	Q_D(QOutParser);
	char name[256];
	sscanf(qPrintable(line), "%*s%s", name);
	d->file = QFILENAME(QString(name));
	if(d->file.isEmpty()) {
		d->file = name;
	}
	return Simple;
}
/** lzip: does not support dirs, compress files individually
	command: lzip -9kvf gui.tar
	output:
	  gui.tar:  4.832:1,  1.655 bits/byte, 79.31% saved, 92160 in, 19071 out.

	command: lzip -kdvf gui.tar.lz
	output:
	  gui.tar.lz: done
***/
Format QLzipOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	int s=0;
	char name[256],r[8];
	if(line.contains("o")) {
		sscanf(qPrintable(line), "%s:%*s%*s%s%*s%d", name, r, &d->size);
		d->ratio = r;
		d->value += d->size = s;
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty()) { //"./a/"
				d->file = name;
		}
		return Detail;
	} else {
		sscanf(qPrintable(line), "%*s%s", name);
		d->file = QFILENAME(QString(name));
		return Simple;
	}
}
/*!
	upx -9kvf --ultra-brute -o out.upx in
	-d :decompress
	output:
					   Ultimate Packer for eXecutables
						  Copyright (C) 1996 - 2009
UPX 3.04		Markus Oberhumer, Laszlo Molnar & John Reiser   Sep 27th 2009

		File size		 Ratio	  Format	  Name
   --------------------   ------   -----------   -----------
	218389 ->	 72900   33.38%  linux/elf386   qop.upx

Packed 1 file.


*/
Format QUpxOutParser::parse(const QString& line)
{
	if(!line.contains("%"))
		return Unknow;

	Q_D(QOutParser);
	int in, out;
	char r[7], format[32], outName[32];

	sscanf(qPrintable(line), "%d%*s%d%s%s%s", &in, &out, r, format, outName);
	d->out = QLatin1String("Name: ") + QString(outName) + QLatin1String("\nFormate: ") + QString(format)+ QLatin1String("\n") \
	+ QLatin1String(size2str(in)) + QLatin1String(" -> ") + QLatin1String(size2str(out)) + QLatin1String("\nRatio: ") +QString(r);
	return Unknow;
}
/**
 gzip -d <../core.tar.gz |tar xvvf - |qop -T `tar -zt <core.tar.gz |wc -l`
	tar cvvf -  gui |gzip -9  >gui.tar.gz

tar zxvf test.tgz |qop -T `tar -zt <test.tgz |wc -l` -n

 */

//7z x -y test.7z |bin/qop -t7z -T $((`7z l test.7z |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l test.7z |sed -n '$s/\(.*\)  \(.*\)files.*/\2/p'`))
//$((files+dirs)): 7z l test.7z |sed -n '$s/\(.*\)  \(.*\)files, \(.*\) folders/$((\2+\3))/p'
// $((`7z l test.7z |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l test.7z |sed -n '$s/\(.*\)  \(.*\)files.*/\2/p'`))

Format Q7zOutParser::parse(const QString& line)
{
	Q_D(QOutParser);
	if(line.contains("Compressing") || line.contains("Extracting")) {
		char name[256];
		sscanf(qPrintable(line), "%*s%s", name);
		d->file = QFILENAME(QString(name));
		if(d->file.isEmpty())
			d->file = name;
		return Simple;
	} else if (line.contains("Ok") ||line.contains("Folders:")  ||line.contains("Files:") \
			   ||line.contains("Size:") ||line.contains("Compressed:")) {
		return End7z;
	}
	return Unknow;
}
