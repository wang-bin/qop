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
#define LINE_LENGTH_MAX 1024
#define NO_SOCKET 1

#include "qtcompat.h"
#include <qobject.h>
#include <qdatetime.h>
#include "qcounterthread.h"

typedef enum {
	All,Simple, Detail, DetailWithRatio, EndZip,End7z, Error, Unknow
} Format;

class QOutParser;
extern "C" QOutParser* getParser(const QString& type="tar");

class QOutParserPrivate;
class QOutParser :public QObject
{
	Q_OBJECT
public:
	QOutParser();
	~QOutParser();

	void start(); //start to parse output
	void parseLine(const QString& line);
	void startCounterThread(); //start a QCounterThread thread

	void setInterval(unsigned int interval);
	void setUpdateMsgOnChange(bool on);
	void setMultiThread(bool multiThread);
	void setFiles(const QStringList&);
	void setCountType(QCounterThread::CountType);
	void setRecount(bool);

public slots:
	void initTimer();
	void setTotalSize(int);
	void terminate();

private slots:
	void slotFinished();
	void slotResetUnit();
#if !NO_SOCKET
	//void readFromSocket(int);
#endif
signals:
	void valueChanged(int);
	void textChanged(const QString&);
	void finished();
	void maximumChanged(int);
	void unitChanged();

protected:
	virtual void timerEvent(QTimerEvent *);
	virtual Format parse(const QString& line); //return int
	//<H1>File: %1</H>, _out=dspFormat.arg(file);
	//virtual void setDiaplayFormat(Format fmt=All,const QString& txt="");

	Q_DECLARE_PRIVATE(QOutParser)
	QOutParserPrivate *d_ptr;

};


#define Q_DECLARE_OUTPARSER(T) \
	class Q##T##OutParser :public QOutParser \
	{ \
		protected: Format parse(const QString& line); \
	};

//NO Q_OBJECT?

Q_DECLARE_OUTPARSER(Tar)
//Q_DECLARE_OUTPARSER(Untar)
Q_DECLARE_OUTPARSER(Zip)
Q_DECLARE_OUTPARSER(Unzip)
Q_DECLARE_OUTPARSER(Unrar)
Q_DECLARE_OUTPARSER(Lzip)
Q_DECLARE_OUTPARSER(Upx)
Q_DECLARE_OUTPARSER(7z)

class QUntarOutParser :public QOutParser
{
public:
	QUntarOutParser();

protected:
	Format parse(const QString& line);
};

#endif // QOUTPARSER_H
