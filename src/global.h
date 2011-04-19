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
#ifndef GLOBAL_H
#define GLOBAL_H
#ifndef __cplusplus
#error "Use C++ Compiler Please..."
#endif

#include <qglobal.h>

#if defined(linux) || defined(__linux) || defined(__linux__)
#define _OS_LINUX_
#elif defined(__CYGWIN__)
#define _OS_CYGWIN_
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define _OS_WIN32_
#elif defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__)
#define _OS_MSDOS_
#endif

#include <qglobal.h>
#if QT_VERSION >= 0x040000
#	define CONFIG_QT4 1
#	if CONFIG_EZX
#		define CONFIG_EZX 0
#	endif
#	define EZQT 1
#elif QT_VERSION >= 0x030000
#	define CONFIG_QT3 1
#elif QT_VERSION >= 235
#	define CONFIG_QT2 1
#   ifndef CONFIG_EZX
#	define CONFIG_EZX 1
#   endif
#else
#	error Qt 2, Qt 3 or Qt 4 is required!
#endif

#if CONFIG_QT4
#include <Qt>
#include <QtGui/QApplication>
#include <QColorGroup>
#include <QDebug>
#include <QtGui/QProgressDialog>
#include <QtGui/QMessageBox>
#include <QDesktopWidget> //QApplication::desktop()
#include <QMouseEvent>
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
#endif //EZXT_QT4


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
#   define ZBaseDialog QWidget
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
#   define EZ_ListViewItem(parent,afterOrLabel,x,labels...) Q3ListViewItem(parent,afterOrLabel,## labels)
#   define EZ_UPushButton(act_pix,nor_pix,text,parent,name,w,h) UTIL_PushButton(parent)
#   define EZ_PushButton(sid_icon,text,parent,...) ZPushButton(text,parent)
#ifndef QT3_SUPPORT
#   define insertStringList(strList) addItems(strList)
#   define setMaxValue(max) setMaximum(max)
#   define addMultiCellWidget(widget,fr,tor,fc,toc,align...) addWidget(widget,fr,fc,tor-fr+1,toc-fc+1,## align)
#   define addMultiCell(item,fr,tor,fc,toc,align...) addItem(item,fr,fc,tor-fr+1,toc-fc+1,## align)
#endif //QT3_SUPPORT
#endif //NO_EZX


#include <stdio.h>
#include <unistd.h>
//#include <assert.h>

#define ZDEBUG(fmt,args...) qDebug("[%s] %s @%d: \t"fmt,__FILE__,__PRETTY_FUNCTION__,__LINE__,## args)
//#define ZDEBUG(fmt,args...) fprintf(stdout,"[%s] %s @%d: \t"fmt" \t---%s, %s\n",__FILE__,__PRETTY_FUNCTION__,__LINE__,## args,__TIME__,__DATE__); fflush(stdout)

#if !CONFIG_QT4
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
#define Q_DECLARE_PRIVATE(Class) \
	inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_ptr); } \
	inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr); }
//	friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
	inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Dptr); } \
	inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Dptr); }
//	friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)									\
	inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
	inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
	friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()
#endif

#if CONFIG_QT4
#define QFILENAME(path) QString(path).mid(QString(path).lastIndexOf('/')+1)
#define qstr2cstr(qstr) qstr.toLocal8Bit().constData()
#else
#define QFILENAME(path) QString(path).mid(QString(path).findRev('/')+1)
#define qstr2cstr(qstr) qstr.local8Bit().data()
#endif
/*
inline
const char* qstr2cstr(const QString& qstr){
#if CONFIG_QT4
	return qstr.toLocal8Bit().constData();
#else
	return qstr.local8Bit().data();
#endif
}
*/


#endif // GLOBAL_H
