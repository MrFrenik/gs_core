/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_crash_win32.c

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2025 John Jackson

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=================================================================================================================*/

/*================================================================================
// Windows Crash Handler Implementation
//
// Uses Windows exception handling and DbgHelp API to generate minidumps.
================================================================================*/

#if defined(GS_PLATFORM_WIN)

#include "core/gs_core_crash.h"
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#include <time.h>

/*================================================================================
// Internal State
================================================================================*/

typedef struct gs_core_crash_state_t {
    bool initialized;
    gs_core_crash_handler_desc_t desc;
    LPTOP_LEVEL_EXCEPTION_FILTER previous_filter;
    char dump_directory[MAX_PATH];
} gs_core_crash_state_t;

static gs_core_crash_state_t g_crash_state = {0};

/*================================================================================
// Helper Functions
================================================================================*/

// Create directory recursively if it doesn't exist
static bool gs_core_crash_create_directory(const char* path)
{
    char temp_path[MAX_PATH];
    char* p = NULL;
    size_t len;
    
    // Copy path
    strncpy(temp_path, path, sizeof(temp_path) - 1);
    temp_path[sizeof(temp_path) - 1] = '\0';
    
    len = strlen(temp_path);
    if (temp_path[len - 1] == '/' || temp_path[len - 1] == '\\')
        temp_path[len - 1] = '\0';
    
    // Create directories recursively
    for (p = temp_path + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            *p = '\0';
            CreateDirectoryA(temp_path, NULL);
            *p = '\\';
        }
    }
    
    return CreateDirectoryA(temp_path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

// Get exception code name for logging
static const char* gs_core_crash_get_exception_name(DWORD code)
{
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:         return "Access Violation";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array Bounds Exceeded";
        case EXCEPTION_BREAKPOINT:               return "Breakpoint";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype Misalignment";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float Denormal Operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float Divide By Zero";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "Float Inexact Result";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "Float Invalid Operation";
        case EXCEPTION_FLT_OVERFLOW:             return "Float Overflow";
        case EXCEPTION_FLT_STACK_CHECK:          return "Float Stack Check";
        case EXCEPTION_FLT_UNDERFLOW:            return "Float Underflow";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal Instruction";
        case EXCEPTION_IN_PAGE_ERROR:            return "In Page Error";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer Divide By Zero";
        case EXCEPTION_INT_OVERFLOW:             return "Integer Overflow";
        case EXCEPTION_INVALID_DISPOSITION:      return "Invalid Disposition";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable Exception";
        case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged Instruction";
        case EXCEPTION_SINGLE_STEP:              return "Single Step";
        case EXCEPTION_STACK_OVERFLOW:           return "Stack Overflow";
        default:                                 return "Unknown Exception";
    }
}

// Write crash log file
static void gs_core_crash_write_log(const char* dump_filename, EXCEPTION_POINTERS* exception_ptrs)
{
    char log_filename[MAX_PATH];
    FILE* log_file;
    SYSTEMTIME local_time;
    DWORD exception_code = exception_ptrs->ExceptionRecord->ExceptionCode;
    void* exception_address = exception_ptrs->ExceptionRecord->ExceptionAddress;
    
    // Generate log filename (same as dump but with .log extension)
    strncpy(log_filename, dump_filename, sizeof(log_filename) - 1);
    log_filename[sizeof(log_filename) - 1] = '\0';
    
    // Replace .dmp with .log
    char* ext = strrchr(log_filename, '.');
    if (ext) {
        strcpy(ext, ".log");
    }
    
    // Open log file
    log_file = fopen(log_filename, "w");
    if (!log_file) return;
    
    GetLocalTime(&local_time);
    
    // Write crash information
    fprintf(log_file, "=== CRASH REPORT ===\n");
    fprintf(log_file, "Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            local_time.wYear, local_time.wMonth, local_time.wDay,
            local_time.wHour, local_time.wMinute, local_time.wSecond);
    fprintf(log_file, "\n");
    
    fprintf(log_file, "Exception Code: 0x%08X (%s)\n", 
            exception_code, gs_core_crash_get_exception_name(exception_code));
    fprintf(log_file, "Exception Address: 0x%p\n", exception_address);
    fprintf(log_file, "Thread ID: %lu\n", GetCurrentThreadId());
    fprintf(log_file, "\n");
    
    // OS Version
    fprintf(log_file, "OS Version: Windows\n");
    
    // Module info
    HMODULE module;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)exception_address, &module)) {
        char module_name[MAX_PATH];
        if (GetModuleFileNameA(module, module_name, sizeof(module_name))) {
            fprintf(log_file, "Module: %s\n", module_name);
        }
    }
    
    fprintf(log_file, "\n");
    fprintf(log_file, "Dump File: %s\n", dump_filename);
    
    fclose(log_file);
}

