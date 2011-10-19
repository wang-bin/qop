/******************************************************************************
	Macros for compatibility
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
#ifndef QTCOMPAT_H
#define QTCOMPAT_H
#ifndef __cplusplus
#error "Use C++ Compiler Please..."
#endif
/* Change to qtcompatible.h*/
#include <qglobal.h>

#if QT_VERSION < 0x030000
#if defined(_OS_LINUX_)
#  define Q_OS_LINUX
#elif defined(_OS_CYGWIN_)
#  define Q_OS_CYGWIN
#elif defined(_OS_WIN32_)
#  define Q_OS_WIN32
#elif defined(_OS_MSDOS_)
#  define Q_OS_MSDOS
#elif defined(_OS_MAC_)
#  define Q_OS_DARWIN
#  define Q_OS_BSD4
#  define Q_OS_MAC
#elif defined(_OS_SOLARIS_)
#  define Q_OS_SOLARIS
#elif defined(_OS_OS2EMX_)
#  define Q_OS_OS2EMX
#elif defined(_OS_OS2_)
#  define Q_OS_OS2
#elif defined(_OS_HPUX_)
#  define Q_OS_HPUX
#elif defined(_OS_ULTRIX_)
#  define Q_OS_ULTRIX
#elif defined(_OS_RELIANT_)
#  define Q_OS_RELIANT
#elif defined(_OS_FREEBSD_)
#  define Q_OS_FREEBSD
#elif defined(_OS_NETBSD_)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(_OS_OPENBSD_)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(_OS_IRIX_)
#  define Q_OS_IRIX
#elif defined(_OS_OSF_)
#  define Q_OS_OSF
#elif defined(_OS_BSDI_)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(_OS_AIX_)
#  define Q_OS_AIX
#elif defined(_OS_LYNXOS_)
#  define Q_OS_LYNXOS
#elif defined(_OS_UNIXWARE_) || defined(_OS_UNIXWARE7_)
#  define Q_OS_UNIXWARE
#elif defined(_OS_GNU_)
#  define Q_OS_HURD
#elif defined(_OS_DGUX_)
#  define Q_OS_DGUX
#elif defined(_OS_QNX_) || defined(_OS_QNX6_)
#  define Q_OS_QNX
#elif defined(_OS_SCO_)
#  define Q_OS_SCO
#elif defined(_OS_DYNIX_)
#  define Q_OS_DYNIX
#elif defined(_OS_UNIX_)
#  define Q_OS_UNIX
#elif defined(VXWORKS) /* there is no "real" VxWorks define - this has to be set in the mkspec! */
#  define Q_OS_VXWORKS
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WINCE)
#  define Q_OS_WIN
#endif

#if defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif
#endif //QT_VERSION < 0x030000

#if QT_VERSION >= 0x040600
//QStringBuilder
#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS
#endif

#include <qglobal.h>
#if QT_VERSION >= 0x040000
#	define CONFIG_QT4 1
#	if CONFIG_EZX
#		undef CONFIG_EZX
#		define CONFIG_EZX 0
#	endif
#elif QT_VERSION >= 0x030000
#	define CONFIG_QT4 0
#	define CONFIG_QT3 1
#	if CONFIG_EZX
#		undef CONFIG_EZX
#		define CONFIG_EZX 0
#	endif
#elif QT_VERSION >= 235
#	define CONFIG_QT4 0
#	define CONFIG_QT3 0
#	define CONFIG_QT2 1
#   ifndef CONFIG_EZX
#	define CONFIG_EZX 1
#   endif
#else
#	error Qt 2, Qt 3 or Qt 4 is required!
#endif


/*
class A;
typedef A B;
*/
#if CONFIG_QT4
#include <Qt>
#include <QtGui/QApplication>
#include <QColorGroup>
#include <QDebug>
//#include <QtGui/QProgressDialog>
//#include <QtGui/QMessageBox>
#include <QDesktopWidget> //QApplication::desktop()
#include <QMouseEvent>
//class QMessageBox;
//class QProgressDialog;
using Qt::WindowFlags;
using Qt::Alignment;
#define Q_EXPORT
#   define ezDebug(s) qDebug()<<s
#define EZ_ProgressDialog(labeltext,cancelbutton,min,max,parent,name,modal,flag) UTIL_ProgressDialog(labeltext,cancelbutton,min,max,parent,flag)
#define ZMessageBox QMessageBox
#define timerInformation(parent,pix_title,text,time,ok) information(parent,"",text)
#else
#include <qlist.h>
//#define WindowFlags WFlags
#include <qapplication.h>
#define ezDebug(s) qDebug(s)
#define qMax(a,b) QMAX(a,b)
#define qMin(a,b) QMIN(a,b)
#if !CONFIG_EZX
#	include <qprogressdialog.h>
#define timerInformation(parent,pix_title,text,time,ok) information(parent,"",text)
# else
#	include <UTIL_ProgressDialog.h>
#	include <ZApplication.h>
#	include <ZMessageBox.h>
# endif //CONFIG_EZX
#define EZ_ProgressDialog(labeltext,cancelbutton,min,max,parent,name,modal,flag) UTIL_ProgressDialog(labeltext,cancelbutton,max,parent,name,modal,flag)
//#define setValue(v) setProgress(v)
typedef int Alignment;
#define setWindowTitle(s) setCaption(s)
#endif //CONFIG_QT4


