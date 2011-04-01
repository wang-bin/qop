#ifndef TARHEADER_H
#define TARHEADER_H

namespace Archive {
namespace Tar {

namespace Header {
	const unsigned int RecordSize = 512;
	const int NameSize = 100;
	const int UserNameSize = 32;
	const int GroupNameSize = 32;
	const int PrefixSize = 155;

	namespace Mode {
		const int SetUID   = 04000;  // Set UID on execution
		const int SetGID   = 02000;  // Set GID on execution
		const int SaveText = 01000;  // Save text (sticky bit)
	}

	namespace Permission {
		const int UserRead     = 00400;  // read by owner
		const int UserWrite    = 00200;  // write by owner
		const int UserExecute  = 00100;  // execute/search by owner
		const int GroupRead    = 00040;  // read by group
		const int GroupWrite   = 00020;  // write by group
		const int GroupExecute = 00010;  // execute/search by group
		const int OtherRead    = 00004;  // read by other
		const int OtherWrite   = 00002;  // write by other
		const int OtherExecute = 00001;  // execute/search by other
	}

	namespace LinkFlag {
		const char kOldNormal    = '\0'; // Normal disk file, Unix compatible
		const char kNormal       = '0'; // Normal disk file
		const char kLink         = '1'; // Link to previously dumped file
		const char kSymbolicLink = '2'; // Symbolic link
		const char kCharacter    = '3'; // Character special file
		const char kBlock        = '4'; // Block special file
		const char kDirectory    = '5'; // Directory
		const char kFIFO         = '6'; // FIFO special file
		const char kContiguous   = '7'; // Contiguous file

		const char kDumpDir       = 'D'; /* GNUTYPE_DUMPDIR.
	data: list of files created by the --incremental (-G) option
	Each file name is preceded by either
	- 'Y' (file should be in this archive)
	- 'N' (file is a directory, or is not stored in the archive.)
	Each file name is terminated by a null + an additional null after the last file name. */
	}

	//h: extern char* const var; //const ptr variable value
	//cpp: char const *var="xxx"
	//h: const char* const var="xxx" //const ptr and value
	const char * const CheckSumBlanks	= "        ";		// 8 blanks, no null
	const char * const LongLink			= "././@LongLink";
	const char * const LongLink2		= "@LongLink";

	namespace Magic {
		const char * const UsTar	= "ustar";				// 5 chars
		const char * const GNUTar	= "GNUtar ";			// 7 chars and a null
		const char * const Empty	= "\0\0\0\0\0\0\0\0";	// 7 chars and a null
	}
}
}

}
#endif // TARHEADER_H
