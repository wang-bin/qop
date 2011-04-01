#ifndef GZIPHEADER_H
#define GZIPHEADER_H

namespace Archive {
namespace Gzip {

const UInt16 kSignature = 0x8B1F;

namespace Header
{
  namespace Flags
  {
	const Byte kIsText = 1 << 0;
	const Byte kCrc = 1 << 1;
	const Byte kExtra = 1 << 2;
	const Byte kName = 1 << 3;
	const Byte kComment = 1 << 4;
  }

  namespace ExtraFlags
  {
	const Byte kMaximum = 2;
	const Byte kFastest = 4;
  }

  namespace CompressionMethod
  {
	const Byte kDeflate = 8;
  }

  namespace HostOS
  {
	enum EEnum
	{
	  kFAT = 0,
	  kAMIGA,
	  kVMS,
	  kUnix,
	  kVM_CMS,
	  kAtari,
	  kHPFS,
	  kMac,
	  kZ_System,
	  kCPM,
	  kTOPS20,
	  kNTFS,
	  kQDOS,
	  kAcorn,
	  kVFAT,
	  kMVS,
	  kBeOS,
	  kTandem,

	  kUnknown = 255
	};
  }
}
}}

#endif // GZIPHEADER_H
