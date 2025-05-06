/*****************************************************************************
  Some small snippets from a memory debugger project for both Windows and Linux.
  Some parts of the snippets are redacted to prevent future students from cheating.
  Lots of syntax is incorrect on purpose to convey the structure, rather than specifics.

  Author(s): Evan O'Bryant
  Copyright © 2022 DigiPen (USA) Corporation.    
*****************************************************************************/

// A custom allocator that allocates data using malloc/free
// rather than new/delete, since the debugger relies on overriding
// new and delete.
class MAllocator { ... };

// Logger structure to write leak data.
class Logger { ... };

// Stores the type of 'new' function that was called to allocate data.
// Checks details during deletion.
enum class AllocType { ... };

// Holds info about each piece of allocated data.
struct MemInfo { ... };

// Holds info about a leak, passed to logger.
struct LeakInfo { ... };

// typedefs for collections that need an allocator
template <typename T>
using list = std::list<T, MAllocator<T>>;

using string = std::basic_string<char, std::char_traits<char>, MAllocator<char>>;

// This class is a singleton, using a counter struct to allocate,
// initialize, and free the debugger using malloc/free, and placement new.
class MemDebugger {
    inline static MemDebugger* s_instance = nullptr;

public:
    static inline MemDebugger& Get() { return *s_instance; }

    // ----- Platform independent -----
    // Add memory to the watch list (allocList)
    void WatchMemory(void* addr, size_t size, void* ret, AllocType a) { ... }

    // Move memory from the alloc list to the free list
    void DisregardMemory(void* addr) { ... }

    // Find if memory at an address is already on the free list.
    // If true, log double delete, and update the return pointer to be correct.
    bool OnFreeList(void* addr, void* ret) { ... }

    // Check if memory[addr] can be deleted
    bool IsValidDel(void* addr, AllocType delType, void* ret) {
        auto [couldFind, allocType] = allocList.find(addr);

        // Log non-heap pointer free
        if (!couldFind) { ... return false; }

        // Log mismatched new/delete and update return pointer to be correct.
        if (allocType != delType) { ... return false; }

        return true;
    }

    // ----- Platform dependent -----
    void* VAAlloc(size_t size); // The allocator
    bool VDealloc(void* addr);  // Decommits memory, still keeps track of it.
    bool VRelease(void* addr);  // Releases all deallocated memory.

private:
    friend class MemDebugCounter; // counter struct mentioned above

    // ----- Platform independent -----
    // Writes leak info for any still allocated data, then releases everything else.
    void OnExit();
    ~MemDebugger();

    // ----- Platform dependent -----
    MemDebugger();
    LeakInfo GetLeakInfo(MemInfo& mInf, MemIssue issue);

// ----- Vars -----
private:
    list<MemInfo> allocList, freeList;
};

// Memory allocation function, also has noexcept version.
void* DebugNew(size_t size, AllocType a, void* ret) {
    MemDebugger& debug = MemDebugger::Get();
    void* data = debug.VAAlloc(size);

    if (!data) {
        // This value should be initialized upon the function being called for the
        // first time. Prevents creation of this bad_alloc exception from throwing
        // more bad_alloc exceptions, most of the time at least.
        static std::bad_alloc outOfMem;
        throw outOfMem;
    }

    debug.WatchMemory(...);

    return data;
}

// Memory dellocation function, doesn't release memory
void DebugDelete(void* addr, AllocType a, void* ret) noexcept {
    // deletion of nullptr ok!
    if (!addr)
        return;
    
    MemDebugger& debug = MemDebugger::Get();

    // Attempt to deallocate the memory if it's valid. Otherwise log the issue.
    if (!debug.OnFreeList(...) && debug.IsValidDel(...)) {
        debug.DisregardMemory(...);

        try debug.VDealloc(addr);
    }
}

#include <new>

// Override each operate new/delete overload to have any 'new's or 'delete's
// used in the program call the DebugNew/DebugDelete function to allocate memory
// instead. Instrinsics/builtins are used to get the return address
// depending on the OS.
void* operator new(size_t size) {
    return DebugNew(size, ...);
}

void operator delete(void* addr) {
    DebugDelete(addr, ...);
}


// ----- Windows -----
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <Windows.h>

