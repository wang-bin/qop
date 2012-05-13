/******************************************************************************
    tst_zprocess_gui.cpp: description
    Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
    
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

#include "zprocess.h"
#include "qop.h"
#include <QApplication>
#include "qcounterthread.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QString arch("../test/test.tgz");
    QString in("../../qop");
    /*
    QCounterThread ct;
    ct.setFiles(QStringList() << in);
    size_t s = ct.sizeOfFiles(QStringList() << in);
    qDebug("size=%d", s);
    */
    Qop qop;
    qop.progress->show();
    qop.setArchive(arch);
    /*
    qop.parser_type = "tar";
    qDebug("size=%d", s);
    qop.parser->setCountType(QCounterThread::Size);
    qDebug("size=%d", s);
    qop.progress->setMaximum(s);
    qDebug("size=%d", s);
    qop.initParser();
    qDebug("size=%d", s);
    */
    ZProcess *z = qop.process();
    QObject::connect(z, SIGNAL(finished(int)), qApp, SLOT(quit()));
    z->setArchiveTool(ZProcess::Tar);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::TGz);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::TBz2);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::TXz);
    z->packCommand();
    z->setArchiveTool(ZProcess::T7z);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::TLz);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::TLzo);
    z->packCommand();
    z->setArchiveTool(ZProcess::TLzma);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::Zip);
    z->packCommand();
    z->unpackCommand();
    z->setArchiveTool(ZProcess::Upx);
    z->packCommand();
    z->unpackCommand();

    z->setArchiveTool(ZProcess::TGz);
    z->setOutputPath(arch);
    z->setFiles(QStringList() << in);
    qop.execute(z->packCommand());
    //z->pack();

    return app.exec();
}


