/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_crash.c

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
// Crash Handler Implementation
//
// Platform-agnostic crash handling system with per-platform implementations.
// This file contains the public API and dispatches to platform-specific code.
================================================================================*/

#include "core/gs_core_crash.h"

/*================================================================================
// Platform-Specific Function Declarations
//
// These functions are implemented in platform-specific files:
// - gs_core/source/platform/gs_core_crash_win32.c (Windows)
// - gs_core/source/platform/gs_core_crash_linux.c (Linux)
// - gs_core/source/platform/gs_core_crash_apple.c (macOS/iOS)
// - gs_core/source/platform/gs_core_crash_android.c (Android)
================================================================================*/

// Platform-specific implementation functions
bool gs_core_crash_handler_init_impl(gs_core_crash_handler_desc_t* desc);
void gs_core_crash_handler_shutdown_impl(void);
bool gs_core_crash_handler_is_initialized_impl(void);
void gs_core_crash_handler_write_dump_impl(void);

/*================================================================================
// Public API Implementation
//
// These functions dispatch to platform-specific implementations.
================================================================================*/

GS_API_DECL bool gs_core_crash_handler_init(gs_core_crash_handler_desc_t* desc)
{
    // Use default values if desc is NULL
    gs_core_crash_handler_desc_t default_desc = {
        .dump_directory = "./crash_dumps",
        .show_message_box = true,
        .write_log_file = true,
        .on_crash_callback = NULL,
        .callback_user_data = NULL
    };
    
    gs_core_crash_handler_desc_t* actual_desc = desc ? desc : &default_desc;
    
    // Dispatch to platform-specific implementation
    return gs_core_crash_handler_init_impl(actual_desc);
}

GS_API_DECL void gs_core_crash_handler_shutdown(void)
{
    gs_core_crash_handler_shutdown_impl();
}

GS_API_DECL bool gs_core_crash_handler_is_initialized(void)
{
    return gs_core_crash_handler_is_initialized_impl();
}

GS_API_DECL void gs_core_crash_handler_write_dump(void)
{
    gs_core_crash_handler_write_dump_impl();
}
