/******************************************************************************
	Name: description
	Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>

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

#include "qt_util.h"

#define SLEEP_QTIME 0
#define SLEEP_WAITCONDITION 1
#define  SLEEP_EVENTLOOP 0
#if QT_VERSION <= 0x040000
#define SLEEP_EVENTLOOP 0
#endif



#include "qtcompat.h"
#if QT_VERSION >= 0x040700
#include <QtCore/QElapsedTimer>
#else
#include <qdatetime.h>
typedef QTime QElapsedTimer;
#endif

#if SLEEP_QTIME
#include <qdatetime.h>
#elif SLEEP_EVENTLOOP
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#elif SLEEP_WAITCONDITION
#if CONFIG_QT2
#include <qthread.h>
#else
#include <qwaitcondition.h>
#include <qmutex.h>
#endif //CONFIG_QT2
static QMutex mutex;
#else
#ifdef Q_OS_WIN
#include <windows.h>  //compile error in mingw
#endif //Q_OS_WIN
#endif //SLEEP_QTIME




namespace QT_UTIL {

//sleep和usleep都已经obsolete，建议使用nanosleep代替
void qSleep(int ms)
{
#if SLEEP_QTIME
	QTime dieTime = QTime::currentTime().addMSecs(ms);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#elif SLEEP_EVENTLOOP
	QEventLoop eventloop;
	QTimer::singleShot(ms, &eventloop, SLOT(quit()));
	eventloop.exec();
#elif SLEEP_WAITCONDITION
	mutex.lock();
	static QWaitCondition wait;
	wait.wait(&mutex,ms);
	mutex.unlock();
#else
#ifdef Q_OS_WIN
	Sleep(uint(ms));
#else
	struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
	nanosleep(&ts, NULL);
#endif //Q_OS_WIN
#endif //SLEEP_QTIME

}

//inline static
void qWait(int ms)
{
	QElapsedTimer timer;
	timer.start();
	do {
#if CONFIG_QT4
		//解决界面无法刷新的问题
		QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
#else
		qApp->processEvents(ms);
#endif
		qSleep(10); //解决程序CPU占用率过高的问题
	} while (timer.elapsed() < ms);
}

} //nameespace QT_UTIL
