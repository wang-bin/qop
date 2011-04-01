#ifndef GZIPITEM_H
#define GZIPITEM_H

#include "Types.h"
#include "GzipHeader.h"
#include <qstring.h>

namespace Archive {
namespace Gzip {

const char * const kHostOSes[] =
{
  "FAT",
  "AMIGA",
  "VMS",
  "Unix",
  "VM/CMS",
  "Atari",
  "HPFS",
  "Macintosh",
  "Z-System",
  "CP/M",
  "TOPS-20",
  "NTFS",
  "SMS/QDOS",
  "Acorn",
  "VFAT",
  "MVS",
  "BeOS",
  "Tandem",
  "OS/400",
  "OS/X"
};

const char* const kUnknownOS = "Unknown";

class Item
{
	bool TestFlag(Byte flag) const { return (Flags & flag) != 0; }
public:
	Byte Method;
	Byte Flags;
	Byte ExtraFlags;
	Byte HostOS;
	UInt32 Time;
	UInt32 Crc;
	UInt32 Size32;

	QString Name;
	QString Comment;
	// CByteBuffer Extra;

	// bool IsText() const { return TestFlag(NHeader::NFlags::kIsText); }
	bool HeaderCrcIsPresent() const { return TestFlag(Header::Flags::kCrc); }
	bool ExtraFieldIsPresent() const { return TestFlag(Header::Flags::kExtra); }
	bool NameIsPresent() const { return TestFlag(Header::Flags::kName); }
	bool CommentIsPresent() const { return TestFlag(Header::Flags::kComment); }

	void Clear()
	{
		Name.isEmpty();
		Comment.isEmpty();
	// Extra.SetCapacity(0);
	}
/*
	HRESULT ReadHeader(NCompress::NDeflate::NDecoder::CCOMCoder *stream);
	HRESULT ReadFooter1(NCompress::NDeflate::NDecoder::CCOMCoder *stream);
	HRESULT ReadFooter2(ISequentialInStream *stream);

	HRESULT WriteHeader(ISequentialOutStream *stream);
	HRESULT WriteFooter(ISequentialOutStream *stream);
*/
};
}}

#endif // GZIPITEM_H
