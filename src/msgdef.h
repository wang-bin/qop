#ifndef SMGDEF_H
#define SMGDEF_H

#include <qobject.h>
#include <qstring.h>

//replace '/' in filename with "/\n", or append().append()...file.replace(file.lastIndexOf('/'), if end with '/'?
static QString g_size_tr;
static QString g_processed_tr;
static QString g_speed_tr;
static QString g_ratio_tr;
static QString g_elapsed_remain_tr;
static QString g_files_tr;

static void initTranslations() {
	g_size_tr = QObject::tr("Size: ");
	g_processed_tr = QObject::tr("Processed: ");
	g_speed_tr = QObject::tr("Speed: ");
	g_ratio_tr = QObject::tr("Ratio: ");
	g_elapsed_remain_tr = QObject::tr("Elapsed: %1s Remaining: %2s");
	g_files_tr = QObject::tr("files");
}

#define g_BaseMsg_Detail(file, size, processed, max) \
	QString("%1\n%2%3\n%4%5%6\n").arg(file, g_size_tr, size2str(size), g_processed_tr, size2str(processed), max)
//QObject::tr("%1\nSize:%2%3\n%4%5%6\n")
#define g_ExtraMsg_Detail(speed, elapsed, left) \
	QString("%1%2/s\n%3").arg(g_speed_tr, size2str(speed), g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

//slower?
#define g_BaseMsg_Simple(file, value, max) \
	QString("%1\n%2%3%4\n").arg(file, g_processed_tr, QString::number(value), max_str, g_files_tr)
#define g_ExtraMsg_Simple(speed, elapsed, left) \
	QString("%1%2/s\n%3").arg(g_speed_tr, QString::number(speed), g_elapsed_remain_tr.arg(_elapsed/1000.,0,'f',1).arg(_left,0,'f',1))

#define g_BaseMsg_Zip(file, size, ratio, processed, max) \
	QString("%1\n%2%3 %4%5\n%6%7%8\n").arg(file, g_size_tr, size2str(size), g_ratio_tr, ratio, g_processed_tr, size2str(processed), max)

#define g_ExtraMsg_Zip(speed, elapsed, left) \
	g_ExtraMsg_Detail(speed, elapsed, left)
//QString("%1%2/s\n%3").arg(g_speed_tr, size2str(speed), g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

#endif // SMGDEF_H
