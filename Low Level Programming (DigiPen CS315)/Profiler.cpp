/*****************************************************************************
	A sampler profiler for Windows. Cross platform support exists, but only
	Windows has an implementation.
	Some details redacted to prevent future students in this class from seeing
	this code and using it to cheat in the class. Some syntax is purposefully
	incorrect.

	Author(s): Evan O'Bryant
	Copyright © 2022 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <atomic>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

namespace SProfiler {
	// Holds data the profiler thread needs to store it's data
static struct ProfilerData {
		// The thread itself so it doesn't go out of scope
	std::thread profileThread;

	std::vector<void*> addrs;
	size_t samples;

		// How much time in between each sample
	std::chrono::microseconds sleepTime;

		// If the program exits before profiler finishes all its samples, force quit
	std::atomic_bool forceStop = false;

		// Only logs data once per "start"
	bool hasLogged = false;
} *data;

namespace internal {
class Logger { ... };
} // namespace internal

// Linking between windows/linux handled in a cmake program.

// ----- Platform specific functions -----
void PlatformSpecificInit();
void PlatformSpecificExit();

// Returns RIP -- impl suspend main thread for duration of function
void* GetRip();
// Returns the name of the function the symbol in the address holds
std::string GetSymbolName(void* addr);

// ----- Non-platform specific functions -----
	// Collect samples every so often (runs on separate thread)
static void RecordData() {
	for (samples) {
			// Sleep until cooldown ends
		std::this_thread::sleep_for(data->sleepTime);
		
		if (data->forceStop)
			return;

			// Add next RIP sample
		data->addrs.push_back(GetRip());
	}
} 

	// Init data needed for profiler to run
void Init() {
	PlatformSpecificInit();
	data = new ProfilerData;
}
	// Start recording the program. records 'numSamples' samples
void Start(size_t numSamples = ..., size_t samplesPerMilli = ...) {
		// Reset params for data
	...
		// Init new thread
	data->profileThread = std::thread(RecordData);
}
	// Manually logs data collected to a file
void Report() {
		// Wait for data gathering to finish
	data->profileThread.join();

	create heat map of data
	for (addr in data->addrs) {
		if (GetSymbolName(addr) != "")
			add data to heat map
	}

	internal::Logger log{ file };
	log.Log(heat map, ...);
}
	// Cleansup all profiler data. also reports data collected so far if it hasn't finished
void Exit() {
		// Properly log and cleanup
	...
	delete data;

	PlatformSpecificExit();
}

} // namespace SProfiler


// ----- Windows Implementation -----
namespace SProfiler {

#include <Windows.h>
	// Links dbghelp library without messing with the visual studio project files.
#pragma comment(lib, "dbghelp.lib")
#include <DbgHelp.h>

static HANDLE process = nullptr;
static HANDLE mainThread = nullptr;

void PlatformSpecificInit() {
	if (already init)
		return;

	HANDLE currentProc = ...;
	HANDLE currentThread = ...;
		// Microsoft warns against using GetCurrentProcess for SymInitialize	
	DuplicateHandle(currentProc..., &process, ...);
	DuplicateHandle(currentThread..., &mainThread, ...);

		// Initialize symbols for the process based on flags.
	SymSetOptions(...);

	if (!SymInitialize(process, ...)) {
		error
		return;
	}
}

	// Just cleanup details
void PlatformSpecificExit() { ... }

void* GetRip() {
	pause mainThread

		// Retrieve RIP from current process
	CONTEXT c = {...};
	if (GetThreadContext(mainThread, &c))
		rip = reinterpret_cast<void*>(c.Rip);

	resume mainThread

	return rip;
}

	// Get name of symbol from an address
std::string GetSymbolName(void* addr) {
	PSYMBOL_INFO symbol = ...;

		// Grab symbol
	if (!SymFromAddr(..., addr, ...)) {
		error
		return {};
	}

		// Return just the name
	return symbol->Name;
}

} // namespace SProfiler