#if CONFIG_EZX
#   define EZ_BaseDialog(parent,name,modal,flag) ZBaseDialog(parent,name,modal,flag)
#   define EZ_Dialog(dt,hastitle,parent,name,modal,flag) UTIL_Dialog(dt,hastitle,parent,name,modal,flag)
#   define EZ_ListView(parent,args...) QListView(parent,##args)
//#   define EZ_ListViewItem(parent,x) Q3ListViewItem(parent,x)
#   define EZ_ListViewItem(parent,afterOrLabel,x,labels...) Q3ListViewItem(parent,afterOrLabel,x,## labels)
#   define EZ_UPushButton(act_pix,nor_pix,text,parent,flag,w,h) UTIL_PushButton(act_pix,nor_pix,text,parent,flag,w,h)
#   define EZ_PushButton(sid_icon,text,parent,flw...) ZPushButton(sid_icon,text,parent,## flw)
//#define addWidget(item,fr,fc,rspan,cspan,align) addMultiCellWidget(item,fr,fc,fr+rspan-1,fc+cspan-1,align)
//confilct
//#   define addItem(item,fr,fc,rspan,cspan,align...) addMultiCell(item,fr,fr+rspan-1,fc,fc+cspan-1,## align)
//#   define addLayout(layout,fr,fc,rspan,cspan,align...) addMultiCellLayout(layout,fr,fr+rspan-1,fc,fc+cspan-1,## align)
#else
#   define ZApplication QApplication
#   define ZBaseDialog QDialog
#   define UTIL_Dialog QDialog
#   define MyQListViewItem Q3ListViewItem
#   define UTIL_ProgressBar QProgressBar
#   define UTIL_ProgressDialog QProgressDialog
#   define UTIL_PushButton QToolButton
#	define ZMessageBox QMessageBox
#   define ZPushButton QPushButton
#   define EZ_BaseDialog(parent,name,modal,flag) ZBaseDialog(parent,flag)
#   define EZ_Dialog(dt,hastitle,parent,name,modal,flag) UTIL_Dialog(parent,flag)
#   define EZ_ListView(parent,...) Q3ListView(parent)
//#   define EZ_ListViewItem(parent,x) Q3ListViewItem(parent)
#   define EZ_ListViewItem(parent,afterOrLabel,x,/*labels*/...) Q3ListViewItem(parent,afterOrLabel,##__VA_ARGS__)
#   define EZ_UPushButton(act_pix,nor_pix,text,parent,name,w,h) UTIL_PushButton(parent)
#   define EZ_PushButton(sid_icon,text,parent,...) ZPushButton(text,parent)
#ifndef QT3_SUPPORT
#   define insertStringList(strList) addItems(strList)
#   define setMaxValue(max) setMaximum(max)
#   define addMultiCellWidget(widget,fr,tor,fc,toc,/*align*/...) addWidget(widget,fr,fc,tor-fr+1,toc-fc+1,##__VA_ARGS__)
#   define addMultiCell(item,fr,tor,fc,toc,/*align*/...) addItem(item,fr,fc,tor-fr+1,toc-fc+1,##__VA_ARGS__)
#endif //QT3_SUPPORT
#endif //NO_EZX

#include <stdio.h>
#if __GNUC__
#else
#define __PRETTY_FUNCTION__ __FUNCTION__
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif //__GNUC__

//use stdarg.h, see apue.h
#define CONFIG_DEBUG 1
#if CONFIG_DEBUG
#define ZDEBUG(fmt, ...) qDebug("[%s] %s @%d: \t"#fmt, __FILE__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define ZDEBUG(fmt,args...) fprintf(stdout,"[%s] %s @%d: \t"fmt" \t---%s, %s\n",__FILE__,__PRETTY_FUNCTION__,__LINE__,## args,__TIME__,__DATE__); fflush(stdout)
#else
#define ZDEBUG(fmt, ...)
#endif //CONFIG_DEBUG


#if CONFIG_QT2
#define ENDSWITH(str, substr) (str.right(QString(substr).length())==substr)
#else
#define ENDSWITH(str, substr) str.endsWith(substr)
#endif //CONFIG_QT2

#if !CONFIG_QT4

//speed of string comparing with const char* str: QLatin1String(str) > str > QString(str) //str: possibly hidden malloc, QString(str): temp QString
#define QLatin1String(str) (str)
typedef QString QLatin1String;
#define indexOf(s) find(s)

//#define qPrintable(qstr) qstr.local8Bit().data()
inline const char* qPrintable(const QString& qstr)
{
	return qstr.local8Bit().data();
}


template <typename ForwardIterator> void qDeleteAll(ForwardIterator begin, ForwardIterator end)
{
	while (begin != end) {
		delete *begin;
		++begin;
	}
}

template <typename Container> inline void qDeleteAll(const Container &c)
{
	qDeleteAll(c.begin(), c.end());
}


//template <typename T> static inline T *qGetPtrHelper(T *ptr) { return ptr; } // \//return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr));
//protected: ?
#define Q_DECLARE_PRIVATE(Class) \
	inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_ptr); } \
	inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr); } \
	friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
	inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Dptr); } \
	inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Dptr); } \
	friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)									\
	inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
	inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
	friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()
#endif

#if CONFIG_QT4
#define QFILENAME(path) path.mid(path.lastIndexOf('/')+1)
#else
#define QFILENAME(path) path.mid(path.findRev('/')+1)
#endif //CONFIG_QT4

#endif // QTCOMPAT_H
