// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MY7ZA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MY7ZA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MY7ZA_EXPORTS
#define MY7ZA_API __declspec(dllexport)
#else
#define MY7ZA_API __declspec(dllimport)
#endif

/*************************************************************************/
/*                      使用说明

7-Zip (A) 9.20  Copyright (c) 1999-2010 Igor Pavlov  2010-11-18

Usage: 7za <command> [<switches>...] <archive_name> [<file_names>...]
[<@listfiles...>]

<Commands>
a: Add files to archive
b: Benchmark
d: Delete files from archive
e: Extract files from archive (without using directory names)
l: List contents of archive
t: Test integrity of archive
u: Update files to archive
x: eXtract files with full paths
<Switches>
-ai[r[-|0]]{@listfile|!wildcard}: Include archives
-ax[r[-|0]]{@listfile|!wildcard}: eXclude archives
-bd: Disable percentage indicator
-i[r[-|0]]{@listfile|!wildcard}: Include filenames
-m{Parameters}: set compression Method
-o{Directory}: set Output directory
-p{Password}: set Password
-r[-|0]: Recurse subdirectories
-scs{UTF-8 | WIN | DOS}: set charset for list files
-sfx[{name}]: Create SFX archive
-si[{name}]: read data from stdin
-slt: show technical information for l (List) command
-so: write data to stdout
-ssc[-]: set sensitive case mode
-ssw: compress shared files
-t{Type}: Set type of archive
-u[-][p#][q#][r#][x#][y#][z#][!newArchiveName]: Update options
-v{Size}[b|k|m|g]: Create volumes
-w[{path}]: assign Work directory. Empty path means a temporary directory
-x[r[-|0]]]{@listfile|!wildcard}: eXclude filenames
-y: assume Yes on all queries

/*************************************************************************/

namespace N7ZAExitCode {
	enum EEnum {

		kSuccess       = 0,     // Successful operation
		kWarning       = 1,     // Non fatal error(s) occurred
		kFatalError    = 2,     // A fatal error occurred
		// kCRCError      = 3,     // A CRC error occurred when unpacking
		// kLockedArchive = 4,     // Attempt to modify an archive previously locked
		// kWriteError    = 5,     // Write to disk error
		// kOpenError     = 6,     // Open file error
		kUserError     = 7,     // Command line option error
		kMemoryError   = 8,     // Not enough memory for operation
		// kCreateFileError = 9,     // Create file error

		kUserBreak     = 255,   // User stopped the process
	};
}

EXTERN_C MY7ZA_API int __cdecl X7za(const wchar_t* lpszCommandLine);
typedef int (__cdecl* FunX7za)(const wchar_t* lpszCommandLine);
