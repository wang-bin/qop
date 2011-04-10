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
#ifndef QOUTPARSER_H
#define QOUTPARSER_H

#define OP_TEMPLATE 0

#include "global.h"
#include "qcounterthread.h"
#include <qobject.h>
#include <qdatetime.h>

enum Format { All,Simple, Detail, DetailWithRatio, EndZip,End7z, Error, Unknow }; //Format

#if OP_TEMPLATE
class LineFormat {
public:
	virtual ~LineFormat() {}
	char* format;
	QStringList keyword;
	QMap<QString,Format> key_result;

	void* argv1; //ptr
	void* argv2;
	void* argv3;

	virtual void mapPtr(void*,void*,void*) {}
};

template<typename T1,typename T2,typename T3> class LineFormatArc :public LineFormat {
	void mapPtr(T1 ptr1,T2 ptr2,T3 ptr3) {
			argv1=ptr1,argv2=ptr2,argv3=ptr3;
	}
};
#endif //OP_TEMPLATE

class QOutParser;
extern "C" QOutParser* getParser(const QString& type="tar");


class QOutParser :public QObject
{
	Q_OBJECT
public:
	QOutParser(uint tota_size=0);
	~QOutParser();

	void start(); //start to parse output
	void setMultiThread(bool);
	void startCounterThread(); //start a QCounterThread thread

	void setFiles(const QStringList&);
	void setCountType(CountType);
#if OP_TEMPLATE
	void setLineFormat(const QString& type);
#endif //OP_TEMPLATE
public slots:
    void setTotalSize(uint);
    void estimate();
    void terminate();

private slots:
    void slotFinished();

signals:
    void valueChanged(int);
    void textChanged(const QString&);
    void finished();
	void maximumChanged(int);

protected:
    virtual void timerEvent(QTimerEvent *);
	virtual Format parse(const char* line); //return int
	//<H1>File: %1</H>, _out=dspFormat.arg(file);
	//virtual void setDiaplayFormat(Format fmt=All,const QString& txt="");

//private:
	QString file; //file just compressed/extracted
    char line[1024]; //add char name[256], ratio[4], int s? they are frequently used in parse
    uint size, compressed, value; //outSize numbers-->value
	QString ratio; //ratio: zip
    QTime _time;
    QString _out, _extra;
	uint _elapsed, _speed; //ms
    double  _left;
    volatile uint max_value;
	QString max_str;
	Format res;
    int tid;

	QCounterThread counter;

	CountType count_type;
	bool multi_thread;

#if OP_TEMPLATE
	LineFormat *lineFmt;
	int s;
	char name[256], r[8];
#endif //OP_TEMPLATE

};

const float KInvMath=1./1000; //to speed up
inline void QOutParser::estimate()
{
	/*
	_elapsed=_time.elapsed()+1;
	//assert(_elapsed>0);
	//if(count_type==Size) _speed=value/_elapsed*1000; //>0  _speed=value/(1+_elapsed)*1000
	else _speed=value*1000/_elapsed; //>0
	*/
	_elapsed=_time.elapsed();
	_speed=value/(_elapsed*KInvMath+1);
	_left= (max_value-value)/(1+_speed);
	/*
	printf("t=%d, v=%d\n",_elapsed,_speed);
	_left= (max_value-value)*_elapsed/(value*1000+1); //a/b ~ a/(b+1)
	fflush(stdout);*/
#ifndef NO_EZX
	qApp->processEvents();
#endif //NO_EZX
}

class QTarOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class QUntarOutParser :public QOutParser
{
public:
	QUntarOutParser(uint tota_size=0);

protected:
	Format parse(const char* line);
};

class QZipOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class QUnzipOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class QUnrarOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class QLzipOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class QUpxOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

class Q7zOutParser :public QOutParser
{
protected:
	Format parse(const char* line);
};

#endif // QOUTPARSER_H
