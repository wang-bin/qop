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
/*!

  TODO:
	auto detect type
	-v --verbose
	stderr
	input password
  tar cvf: count the number of files, also estimate left time, speed files/s--------------X
  need 1 more timer to estimate when time out----------------------------------------------X
  display size/number left----------------------------------------------X
  consol mode

			_out="<H3><font color=#0000ff>"+file+"</font></H3><left>"+tr("Size: ")+size2Str<double>(size) \
				 +"</left><br><left>"+tr("Processed: ")+size2Str<double>(value)+" / "+max_str+"</left><br>";
			Rich text takes much more time


	Format --> TarVerbose TarQuite ... EndZip 7zEnd

	7z l archieve
	7z counts files exclude dirs----------------------------------------------X
	kill 7z

	Processed: files/files----------------------------------------------X


	template<typename T1,typename T2,typename T3>
	class OutputFormat {
	OutputFormat() {map_addr()];
	const char* fmt,
	char *keyword[16];
	T1 argv1;
	T2 argv2;
	T3 argv3;

	map_addr(void* ptr1,void* ptr2,void* ptr3);//T1, T2, T3-->&size,name,rate
	}


	//QMap<const QString& type,struct OutputFormat ofmt>

	getParser(const QString&) {
	new QOutParser
	//ref to boost.any
	setOutputFormat()
	}

	parse()

	class LineFormat {
	public:
		virtual ~LineFormat()=0;
		char* format;
		QStringList keyword, keyword_unkown;
		QString keyword_err;
	}

	template<typename T1,typename T2,typename T3>
	class LineFormatArc :public LineFormat {
		void map_addr(T1* ptr1,T2* ptr2,T3* ptr3) {
			argv1=ptr1,argv2=ptr2,argv3=ptr3;
		}
	private:
		T1 argv1; //ptr
		T2 argv2;
		T3 argv3;

	}


	class QOutParser {
	...
	private:
		char name[256], r[8];
		int s;
		LineFormat *line_fmt;
	}

	QOutParser::setLineFormat(const QString& type)
	{
		if(type=="tar") { line_fmt=new LineFormatArc<int,char*,char*>();lf.map_addr(*s,name,null);
		keyword<<""<<...;}
		...

	}

	QOutParser::parse() {
		bool kw_found=false;
		it //keyword
		for(it) {
			if(line.contains(*it) {kw_found=true;break;}
		}
		if(!kw_found) return Unknow;
		else { fprintf(line,line_fmt.format,line_fmt.argv1...);
			file=...
			...
		}
	}

	main.cpp:
		QOutParser *qop=new QOutParser;
		qop->setFormat(type);

		qop->parse()


*/
#include "QOutParser.h"
#include <qsocketnotifier.h>

#include "util.h"
QOutParser* getParser(const QString& type)
{
#if CONFIG_QT4
	QString t=type.toLower();
#else
	QString t=type.lower();
#endif // CONFIG_QT4
	if(t=="tar") {
		return new QTarOutParser;
	} else if(t=="zip") {
		return new QZipOutParser;
	} else if(t=="untar") {
		return new QUntarOutParser;
	} else if(t=="unzip") {
		return new QUnzipOutParser;
	} else if(t=="unrar") {
		return new QUnrarOutParser;
	} else if(t=="7z") {
		QOutParser *qop=new Q7zOutParser;
		qop->setCountType(NumNoDir);
		return qop;
	} else if(t=="lzip") {
		return new QLzipOutParser;
	} else if(t=="upx") {
		return new QUpxOutParser;
	} else {
		return new QOutParser;
	}
}



