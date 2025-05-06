### A Collection of Different Projects
Each file contains a different project in some way, condensed down into one file:
- CrashHandler.cpp
    - A static library that will produce a dump file for whatever program you are running upon a crash.
- Profiler.cpp
    - A library that provides a sampler profiler for your program. Automatically records
    data to a file specified in the Logger class. (Logger implementation not shown.)
- MemDebugger.cpp
    - A library that provides a simple memory debugger for a Windows or Linux program. Overrides
    global new and delete functions to accomplish this, and handles logging the information out to a file.
    (Logger implementation not shown.)
- SSE.cpp
    - A simple Windows program that calculates the dot product for a vector type,
    multiplication for a vector type, and multiplication for a matrix type
    using SSE instructions up to SSE4.2