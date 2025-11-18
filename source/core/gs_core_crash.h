/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_crash.h

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

#ifndef GS_CORE_CRASH_H
#define GS_CORE_CRASH_H

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================================
// Crash Handler API
//
// Platform-agnostic crash handling system that automatically generates crash 
// dumps/reports when applications crash, enabling easier debugging and analysis.
//
// Features:
//   - Automatic minidump generation on Windows
//   - Optional crash callback for custom cleanup/logging
//   - Configurable dump directory
//   - Manual dump trigger for debugging
//
// Example Usage:
//
//   int main(int argc, char** argv) {
//       // Initialize crash handler early
//       gs_core_crash_handler_init(&(gs_core_crash_handler_desc_t){
//           .dump_directory = "./crash_dumps",
//           .show_message_box = true,
//           .write_log_file = true,
//           .on_crash_callback = my_crash_callback,
//           .callback_user_data = NULL
//       });
//       
//       // Rest of initialization...
//       gs_core_init();
//       
//       // Main loop...
//       
//       // Cleanup
//       gs_core_shutdown();
//       gs_core_crash_handler_shutdown();
//       
//       return 0;
//   }
//
================================================================================*/

typedef struct gs_core_crash_handler_desc_t {
    const char* dump_directory;           // Where to write crash dumps (default: "./crash_dumps")
    bool show_message_box;                // Show crash dialog on desktop platforms
    bool write_log_file;                  // Write crash log alongside dump
    void (*on_crash_callback)(void* ctx); // Pre-dump callback (for cleanup/logging)
    void* callback_user_data;             // User data for callback
} gs_core_crash_handler_desc_t;

// Initialize crash handler
// Returns true on success, false on failure
GS_API_DECL bool gs_core_crash_handler_init(gs_core_crash_handler_desc_t* desc);

// Shutdown crash handler
GS_API_DECL void gs_core_crash_handler_shutdown(void);

// Check if crash handler is active
GS_API_DECL bool gs_core_crash_handler_is_initialized(void);

// Manually trigger crash dump (for debugging)
GS_API_DECL void gs_core_crash_handler_write_dump(void);

#ifdef __cplusplus
}
#endif

#endif // GS_CORE_CRASH_H