QOutParser::QOutParser(uint total):QObject(0),file(""),size(0),compressed(0),value(0) \
	,_out(""),_extra(tr("Calculating...")),_elapsed(0),_left(0),max_value(total),res(Unknow) \
	,count_type(Size),multi_thread(false)
{
	res_tmp=res;
	first=true;
	_time.start();
	connect(this,SIGNAL(finished()),SLOT(slotFinished()));
	connect(&counter,SIGNAL(counted(uint)),SLOT(setTotalSize(uint)));
	//connect(&counter,SIGNAL(counted(uint)),this,SIGNAL(maximumChanged(int)));
	//connect(&counter,SIGNAL(done()),SLOT());
#if 0
		QSocketNotifier *socket_notifier=new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read,this);
		ZDEBUG("SocketNotifier is enabled: %d",socket_notifier->isEnabled());
		connect(socket_notifier,SIGNAL(activated(int)),SLOT(readFromFile(int)));
		initTimer();
#endif

}

QOutParser::~QOutParser()
{
}

void QOutParser::parseLine(const char* line)
{
	//Format res_tmp=res;
	qApp->processEvents(); //remove ?
	res_tmp=parse(line);
	//_speed=value/(1+_elapsed)*1000;
	/*! 2010-11-27
				set count_type when as the first available format and switch to correct mode
				1-thread mode will perform perfectly
			*/
	if(first && res_tmp!=Error && res_tmp!=Unknow) {
		res=res_tmp;
		first=false;
		if((res_tmp==Simple && count_type==Size)||((res_tmp==Detail || res_tmp==DetailWithRatio)&&count_type!=Size)) {
			count_type= (CountType)((int)~count_type&0x1);
			setCountType(count_type);
			startCounterThread(); //no effect on multi-threading
		}
	}

	if(res==Detail) {
		_out=file+"\n"+tr("Size: ")+size2Str<double>(size)+"\n"+tr("Processed: ")+size2Str<double>(value)+max_str+"\n";
		_extra=tr("Speed: ")+size2Str<double>(_speed)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	} else if(res==Simple) {
		_out=file+"\n"+tr("Processed: ")+QString::number(++value)+max_str+tr("files")+"\n";// .arg(_elapsed/1000.,0,'f',1).arg(++value).arg(max_str);
		_extra=tr("Speed: ")+QString::number(_speed)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
		//_extra=tr("Elapsed: %1s Processed: %2%3 files").arg(_elapsed/1000.,0,'f',1).arg(++value).arg(max_str);
	} else if(res==DetailWithRatio) {
		_out=file+"\n"+tr("Size: ")+size2Str<double>(size)+"  "+tr("Ratio: ")+ratio+"\n"+tr("Processed: ")+size2Str<double>(value)+max_str+"\n";
		_extra=tr("Speed: ")+size2Str<double>(_speed)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	} else if(res==Unknow) {
		puts(line);
		fflush(stdout);
		return;//continue;
	} else if(res==Error) {
		puts(line);
		_out=tr("Password Error!");
		_extra="";
	} else {
		_out=line;
		_extra="";
	}
	emit valueChanged(value);//_value++);//inavailable lines, value not ++
	emit textChanged(_out+_extra);

}

void QOutParser::initTimer()
{
	_time.restart();
	tid=startTimer(300); //startTimer(0) error in ezx
}

void QOutParser::readFromFile(int fd)
{
	if(fd!=STDIN_FILENO) {
		ZDEBUG("Data not from stdin...");
		return;
	}
	int count = read(STDIN_FILENO, line, 1024);
	//ZDEBUG("stdout: %s",line);
	//parseLineByLine(line);
}

void QOutParser::start() {
	initTimer();
	//read(STDIN_FILENO,buf,SIZE))
	while(fgets(line,MAX,stdin)) {
		parseLine(line);
	}
	emit finished();
}


void QOutParser::setCountType(CountType ct)
{
	count_type=ct;
	counter.setCountType(count_type);
}

void QOutParser::setMultiThread(bool mt)
{
	multi_thread=mt;
}

void QOutParser::startCounterThread()
{
	emit textChanged(tr("Calculating..."));
	qApp->processEvents();
	if(multi_thread) counter.start();
	else counter.run();
}

void QOutParser::setFiles(const QStringList &f)
{
	counter.setFiles(f);
}

