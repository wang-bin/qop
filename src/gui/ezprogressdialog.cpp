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
#include "ezprogressdialog.h"
#include "ezprogressdialog_p.h"
#include <qdatetime.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#if CONFIG_EZX
#include <ZPushButton.h>
#include <UTIL_ProgressBar.h>
#endif

/*!
  1.template<typename Button> addButton(Button btn,...)

  2.if QT3
	typedef QButton QAbstractButton

	How to deal with QList

*/
void EZProgressDialogPrivate::setupUi(EZProgressDialog *dialog)
{
	q_ptr=dialog;
	//buttons.setAutoDelete(true);  //not exists in Qt4
	Q_Q(EZProgressDialog);
#if CONFIG_QT4
	q->setMaximumWidth(qApp->desktop()->width()*4/5);
#else
	q->setMaximumWidth(qApp->desktop()->width());
#endif
	labelLayout = new QVBoxLayout();
	labelLayout->setSpacing(10);

	content = new QLabel(q);
	content->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	content->setMargin(8);
#if CONFIG_EZX
	ZApplication::setMouseMode(content,1);
	content->setFixedWidth(qApp->desktop()->width()-24);
#else
	//content->setMaximumWidth(qApp->desktop()->width()/2);
#endif //NO_EZX
	labelLayout->addWidget(content);
	labels.insert(0,content);

	bar = new UTIL_ProgressBar(q);
#if CONFIG_QT4
	//bar->setAlignment(Qt::AlignCenter);
#else
	bar->setCenterIndicator(true);
#endif //CONFIG_QT4
	labelLayout->addWidget(bar);

	buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing(0);
	QVBoxLayout* contentLayout=new QVBoxLayout(q);//static_cast<QVBoxLayout*>(q->layout());
	contentLayout->setSpacing(8);
	contentLayout->setMargin(2);
	contentLayout->addLayout(labelLayout);
	//QSpacerItem *verticalSpacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
	//contentLayout->addItem(verticalSpacer);
	contentLayout->addLayout(buttonLayout);

	time.start();
}

void EZProgressDialogPrivate::init(const QString &labelText, int value, int max)
{
	content->setText(labelText);
#if CONFIG_QT4
	bar->setMaximum(max);
	bar->setValue(value);
#else
	bar->setTotalSteps(max);
	bar->setProgress(value);
#endif //CONFIG_QT4
#if CONFIG_EZX
	this->value=value;
	//ZDEBUG("%d,%d",max,bar->QProgressBar::totalSteps());
#endif
}

/*!
	EZProgressDialog
*/


EZProgressDialog::EZProgressDialog(QWidget *parent,Qt::WFlags f) :
		EZ_BaseDialog(parent,"EZProgressDialog",ZBaseDialog::NO_MODAL,f)
#if QT_VERSION < 0x040000 || !INHERIT_PRIVATE
		,d_ptr(new EZProgressDialogPrivate)
#endif
{
#if CONFIG_QT4
	setWindowFlags(Qt::Dialog);
#endif //CONFIG_QT4
	Q_D(EZProgressDialog);
	d->setupUi(this);
	retranslateUi();
}

EZProgressDialog::EZProgressDialog(const QString &labelText,const QString &cancelButtonText,int value,int max,QWidget* parent,Qt::WFlags f)
	: EZ_BaseDialog(parent,"EZProgressDialog",ZBaseDialog::NO_MODAL,f)
#if QT_VERSION < 0x040000 || !INHERIT_PRIVATE
	,d_ptr(new EZProgressDialogPrivate)
#endif
{
#if CONFIG_EZX
	setMaximumWidth(qApp->desktop()->width()-24);
#endif //NO_EZX
	Q_D(EZProgressDialog);
	d->setupUi(this);
	d->init(labelText,value,max);

	retranslateUi();

	addButton(cancelButtonText);
	ZPushButton *b=button(0);
#if CONFIG_QT4
	b->setObjectName("CancelButton");
	b->setCheckable(true);
#else
	b->setName("CancelButton");
	b->setToggleButton(true);
#endif
#if !CONFIG_EZX
	b->setDefault(true);  //Press Enter or Space to cancel
#endif
	//connect(b,SIGNAL(clicked()),SLOT(reject()));
	connect(b,SIGNAL(clicked()),this,SIGNAL(canceled()));
	connect(b,SIGNAL(clicked()),this,SIGNAL(cancelled()));
//#if CONFIG_EZX
	connect(b,SIGNAL(clicked()),qApp,SLOT(quit())); //EZX needs this
//#endif
	startTimer(1000);
}

EZProgressDialog::~EZProgressDialog()
{
	qDebug("EZProgressDialog::~EZProgressDialog()");
#if QT_VERSION < 0x040000
	delete d_ptr;
#endif
}