#pragma comment(lib, "dbghelp.lib")
#include <DbgHelp.h>

// private var
HANDLE processHandle = nullptr;

MemDebugger::MemDebugger() {
    init processHandle
    set symbol options
    initialize symbols
}

MemDebugger::VAAlloc(size_t size) {
    size = min(1, size);

    // Determine amount of pages to allocate
    const auto pages = calcPagesFromSize(size);

    // Calculate amount of user-accessible memory.
    const auto commitSize = calcComitSizeFromPages(pages);

    // Just reserving enough memory + one extra page
    void* base = VirtualAlloc(...);

    // Allocate user-accessible memory from base
    void* commit = VirtualAlloc(base, ...);

    // Address closest to the end of the usable page
    return calcCommitAddr(commit, commitSize);
}

MemDebugger::VDealloc(void* addr) {
    // This causes a warning, it is ignored as the memory is released later.
    return VirtualFree(..., MEM_DECOMMIT);
}

MemDebugger::VRelease(void* addr) {
    return VirtualFree(..., MEM_RELEASE);
}

LeakInfo MemDebugger::GetLeakInfo(MemInfo& mInf, MemIssue issue) {
    IMAGEHLP_LINE64 line{ ... };

    // Statically allocate a symbol and the name array
    char symbolBuf[sizeof(PIMAGEHLP_SYMBOL64) + ...] = { 0 };
    PIMAGEHLP_SYMBOL64 symbol = ...;
    init symbol;

    // -- All errors are checked --

    // Retrieve symbol and displacement
    SymGetSymFromAddr64(..., symbol);

    // Use symbol displacement and line to get line data.
    SymGetLineFromAddr64(..., &line);

    // Send leak info to logger
    return LeakInfo{ ... };
}

// ----- Linux -----
#include <dlfcn.h>
#include <execinfo.h>
#include <link.h>
#include <sys/mman.h>
#include <sstream>
#include <stdio.h>
#include <unordered_map>

template <typename K, typename V>
using map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, MAllocator<std::pair<const K, V>>>;

// private vars:

// munmap doesn't autodetect mapped regions + sizes, so this is kept track of manually
map<void*, ...> memMap;
const string execPath;

// Get virtual memory address representation of a physical address
void* GetVMA(void* addr) {
    dladdr1(addr, ...);
    
    return convertToVirtual(addr);
}

string GetExecutablePath() { ... }

MemDebugger::MemDebugger() : memMap(), execPath(GetExecutablePath()) { ... }

void* MemDebugger::VAAlloc(size_t size) {
    size = min(1, size);

    // Determine amount of pages to allocate
    const auto pages = calcPagesFromSize(size);

    // Calculate amount of user-accessible memory.
    const auto commitSize = calcComitSizeFromPages(pages);

    // Just reserving enough memory + one extra page
    void* base = mmap(...);

        // Mark the usable area
    mprotect(base, ..., PROT_READ | PROT_WRITE);

    // Address closest to the end of the usable page
    void* userStart = calcCommitAddr(base, commitSize);

    // Keep track of allocated memory info
    memMap[userStart] = std::make_pair(base, ...);

    return userStart;
}

bool VDealloc(void* addr) {
    auto& base = memMap.at(addr);

    return mprotect(..., PROT_NONE) was successful;
}

bool VRelease(void* addr) {
    auto& base = memMap.at(addr);

    // Attempt to unmap the data
    munmap(...) -> return false if failure;

    memMap.erase(addr);
    return true;
}

LeakInfo MemDebugger::GetLeakInfo(MemInfo& mInf, MemIssue issue) {
    // return addr --> virtual mem addr
    void* vma = GetVMA(...);

    // Create command to run in a shell window
    stringstream<..., Mallocator<char>> stringStream;
    stringStream << "/bin/addr2line -e" << execPath << " --functions --demangle " << vma;
    string command = stream.str();

    // Invoke new shell and run command
    auto shell = popen(command.c_str(), "r");

    reset stringStream

    for (each line in shell output) {
        remove newline character
        add to stringStream
    }

    pclose(shell);

    string result = stringStream.str();
    string leakData[X] = substr of result containing needed data;

    return LeakInfo{ ... };
}
