/*****************************************************************************
  Crash Handler Program for Windows
	Some details redacted to prevent future students in this class from seeing
	this code and using it to cheat in the class.

  Author(s): Evan O'Bryant
  Copyright © 2023 DigiPen (USA) Corporation.    
*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Links dbghelp library without messing with the visual studio project files.
#pragma comment(lib, "dbghelp.lib")
#include <DbgHelp.h>

namespace MiniCrashHandler {
		// A list of different dump modes to output, depending on how much info you want
	enum class DumpType : char {
		DUMP_SMALL,		//MiniDumpNormal
		DUMP_PARTIAL,	//MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory
		DUMP_BIG,			//MiniDumpNormal | MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithUnloadedModules
		DUMP_MASSIVE	//MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithCodeSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithUnloadedModules | MiniDumpWithFullMemory
	};

		// Stores minidump flags to be used in the event of a crash
	static MINIDUMP_TYPE dumpInfo = MiniDumpNormal;

		// The exception filter function that will create the minidump file upon a crash
	LONG WINAPI ExceptionLogger(EXCEPTION_POINTERS* e) {
			// Get variables needed to produce the minidump
		HANDLE process = ...;
		DWORD processID = ...;
		HANDLE dumpFile = ...;
		MINIDUMP_EXCEPTION_INFORMATION mei = ... ;
		
			// Create the dump and close the non-pseudo handle (the file handle)
		MiniDumpWriteDump(process, processID, dumpFile, dumpInfo, &mei, nullptr, nullptr);
		CloseHandle(dumpFile);

			// Continue crashing
		return EXCEPTION_CONTINUE_SEARCH;
	}

		// Initialize the crash dump handler
	void Init(DumpType dS = DumpType::DUMP_SMALL) {
			// Set the minidump creation function as the crash handler
		SetUnhandledExceptionFilter(ExceptionLogger);
	
			// Set dump info flags based on the provided dump type.
			// Also gets the size that the stack should guarantee will be available to the
			// exception filter function.
		switch (dS) {
		case CONDITION:
			dumpInfo = ...;
			dumpStackSize = ...;
			break;
		}

			// Reserve a certain amount of memory on the stack. Prevents issues when dumping
			// during something like a stack overflow.
		SetThreadStackGuarantee(&dumpStackSize);
	}

} // namespace MiniCrashHandler
