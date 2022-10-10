/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_editor.h

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2022 John Jackson

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

#ifndef GS_CORE_EDITOR_H
#define GS_CORE_EDITOR_H

// Core Includes
#include "core/gs_core.h"

// Defines 
#define GS_CORE_EDITOR_VIEW_STR_MAX    64

#ifdef GS_PLATFORM_WIN
    #define GS_CORE_EDITOR_DYNAMIC_LIB_EXT  "dll"
#else
    #define GS_CORE_EDITOR_DYNAMIC_LIB_EXT  "so"
#endif

struct gs_core_editor_view_s;

typedef void (* gs_core_editor_view_cb)(struct gs_core_editor_view_s* view);
typedef void (* gs_core_editor_scene_draw_cb)(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd);

// Editor view base
typedef struct gs_core_editor_view_s
{
    char name[GS_CORE_EDITOR_VIEW_STR_MAX]; // Name for editor view
    gs_core_editor_view_cb cb;              // Callback for view
} gs_core_editor_view_t;

typedef struct gs_core_editor_view_desc_s
{
    const char* name;
    gs_core_editor_view_cb cb;
    struct {
        const char* parent;
        gs_gui_split_type type;
        float amount;
    } split;

} gs_core_editor_view_desc_t;

typedef struct gs_core_editor_s
{
    gs_core_t* core;                                       // Core 
    gs_hash_table(uint64_t, gs_core_editor_view_t) views;  // All registered editor views 
    gs_gui_context_t gui;                                  // Gui context for all editor views
    gs_camera_t camera;                                    // Camera for scene (this should be held in a specific view instead)
    struct {
        gs_platform_file_stats_t fstats;    // To track dll hot reloading
        bool32 hot_reload_begin;            // Change detected for hot-reload             
        void* dll;                          // Pointer to library
        void* (* new)(gs_t* gs, gs_core_t* core);
        void (* free)(void* app);
        void (* meta_register)();
        void (* meta_unregister)();
        uint32_t hot_reload_timer;
        gs_core_app_t* app;                 // Pointer to application data
    } app;                                  // Struct for loaded application
} gs_core_editor_t;

GS_API_DECL void
gs_core_editor_view_register(const gs_core_editor_view_desc_t* view);

GS_API_DECL void 
gs_core_editor_init(void* data);

GS_API_DECL void
gs_core_editor_update(void* data);

GS_API_DECL void 
gs_core_editor_shutdown(void* data);

GS_API_DECL const char*
gs_core_editor_dll_path(); 

GS_API_DECL const char* 
gs_app_dll_path();

GS_API_DECL const char* 
gs_app_new_func_name(); 

GS_API_DECL const char* 
gs_app_init_func_name(); 

GS_API_DECL const char* 
gs_app_update_func_name(); 

GS_API_DECL const char* 
gs_app_shutdown_func_name(); 

GS_API_DECL const char* 
gs_app_scene_render_cb_name(); 

GS_API_DECL gs_core_editor_t* 
gs_core_editor_instance();

#endif // GS_CORE_EDITOR_H