void EZProgressDialog::retranslateUi()
{
	setWindowTitle(tr("EZProgressDialog"));
}

void EZProgressDialog::addButton(ZPushButton *btn,int index,int stretc,Alignment align)
{
	connect(btn,SIGNAL(clicked()),SLOT(slotButtonClicked()));
	int idx=index;

	Q_D(EZProgressDialog);
	if(index>(int)d->buttons.count()) idx=-1;
	idx=index<0 ? d->buttons.count() : index;
	d->buttonLayout->insertWidget(idx,btn,stretc,align);
	d->buttons.insert(idx,btn);
#if CONFIG_EZX
	QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); //
	sizePolicy.setHeightForWidth(btn->sizePolicy().hasHeightForWidth());
	btn->setSizePolicy(sizePolicy);
	resizeButtons();
#endif //NO_EZX
	btn->show();
}

void EZProgressDialog::addButton(const QString &text, int index,int stretc,Alignment align)
{
	addButton(new EZ_PushButton("",text,this),index,stretc,align);
}
void EZProgressDialog::setButtonText(int index, const QString &text)
{
	if(buttonsCount()<=index) return;
	button(index)->setText(text);
}

ZPushButton* EZProgressDialog::button(int index) const
{
	//Q_D(const EZProgressDialog);  //Failed in qt2
	EZProgressDialogPrivate *d=d_ptr;
	//if(index>=(int)d->buttons.count()) index=-1;
	return d->buttons.at(index % d->buttons.count());//index<0 ? d->buttons.count()+index : index); //index%d->buttons.count()
}

int EZProgressDialog::buttonsCount() const
{
	Q_D(const EZProgressDialog);
	return d->buttons.count();
}

void EZProgressDialog::addLabel(QLabel *label,int index,int stretc,Alignment align)
{
	Q_D(EZProgressDialog);
	int idx=index;
	if(index>(int)d->buttons.count()) idx=-1;
	idx=index<0 ? d->buttons.count() : index;
	d->labelLayout->insertWidget(idx,label,stretc,align);
	label->show();

	d->labels.insert(idx,label);
}

void EZProgressDialog::addLabel(const QString &text,int index,int stretc,Alignment align)
{
	addLabel(new QLabel(text,this),index,stretc,align);
}

void EZProgressDialog::setLabelText(int index, const QString &text)
{
	if(labelsCount()<=index) return;
	label(index)->setText(text);
	//qApp->processEvents(); //slow down
}

QLabel* EZProgressDialog::label(int index) const
{
	//Q_D(const EZProgressDialog); //Failed in qt2
	EZProgressDialogPrivate *d=d_ptr;
	//if(index>=(int)d->buttons.count()) index=-1;
	return d->labels.at(index % d->buttons.count());//index<0 ? d->buttons.count()+index : index);
}

int EZProgressDialog::labelsCount() const
{
	Q_D(const EZProgressDialog);
	return d->labels.count();
}

void EZProgressDialog::setAutoClose(bool ac)
{
	Q_D(EZProgressDialog);
	d->autoClose=ac;
}

void EZProgressDialog::setAutoReset(bool ar)
{
	Q_D(EZProgressDialog);
	d->autoReset=ar;
}

bool EZProgressDialog::autoClose() const
{
	Q_D(const EZProgressDialog);
	return d->autoClose;
}

bool EZProgressDialog::autoReset() const
{
	Q_D(const EZProgressDialog);
	return d->autoReset;
}

void EZProgressDialog::setBar(UTIL_ProgressBar *bar)
{
	if (!bar) return;

	Q_D(EZProgressDialog);
	delete d->bar;
	d->bar = bar;
}

void EZProgressDialog::setLabel(QLabel *label)
{
	if(!label) return;

	Q_D(EZProgressDialog);
	delete d->content;
	d->content = label;
	if (label) {
		if (label->parentWidget()!= this) {
#if CONFIG_QT4
			label->setParent(this, 0);
#else
			label->reparent(this, 0, QPoint());
#endif //CONFIG_QT4
		}
	}
}

void EZProgressDialog::setLabelFont(int index,const QFont &f)
{
	label(index)->setFont(f);
}

int EZProgressDialog::value() const
{
	Q_D(const EZProgressDialog);
#if CONFIG_QT4
	return d->bar->value();
#else
	return d->bar->QProgressBar::progress();
#endif //CONFIG_QT4
}
/*
bool EZProgressDialog::clickDo(int index, const QObject *receiver, const char *member)
{
	return QObject::connect( (QObject *)button(index), SIGNAL(clicked()),receiver, member );
}

bool EZProgressDialog::clickUndo(int index, const QObject *receiver, const char *member)
{
	return QObject::disconnect((QObject *)button(index),SIGNAL(clicked()),receiver, member);
}
*/
/*!
	public slots
*/

