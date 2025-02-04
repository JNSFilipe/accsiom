#ifndef AXLOG_H_
#define AXLOG_H_

#include <stdio.h>

//--------------------------------------------
// Logging System
//--------------------------------------------
typedef enum {
    AX_LOG_INFO,    // Informational message (no crash)
    AX_LOG_WARN,    // Non-fatal warning (no crash)
    AX_LOG_FATAL    // Fatal error (crashes the program)
} AxLogLevel;

// Helper to convert log level to string
static const char* ax_log_level_str(AxLogLevel level) {
    switch (level) {
        case AX_LOG_INFO:    return "INFO";
        case AX_LOG_WARN:    return "WARN";
        case AX_LOG_FATAL:   return "FATAL";
        default:             return "UNKNOWN";
    }
}

// Logging macro: crashes on FATAL, includes file/line info
#define AX_LOG(severity, fmt, ...) do { \
    fprintf(stderr, "[%s] %s:%d: ", ax_log_level_str((severity)), __FILE__, __LINE__); \
    fprintf(stderr, (fmt), ##__VA_ARGS__);                              \
    fprintf(stderr, "\n"); \
    if (severity == AX_LOG_FATAL) abort(); \
} while (0)


#endif //AXLOG_H_
