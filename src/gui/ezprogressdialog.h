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
#ifndef EZPROGRESSDIALOG_H
#define EZPROGRESSDIALOG_H
/*!
  2010-08-24
  TODO:
	GridLayout
	Mutil-progressBar
	Content type: QTextEdit
*/
#include <qdialog.h>
#include "qtcompat.h"

#if !CONFIG_EZX
#define ZApplication QApplication
#include <qapplication.h>
#include <qpushbutton.h> //include it so that we can use it's apis directly.
#else
#include <ZApplication.h>
#include <ZBaseDialog.h>
#include <ZPushButton.h>
#endif //CONFIG_EZX
#include <qlabel.h>


//class QLabel;
class UTIL_ProgressBar;
class ZPushButton; //need by addButton(ZPushButton*,...) in Qt4, include <qpushbutton.h> is not enough. why?
class EZProgressDialogPrivate;

class EZProgressDialog : public ZBaseDialog {
	Q_OBJECT
	Q_DECLARE_PRIVATE(EZProgressDialog)
	//Q_PROPERTY(bool wasCanceled READ wasCanceled)
	Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
	Q_PROPERTY(int value READ value WRITE setValue)
	Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
	Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose)
	//Q_PROPERTY(int minimumDuration READ minimumDuration WRITE setMinimumDuration)
	//Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)
public:
	explicit EZProgressDialog(QWidget *parent = 0,Qt::WFlags f=0);
	EZProgressDialog(const QString& labelText,const QString& cancelButtonText="Cancel",int value=0,int max=100,QWidget* parent=0,Qt::WFlags f=0);
	~EZProgressDialog();

	void addButton(ZPushButton *button,int index=-1,int stretch=0,Alignment align=0);	//index<0, insert at the tail
	void addButton(const QString& text=0,int index=-1,int stretch=0,Alignment align=0); //addButton(,Align)
	void setButtonText(int index,const QString& text);
	//#include <ZPushButton.h> first. connecting signals to slots etc. index<0, return the last index of abs(index)
	//Don't only write ZPushButton* button(...); Thus the ZPushButton can't convert to QPushButton in connections
	//just addButton() and connect(button(0)...) one by one. connection is not dynamic. why?
	ZPushButton *button(int index=0) const;
	int buttonsCount() const;

	void addLabel(QLabel* label,int index=-1,int stretch=0,Alignment align=0);
	void addLabel(const QString& text=0,int index=-1,int stretch=0,Alignment align=0);
	void setLabelText(int index,const QString& text);
	QLabel *label(int index=0) const;
	int labelsCount() const;
	void setLabelFont(int index,const QFont& font);

	void setAutoClose(bool);
	void setAutoReset(bool);
	bool autoClose() const;
	bool autoReset() const;
	void setBar(UTIL_ProgressBar*);
	void setLabel(QLabel*); //set the content label
	int value() const;
	int maximum() const;

	//bool clickDo(int index,const QObject* receiver,const char* member=0);
	//bool clickUndo(int index,const QObject* receiver,const char* member=0);

	UTIL_ProgressBar* bar();

signals:
	void canceled(); //emit when closing
	void cancelled();
	void buttonClicked(int);

public slots:
	void reset();
	void setMaximum(int maximum);
	void retranslateUi();
	void setLabelText(const QString&);
	void setValue(int);
	void setProgress(int p) {setValue(p);}

	void removeButton(int index=-1);
	void removeLabel(int index=-1);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void timerEvent(QTimerEvent*);
	//virtual void hideEvent(QHideEvent *);

private slots:
	void slotButtonClicked();

private:
	void resizeButtons();

#if !INHERIT_PRIVATE || (QT_VERSION < 0x040000)
	EZProgressDialogPrivate *d_ptr;
#endif
};

#endif // EZPROGRESSDIALOG_H
