#ifndef QOP_H
#define QOP_H

#include "QOutParser.h"
#include "qarchive/arcreader.h"
#include "option.h"
#include "util.h"
#include "qarchive/qarchive.h"
#include "qarchive/tar/qtar.h"
#ifndef EZPROGRESS
#define EZPROGRESS
#endif
#ifdef EZPROGRESS
#include "gui/ezprogressdialog.h"
#endif

class QProcess;
class Qop
{
public:
    Qop();

    void setBuildinMethod(bool);
    void setArchive(const QString& archive_path);

//union!!
/*!
    for build-in method
*/
    Archive::QArchive* archive;

/*!
    for tools in PATH with qop executable. In Qt2.x, copy QProcess
*/
    QOutParser *parser;
    QProcess *process;

/*!
    common gui for all
*/
#ifndef EZPROGRESS
	UTIL_ProgressDialog *progress;
#else
	EZProgressDialog *progress;
#endif //EZPROGRESS

	void initArchive();
private:
    void initGui();

    bool buildin;
    QString arc_path;
};

#endif // QOP_H
