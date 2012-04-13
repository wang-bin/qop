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
#define INHERIT_PRIVATE 0

#include <qdatetime.h>
#if CONFIG_QT4
#include <QLabel>
#include <QPushButton>
#	if INHERIT_PRIVATE
#include <private/qdialog_p.h>
#	endif
#else
#define INHERIT_PRIVATE 0
class QLabel;
#endif
#if CONFIG_EZX
#include <UTIL_ProgressBar.h>
#include <ZPushButton.h>
#else
#include <qprogressbar.h>
#endif
#include <qlayout.h>

class EZProgressDialog;
class QString;
//class QHBoxLayout;
//class QVBoxLayout;
//class UTIL_ProgressBar;

#if CONFIG_QT4
typedef QListIterator<ZPushButton*> ButtonIterator;
typedef QListIterator<QLabel*> LabelIterator;
#else
typedef QListIterator<ZPushButton> ButtonIterator;
typedef QListIterator<QLabel> LabelIterator;
#endif

class EZProgressDialogPrivate
#if (QT_VERSION >= 0x040000) && !defined(QT_NO_QOBJECT) && INHERIT_PRIVATE
	:public QDialogPrivate
#endif
{
	Q_DECLARE_PUBLIC(EZProgressDialog)
public:
	EZProgressDialogPrivate():labelLayout(0),content(0),bar(0)
			,buttonLayout(0),autoReset(true),autoClose(false)
	{
#if CONFIG_EZX
		value=0;
#endif
	}

	~EZProgressDialogPrivate() {
		//ButtonIterator it(buttons);
	#if CONFIG_QT4
		//for(it.toFront();it.hasNext();) {
			//delete it.next(); //
		qDeleteAll(buttons.begin(),buttons.end());
		//for(it_l.toFront();it_l.hasNext();) {
		//	delete it_l.next(); //
		qDeleteAll(labels.begin(),labels.end());
	#else
		ButtonIterator it(buttons);
		for(it.toFirst();it.current();++it) //{
			delete *it; //
		LabelIterator it_l(labels);
		for(it_l.toFirst();it_l.current();++it_l) //{
			delete *it_l; //
	#endif
		buttons.clear();
		labels.clear();
		delete labelLayout;
		delete buttonLayout;
		delete bar;
		//delete q_ptr; //Memory error!
	}


	void setupUi(EZProgressDialog* dialog);
	void init(const QString &labelText, int value, int max);

	QVBoxLayout *labelLayout;
	QLabel *content;
	UTIL_ProgressBar *bar;
	QHBoxLayout *buttonLayout;
#if QT_VERSION >= 0x040000
	QList<QLabel*> labels;
	QList<ZPushButton*> buttons;
#else
	QList<QLabel> labels;
	QList<ZPushButton> buttons;
#endif //CONFIG_QT4
	QTime time;

#if !INHERIT_PRIVATE || (QT_VERSION < 0x040000)
	EZProgressDialog *q_ptr;
#endif
	bool autoReset, autoClose;
#if CONFIG_EZX
	int value;
#endif
};

#endif // EZPROGRESSDIALOG_P_H
