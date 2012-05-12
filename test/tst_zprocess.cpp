#include "zprocess.h"
#include <QtCore/QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    ZProcess z;
    z.setArchiveTool(ZProcess::Tar);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::TGz);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::TBz2);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::TXz);
    z.packCommand();
    z.setArchiveTool(ZProcess::T7z);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::TLz);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::TLzo);
    z.packCommand();
    z.setArchiveTool(ZProcess::TLzma);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::Zip);
    z.packCommand();
    z.unpackCommand();
    z.setArchiveTool(ZProcess::Upx);

    z.packCommand();
    z.unpackCommand();

    return 0;
}