/*================================================================================
// Exception Handler
================================================================================*/

static LONG WINAPI gs_core_crash_exception_handler(EXCEPTION_POINTERS* exception_ptrs)
{
    char dump_filename[MAX_PATH];
    HANDLE dump_file;
    SYSTEMTIME local_time;
    MINIDUMP_EXCEPTION_INFORMATION mdei;
    BOOL dump_success;
    
    // Call user callback if provided
    if (g_crash_state.desc.on_crash_callback) {
        g_crash_state.desc.on_crash_callback(g_crash_state.desc.callback_user_data);
    }
    
    // Create dump directory if it doesn't exist
    gs_core_crash_create_directory(g_crash_state.dump_directory);
    
    // Generate timestamp-based filename
    GetLocalTime(&local_time);
    snprintf(dump_filename, sizeof(dump_filename),
             "%s\\crash_%04d%02d%02d_%02d%02d%02d.dmp",
             g_crash_state.dump_directory,
             local_time.wYear, local_time.wMonth, local_time.wDay,
             local_time.wHour, local_time.wMinute, local_time.wSecond);
    
    // Create dump file
    dump_file = CreateFileA(dump_filename, GENERIC_WRITE, 0, NULL,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (dump_file != INVALID_HANDLE_VALUE) {
        // Setup minidump info
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = exception_ptrs;
        mdei.ClientPointers = FALSE;
        
        // Write minidump with comprehensive information
        MINIDUMP_TYPE dump_type = (MINIDUMP_TYPE)(
            MiniDumpWithDataSegs |
            MiniDumpWithHandleData |
            MiniDumpWithUnloadedModules |
            MiniDumpWithProcessThreadData |
            MiniDumpWithFullMemoryInfo
        );
        
        dump_success = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            dump_file,
            dump_type,
            &mdei,
            NULL,
            NULL
        );
        
        CloseHandle(dump_file);
        
        // Write log file if enabled
        if (dump_success && g_crash_state.desc.write_log_file) {
            gs_core_crash_write_log(dump_filename, exception_ptrs);
        }
        
        // Show message box if enabled
        if (dump_success && g_crash_state.desc.show_message_box) {
            char message[512];
            snprintf(message, sizeof(message),
                     "The application has crashed.\n\n"
                     "A crash dump has been saved to:\n%s\n\n"
                     "Please send this file to the developers.",
                     dump_filename);
            MessageBoxA(NULL, message, "Application Crash", MB_OK | MB_ICONERROR);
        }
    }
    
    // Call previous exception handler if it exists
    if (g_crash_state.previous_filter) {
        return g_crash_state.previous_filter(exception_ptrs);
    }
    
    // Continue with default exception handling
    return EXCEPTION_CONTINUE_SEARCH;
}

/*================================================================================
// Platform-Specific API Implementation
================================================================================*/

bool gs_core_crash_handler_init_impl(gs_core_crash_handler_desc_t* desc)
{
    if (g_crash_state.initialized) {
        return false; // Already initialized
    }
    
    // Store configuration
    g_crash_state.desc = *desc;
    
    // Store dump directory
    strncpy(g_crash_state.dump_directory, desc->dump_directory, sizeof(g_crash_state.dump_directory) - 1);
    g_crash_state.dump_directory[sizeof(g_crash_state.dump_directory) - 1] = '\0';
    
    // Register unhandled exception filter
    g_crash_state.previous_filter = SetUnhandledExceptionFilter(gs_core_crash_exception_handler);
    
    g_crash_state.initialized = true;
    
    return true;
}

void gs_core_crash_handler_shutdown_impl(void)
{
    if (!g_crash_state.initialized) {
        return;
    }
    
    // Restore previous exception filter
    SetUnhandledExceptionFilter(g_crash_state.previous_filter);
    
    // Clear state
    memset(&g_crash_state, 0, sizeof(g_crash_state));
}

bool gs_core_crash_handler_is_initialized_impl(void)
{
    return g_crash_state.initialized;
}

void gs_core_crash_handler_write_dump_impl(void)
{
    if (!g_crash_state.initialized) {
        return;
    }
    
    // Trigger an exception to generate a dump
    // This simulates a crash for debugging purposes
    __try {
        RaiseException(0xE0000001, 0, 0, NULL);
    }
    __except(gs_core_crash_exception_handler(GetExceptionInformation())) {
        // Exception handled
    }
}

#endif // GS_PLATFORM_WIN
