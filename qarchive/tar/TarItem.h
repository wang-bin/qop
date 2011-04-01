#ifndef TARITEM_H
#define TARITEM_H
#include "TarHeader.h"
#include "Types.h"

#include <qstring.h>

namespace Archive {
namespace Tar {

struct Item
{
	QString Name;
	UInt64 Size;

	UInt32 Mode;
	UInt32 UID;
	UInt32 GID;
	UInt32 MTime;
	UInt32 DeviceMajor;
	UInt32 DeviceMinor;

	QString LinkName;
	QString User;
	QString Group;

	char Magic[8];
	char LinkFlag;
	bool DeviceMajorDefined;
	bool DeviceMinorDefined;

//ItemEx>>
	UInt64 HeaderPosition;
	unsigned LongLinkSize;
	UInt64 GetDataPosition() const { return HeaderPosition + LongLinkSize + Header::RecordSize; }
	UInt64 GetFullSize() const { return LongLinkSize + Header::RecordSize + Size; }
//<<ItemEx
	bool IsDir() const {
		switch(LinkFlag) {
		case Header::LinkFlag::Directory:
		case Header::LinkFlag::DumpDir: return true;
		case Header::LinkFlag::OldNormal:
		case Header::LinkFlag::Normal: return Name.left(1)=="/";
		}
		return false;
	}

	bool IsMagic() const {
		for (int i = 0; i < 5; i++)
			if (Magic[i] != Header::Magic::UsTar[i]) return false;
		return true;
	}

	UInt64 GetPackSize() const { return (Size + 0x1FF) & (~((UInt64)0x1FF)); }
};
}}
#endif // TARITEM_H
