/******************************************************************************
	EZProgressDialog
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
#ifndef EZPROGRESSDIALOG_P_H
#define EZPROGRESSDIALOG_P_H
#include "qtcompat.h"

#include <qdatetime.h>
#include <qlabel.h>
#include <qlayout.h>
#if CONFIG_QT4
#include <QtGui/QPushButton>
#include <private/qdialog_p.h>
#else
#endif
#if CONFIG_EZX
#include <UTIL_ProgressBar.h>
#include <ZPushButton.h>
#else
#include <qprogressbar.h>
#endif

/*!
  TODO: std::list
*/

class EZProgressDialog;
class QString;

template<bool flag, typename T, typename U>
struct Select
{
	typedef T Result;
};

template <typename T, typename U>
struct Select<false, T, U>
{
	typedef U Result;
};

template<typename T, bool flag>
struct Selector
{
	typedef typename Select<flag, T*, T>::Result ValueType;
};


class EZProgressDialogPrivate
#if CONFIG_QT4 && !defined(QT_NO_QOBJECT)
	:public QDialogPrivate
#endif
{
	Q_DECLARE_PUBLIC(EZProgressDialog)
//public:
	EZProgressDialogPrivate();
	~EZProgressDialogPrivate();

	void setupUi(EZProgressDialog* dialog);
	void init(const QString &labelText, int value, int max);

	QVBoxLayout *labelLayout;
	QLabel *content;
	UTIL_ProgressBar *bar;
	QHBoxLayout *buttonLayout;
/*
#if CONFIG_QT4
	QList<QLabel*> labels;
	QList<ZPushButton*> buttons;
#else
	QList<QLabel> labels;
	QList<ZPushButton> buttons;
#endif //CONFIG_QT4
*/
	QList<Selector<QLabel, CONFIG_QT4>::ValueType> labels;
	QList<Selector<ZPushButton, CONFIG_QT4>::ValueType> buttons;
	QTime time;

#if !CONFIG_QT4 || defined(QT_NO_QOBJECT)
	EZProgressDialog *q_ptr;
#endif
	bool autoReset, autoClose;
#if CONFIG_EZX
	int value;
#endif
};

#endif // EZPROGRESSDIALOG_P_H
