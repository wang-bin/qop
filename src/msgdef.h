#ifndef SMGDEF_H
#define SMGDEF_H

#include <algorithm>
#include <qobject.h>
#include <qstring.h>

//replace '/' in filename with "/\n", or append().append()...file.replace(file.lastIndexOf('/'), if end with '/'?
static QString g_size_tr;
static QString g_processed_tr;
static QString g_speed_tr;
static QString g_ratio_tr;
static QString g_elapsed_remain_tr;
static QString g_files_tr;
static int g_align_length;

static void initTranslations() {
	g_size_tr = QObject::tr("Size: ");
	g_processed_tr = QObject::tr("Processed: ");
	g_speed_tr = QObject::tr("Speed: ");
	g_ratio_tr = QObject::tr("Ratio: ");
	g_elapsed_remain_tr = QObject::tr("Elapsed: %1s Remaining: %2s");
	g_files_tr = QObject::tr("files");
	g_align_length = -(std::max(g_size_tr.length(), g_processed_tr.length()));
}



#if QT_VERSION >= 0x040600
//QStringBuilder
#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#define g_BaseMsg_Detail(file, size, processed, max_str) \
	file + QLatin1String("\n") + g_size_tr + QString(size2str(size)) + QLatin1String("\n") + g_processed_tr + QString(size2str(processed)) + max_str + QLatin1String("\n")
//	QString("%1\n%2%3\n%4%5%6\n").arg(file).arg(g_size_tr, g_align_length).arg(size2str(size)).arg(g_processed_tr, g_align_length).arg(size2str(processed)).arg(max)

#define g_ExtraMsg_Detail(speed, elapsed, left) \
	g_speed_tr + QString(size2str(speed)) + QLatin1String("/s\n") + g_elapsed_remain_tr.arg(g_time_convert(elapsed)).arg(g_time_convert(left*1000))
//g_speed_tr + QLatin1String(size2str(speed)) + QLatin1String("/s\n") + g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1)
//QString("%1%2/s\n%3").arg(g_speed_tr).arg(size2str(speed)).arg(g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

//slower?
#define g_BaseMsg_Simple(file, value, max_str) \
	file + QLatin1String("\n") + g_processed_tr + QString::number(value) + max_str + g_files_tr + QLatin1String("\n")
//QString("%1\n%2%3%4\n").arg(file).arg(g_processed_tr).arg(value).arg(max_str).arg(g_files_tr)

#define g_ExtraMsg_Simple(speed, elapsed, left) \
	g_speed_tr + QString::number(speed) + QLatin1String("/s\n") + g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1)
//QString("%1%2/s\n%3").arg(g_speed_tr).arg(speed).arg(g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

#define g_BaseMsg_Ratio(file, size, ratio, processed, max_str) \
	file + QLatin1String("\n") + g_size_tr + QLatin1String(size2str(size)) + g_ratio_tr + ratio + QLatin1String("\n") + g_processed_tr + QLatin1String(size2str(processed)) + max_str + QLatin1String("\n")
//QString("%1\n%2%3 %4%5\n%6%7%8\n").arg(file).arg(g_size_tr, g_align_length).arg(size2str(size)).arg(g_ratio_tr).arg(ratio).arg(g_processed_tr, g_align_length).arg(size2str(processed)).arg(max)

#define g_ExtraMsg_Ratio(speed, elapsed, left) \
	g_ExtraMsg_Detail(speed, elapsed, left)
#else

//QString().sprintf(); alignment; %-*s: max len of g_xxx_tr
#define g_BaseMsg_Detail(file, size, processed, max_str) \
	QString("%1\n%2%3\n%4%5%6\n").arg(file).arg(g_size_tr, g_align_length).arg(size2str(size)).arg(g_processed_tr, g_align_length).arg(size2str(processed)).arg(max_str)
//QString().sprintf("%s\n%-6s%s\n%-6s%s%s", qPrintable(file), qPrintable(g_size_tr), size2str(size), qPrintable(g_processed_tr), size2str(processed), qPrintable(max))
//	QString("%1\n%2%3\n%4%5%6\n").arg(file, g_size_tr, size2str(size), g_processed_tr, size2str(processed), max)
//QObject::tr("%1\nSize:%2%3\n%4%5%6\n")
#define g_ExtraMsg_Detail(speed, elapsed, left) \
		QString("%1%2/s\n%3").arg(g_speed_tr).arg(size2str(speed)).arg(g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))
//	QString("%1%2/s\n%3").arg(g_speed_tr, size2str(speed), g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

//slower?
#define g_BaseMsg_Simple(file, value, max_str) \
	QString("%1\n%2%3%4\n").arg(file).arg(g_processed_tr).arg(value).arg(max_str).arg(g_files_tr)
//	QString("%1\n%2%3%4\n").arg(file, g_processed_tr, QString::number(value), max_str, g_files_tr)
#define g_ExtraMsg_Simple(speed, elapsed, left) \
	QString("%1%2/s\n%3").arg(g_speed_tr).arg(speed).arg(g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))
//	QString("%1%2/s\n%3").arg(g_speed_tr, QString::number(speed), g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

#define g_BaseMsg_Ratio(file, size, ratio, processed, max_str) \
	QString("%1\n%2%3 %4%5\n%6%7%8\n").arg(file).arg(g_size_tr, g_align_length).arg(size2str(size)).arg(g_ratio_tr).arg(ratio).arg(g_processed_tr, g_align_length).arg(size2str(processed)).arg(max_str)
//	QString("%1\n%2%3 %4%5\n%6%7%8\n").arg(file, g_size_tr, size2str(size), g_ratio_tr, ratio, g_processed_tr, size2str(processed), max)

#define g_ExtraMsg_Ratio(speed, elapsed, left) \
	g_ExtraMsg_Detail(speed, elapsed, left)
//QString("%1%2/s\n%3").arg(g_speed_tr, size2str(speed), g_elapsed_remain_tr.arg(elapsed/1000.,0,'f',1).arg(left,0,'f',1))

#endif //QT4.6.0

#endif // SMGDEF_H
