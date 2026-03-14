#include "epch.h"
#include "Server.h"
#include <csignal>
#include <execinfo.h>
#include <unistd.h>

// Global server pointer for signal handler access
static Skeleton::Server* g_Server = nullptr;

void printBacktrace() {
    void* array[20];
    size_t size = backtrace(array, 20);
    fprintf(stderr, "Backtrace (%zu frames):\n", size);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}

void signalHandler(int sig) {
    const char* sigName = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: sigName = "SIGSEGV (Segmentation Fault)"; break;
        case SIGBUS:  sigName = "SIGBUS (Bus Error)"; break;
        case SIGABRT: sigName = "SIGABRT (Abort)"; break;
        case SIGFPE:  sigName = "SIGFPE (Floating Point Exception)"; break;
        case SIGILL:  sigName = "SIGILL (Illegal Instruction)"; break;
    }
    
    fprintf(stderr, "\n=== CRASH SIGNAL: %s (signal %d) ===\n", sigName, sig);
    printBacktrace();
    fprintf(stderr, "=== END CRASH DUMP ===\n");
    
    // Re-raise the signal with default handler to get core dump
    signal(sig, SIG_DFL);
    raise(sig);
}

void shutdownHandler(int sig) {
    fprintf(stderr, "\n=== SHUTDOWN SIGNAL: %d ===\n", sig);
    if (g_Server) {
        g_Server->Close();
    }
    exit(0);
}

int main()
{
    // Install signal handlers for crash diagnostics
    signal(SIGSEGV, signalHandler);
    signal(SIGBUS, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    
    // Install shutdown handlers
    signal(SIGINT, shutdownHandler);
    signal(SIGTERM, shutdownHandler);
    
    std::cout << "EntryPoint - Starting main()" << std::endl;
    try {
        std::cout << "EntryPoint - Creating Server object" << std::endl;
        Skeleton::Server server("127.0.0.1", 43594);
        g_Server = &server;

        std::cout << "EntryPoint - Server created, calling bind()" << std::endl;
        server.bind("127.0.0.1", 43594);
        std::cout << "EntryPoint - Bind complete, calling start()" << std::endl;
        server.start();
        std::cout << "EntryPoint - Start complete, sleeping" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::hours(24));
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    g_Server = nullptr;
    std::cout << "EntryPoint - Exiting normally" << std::endl;
    return 0;
}