void EZProgressDialog::reset()
{
	Q_D(EZProgressDialog);
	d->bar->reset();
	d->time.restart();
}

void EZProgressDialog::setMaximum(int max)
{
	Q_D(EZProgressDialog);
#if CONFIG_QT4
	d->bar->setMaximum(max);
	d->bar->setValue(d->bar->value());
#else
	d->bar->setTotalSteps(max);
	d->bar->setProgress(d->value);
	//ZDEBUG("%d,%d",d->value,d->bar->totalSteps());
	d->bar->setProgress(d->bar->progress());
#endif //CONFIG_QT4
}

int EZProgressDialog::maximum() const
{
	Q_D(const EZProgressDialog);
#if CONFIG_QT4
	return d->bar->maximum();
#else
	return d->bar->totalSteps();
#endif //CONFIG_QT4
}

void EZProgressDialog::setLabelText(const QString &text)
{
	Q_D(EZProgressDialog);
	d->content->setText(text);
	if(isHidden()) {
		QString str(text);
		str.replace(QRegExp("\n"),"  \r");
		fprintf(stdout, "\r%s", qPrintable(str));
		fflush(stdout);
		return;
	}
#if CONFIG_EZX
	if(!isHidden()) adjustSize(); //for Qt2.x
	move(pos()+qApp->desktop()->rect().center()-mapToGlobal(rect().center()));
#endif
}

void EZProgressDialog::setValue(int progress)
{
	Q_D(EZProgressDialog);
#if CONFIG_EZX
	d->value=progress;
	//ZDEBUG("%d,%d",d->value,d->bar->totalSteps());
#endif
#if CONFIG_QT4
	d->bar->setValue(progress);
	int max=d->bar->maximum();
#else
	d->bar->setProgress(progress);
	int max=d->bar->totalSteps();
#endif //CONFIG_QT4
	if (progress > max) {
		if(d->autoClose) hide(); //? why not close?
		if(d->autoReset) {
			int r = progress%max;
#if CONFIG_QT4
			d->bar->setValue(r==0?max:r);
#else
			d->bar->setProgress(r==0?max:r);
#endif //CONFIG_QT4
		}
	}
}

void EZProgressDialog::removeLabel(int index)
{
	Q_D(EZProgressDialog);
#if CONFIG_QT4
	delete d->labels.takeAt(index);
#else
	delete d->labels.take(index);
#endif //CONFIG_QT4
}

void EZProgressDialog::removeButton(int index)
{
	Q_D(EZProgressDialog);
	button(index)->disconnect(this);
#if CONFIG_QT4
	delete d->buttons.takeAt(index);
#else
	delete d->buttons.take(index);
#endif //CONFIG_QT4
#if CONFIG_EZX
	resizeButtons();
#endif //CONFIG_EZX
}


//Protected (Events)
void EZProgressDialog::closeEvent(QCloseEvent *e)
{
	emit canceled();
	emit cancelled();
	QWidget::closeEvent(e);
}

void EZProgressDialog::timerEvent(QTimerEvent *)
{
//	int sec=d->time.elapsed()/1000;
#if CONFIG_EZX
	if(!isHidden()) adjustSize(); //for Qt2.x
	move(pos()+qApp->desktop()->rect().center()-mapToGlobal(rect().center()));
	//qApp->processEvents();
#endif
}

/*void EZProgressDialog::hideEvent(QHideEvent *)
{
	setFixedSize(0,0);
}*/

UTIL_ProgressBar* EZProgressDialog::bar()
{
	Q_D(const EZProgressDialog);
	return d->bar;
}

/*!
	private slots
*/
void EZProgressDialog::slotButtonClicked()
{
	ZPushButton* btn=(ZPushButton*)sender();

	Q_D(EZProgressDialog); //Q_D(const EZProgressDialog)
#if CONFIG_QT4
	int index=d->buttons.indexOf(btn);
#else
	int index=d->buttons.find(btn);
#endif //CONFIG_QT4
	emit buttonClicked(index);
}

/*!
  private
*/

void EZProgressDialog::resizeButtons()
{
	Q_D(const EZProgressDialog);
	int btns=d->buttons.count();
	int w=qMax(qApp->desktop()->width(),width());

	if(btns<=2) w-=30;
	else w-=20;
	w/=btns;
	ButtonIterator it(d->buttons);
#if CONFIG_QT4
	for(it.toFront();it.hasNext();) {
		it.next()->setFixedWidth(w); //
	}
#else
	for(it.toFirst();it.current();++it) {
		(*it)->setFixedWidth(w); //
		qDebug((*it)->text());
	}
#endif

}