Format QOutParser::parse(const char *line)
{
	_out=line; //can remove
	return All;
}

void QOutParser::timerEvent(QTimerEvent *)
{
	estimate();
}

void QOutParser::slotFinished()
{
	killTimer(tid);
	estimate();
	printf("Out: %d b, Time: %.1f s, Speed: %d Kb/s",value,_elapsed/1000.,value/(1+_elapsed));
	fflush(stdout);
	//value*1000/_elapsed is not correct, why?
	if(res==Simple || res==End7z) {
		_out=tr("Finished: ")+QString("%1").arg(value)+max_str+tr("files")+"\n";
		_extra=tr("Speed: ")+QString::number(_speed)+"/s\n"+tr("Elapsed time: ")+QString("%1s").arg(_elapsed/1000.,0,'f',1);
	} else if(res==Detail) {
		_out=tr("Finished: ")+size2Str<double>(value)+max_str+"\n";
		_extra=tr("Speed: ")+size2Str<double>(value/(1+_elapsed)*1000)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	} else if(res==DetailWithRatio) {
		_out=tr("Finished: ")+size2Str<double>(value)+max_str+"\n";
		_extra=tr("Speed: ")+size2Str<double>(value/(1+_elapsed)*1000)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	} else if(res==EndZip) { //zip
		_out=tr("Finished: ")+size2Str<double>(value)+max_str+"\n"+tr("Compressed: ")+size2Str<double>(compressed)+"\n"+tr("Compression ratio: ")+ratio+"\n";
		_extra=tr("Speed: ")+size2Str<double>(value/(1+_elapsed)*1000)+"/s\n"+tr("Elapsed: %1s Remaining: %2s").arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1);
	} else if(res==Error) {
		_out=tr("Password Error!");
		_extra="";
	}
	emit textChanged(_out+_extra);
}

void QOutParser::setTotalSize(uint s)
{
	//emit textChanged(tr("Counting...")+size2Str<double>(s));
	emit maximumChanged(max_value=s);
	//qApp->processEvents();
	estimate();
	if(count_type==Size) max_str=" / "+size2Str<double>(max_value);
	else max_str=" / "+QString::number(s)+" ";
#ifndef NO_EZX
	//ZDEBUG("TS: %d time: %d",max_value,_time.elapsed());
#endif
}

#if defined(_OS_LINUX_) || defined(_OS_CYGWIN_)
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
	qDebug("signal sender: %s",
#if CONFIG_QT4
		sender()->objectName().toLocal8Bit().data()
#else
		sender()->name()
#endif
		);
#if defined(_OS_LINUX_)
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
Format QTarOutParser::parse(const char* line)
{
	int s=0;
	char name[256];
	if(QString(line).contains(" ")) {
		sscanf(line,"%*s%*s%d%*s%*s%s",&s,name);
		value+=size=s;
		file=QFILENAME(name);
		if(file.isEmpty()) { //"./a/"
			file=name;
		} return Detail;
	} else {
		//puts(line);
		file=QFILENAME(line);
		if(file.isEmpty()) {
			file=line;
		} return Simple;
	}
}



QUntarOutParser::QUntarOutParser(uint tota_size):QOutParser(tota_size)
{
	value=512*2; //2*sizeof(tar_header)
}

/*!
  FIXME: QFileInfo(name).fileName() is slow and cost to much;
*/
Format QUntarOutParser::parse(const char *line)
{
	int s=0;
	char name[256];
	if(QString(line).contains(" ")) {
		sscanf(line,"%*s%*s%d%*s%*s%s",&s,name);
		size=s;
		value+=512+ROUND512(s);//512+(s%512 ? s+512-s%512 :s); //sizeof(tar_header)+n*512: 512+s+512+(-s)%512
		file=QFILENAME(name);
		if(file.isEmpty()) { //"./a/"
			file=name;
		} return Detail;
	} else {
		//puts(line);
		file=QFILENAME(line);
		if(file.isEmpty()) {
			file=line;
		} return Simple;
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
 */
Format QZipOutParser::parse(const char* line)
{
	int s=0;
	char name[256],r[4];
	if(QString(line).contains("updating:") || (QString(line).contains("adding:"))) {
		if(QString(line).contains("in=")) {
			sscanf(line,"%*s%s\t(in=%d)%*s%*s%[^)]",name,&s,r);
			ratio=r;
			value+=size=s;
			file=QFILENAME(name);
			if(file.isEmpty()) { //"./a/"
				file=name;
			}
			return DetailWithRatio;
		}  else {
			sscanf(line,"%*s%s",name);
			file=QFILENAME(name);
			if(file.isEmpty()) {
				file=name;
			}
			return Simple;
		}
	} else if(QString(line).contains("->")) {//finished
		char in_s[32], out_s[32], r[3];
		sscanf(line,"%*[^=]=%[^,]%*[^=]=%[^ ]%*[^>]>%[^%]",in_s,out_s,r);
		value=QString(in_s).toInt();
		compressed=QString(out_s).toInt();
		ratio=QString(r)+"%";
		//ratio=QString().sprintf("%.2f",(double)compressed/(double)value*100.)+"%";
		return EndZip;
	} else if(QString(line).contains("Archive is current")) {
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


	command: unzip -ov zip.zip, do not extract
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
Format QUnzipOutParser::parse(const char* line)
{
	if(QString(line).contains("Archive")) return Unknow;
	if(QString(line).contains("--------")) return Unknow;

	if((QString(line).contains("incorrect"))) {
		return Error; //not work??
	}

	int s=0;
	char name[256],r[4];
	if(QString(line).contains("%")) {
		// change to if( extracting, inflating)
		if(!QString(line).contains(":")) return Unknow;
		sscanf(line,"%d%*s%*d%s%*s%*s%*s%s",&s,r,name);
		value+=size=s;
		file=QFILENAME(name);
		if(file.isEmpty()) { //"./a/"
				file=name;
		}
		return Detail;
	} else {
		sscanf(line,"%*s%s",name);
		file=QFILENAME(name);
		if(file.isEmpty()) {
				file=name;
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
Format QUnrarOutParser::parse(const char* line)
{
	if(QString(line).contains("UNRAR ")) return Unknow;
	if(QString(line).isEmpty()) return Unknow;

	char name[256];
	sscanf(line,"%*s%s",name);
	file=QFILENAME(name);
	if(file.isEmpty()) {
		file=name;
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
Format QLzipOutParser::parse(const char* line)
{
	int s=0;
	char name[256],r[8];
	if(QString(line).contains("o")) {
		sscanf(line,"%s:%*s%*s%s%*s%d",name,r,&size);
		ratio=r;
		value+=size=s;
		file=QFILENAME(name);
		if(file.isEmpty()) { //"./a/"
				file=name;
		}
		return Detail;
	} else {
		sscanf(line,"%*s%s",name);
		file=QFILENAME(name);
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
Format QUpxOutParser::parse(const char* line)
{
	int in, out;
	char r[7], format[32], outName[32];
	if(!QString(line).contains("%")) return Unknow;
	sscanf(line,"%d%*s%d%s%s%s",&in,&out,r,format,outName);
	_out="Name: "+QString(outName)+"\nFormate: "+QString(format)+"\n"+size2Str<double>(in)+" -> "+size2Str<double>(out) \
		 +"\nRatio: "+QString(ratio);
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

Format Q7zOutParser::parse(const char *line)
{
	if(QString(line).contains("Compressing") || QString(line).contains("Extracting")) {
		char name[256];
		sscanf(line,"%*s%s",name);
		file=QFILENAME(name);
		if(file.isEmpty())
			file=name;
		return Simple;
	} else if (QString(line).contains("Ok") ||QString(line).contains("Folders:")  ||QString(line).contains("Files:") \
			   ||QString(line).contains("Size:") ||QString(line).contains("Compressed:")) {
		return End7z;
	}
	return Unknow;
}
