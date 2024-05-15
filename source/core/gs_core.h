/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core.h

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

#ifndef GS_CORE_H
#define GS_CORE_H

#include "gs_core_gs.h"
#include "gs_core_object.h"
#include "gs_core_ai.h"
#include "gs_core_asset.h"
#include "gs_core_entity.h"
#include "gs_core_network.h"
#include "gs_core_graphics.h"
#include "gs_core_physics.h"
#include "gs_core_util.h"
#include "gs_core_components.h"
#include "gs_core_app.h"
#include "core/generated/gs_core_generated.h"

#define GS_CORE_REFL_CLASS_MAX     1000
#define GS_CORE_CVAR_STR_MAX       256

// Want a way to track all allocations and log if they've been freed
extern void* gs_core_os_malloc(size_t sz); 
extern void* gs_core_os_malloc_init(size_t sz);
extern void gs_core_os_free(void* ptr); 
extern void* gs_core_os_realloc(void* ptr, size_t sz); 
extern void* gs_core_os_calloc(size_t num, size_t sz);
extern char* gs_core_os_strdup(const char* str);

typedef enum 
{
    GS_CORE_CVAR_INT = 0x00,
    GS_CORE_CVAR_BOOL,          // Probably not needed
    GS_CORE_CVAR_FLOAT,
    GS_CORE_CVAR_FLOAT2,
    GS_CORE_CVAR_FLOAT3,
    GS_CORE_CVAR_FLOAT4
} gs_core_cvar_type;

typedef struct
{
    const char* name; 
    gs_core_cvar_type type; 
    void* val;                   // Pointer to value to be set
    const char* desc;
} gs_core_cvar_desc_t;

typedef struct
{
    char name[GS_CORE_CVAR_STR_MAX];
    gs_core_cvar_type type;
    void* val;
    uint32_t cmd_hndl;
} gs_core_cvar_t;

typedef struct gs_core_s
{
    struct gs_core_assets_s*        assets;
    struct gs_core_physics_s*       physics;
    struct gs_core_entities_s*      entities;
    struct gs_core_network_s*       network;
    struct gs_core_ai_s*            ai;
    struct gs_core_graphics_s*      gfx;
    struct gs_core_meta_registry_s* meta;

    // GS Structures
    gs_command_buffer_t cb;
    gs_immediate_draw_t gsi;
    gs_gui_context_t gui;

    // Console (drop-down terminal)
    struct {
        gs_ddt_t ddt;
        gs_slot_array(gs_ddt_command_t) commands;
    } ddt;

    // Registry of all cvars
    struct {
        gs_hash_table(uint64_t, gs_core_cvar_t) registry;
    } cvars;

    struct {
        gs_scheduler_t sched;
        void* mem;
    } sched;

    gs_gui_context_t* (* gs_gui_context_new)(uint32_t hndl);
    void (* gs_gui_renderpass_submit)(gs_gui_context_t* ctx, gs_command_buffer_t* cb, gs_graphics_clear_action_t* action); 
    void (* gs_gui_begin)(gs_gui_context_t *ctx, const gs_gui_hints_t* hints);
    void (* gs_gui_end)(gs_gui_context_t *ctx); 
    int32_t (* gs_gui_window_begin_ex)(gs_gui_context_t* ctx, const char* title, gs_gui_rect_t rect, bool* open, const gs_gui_selector_desc_t* desc, int32_t opt);
    void (* gs_gui_window_end)(gs_gui_context_t* ctx);

    gs_atomic_int_t mem_lock;

} gs_core_t; 

GS_API_DECL gs_core_t* 
gs_core_new();

GS_API_DECL void
gs_core_free(gs_core_t* core);

GS_API_DECL gs_core_t* 
gs_core_instance();

GS_API_DECL void
gs_core_instance_set(gs_core_t* core);

GS_API_DECL gs_scheduler_t* 
gs_core_scheduler_instance();

GS_API_DECL gs_ddt_t*
gs_core_ddt_instance();

GS_API_DECL uint32_t
gs_core_ddt_cmd_register(gs_ddt_command_t cmd);

GS_API_DECL void
gs_core_ddt_cmd_unregister(uint32_t hndl);

GS_API_DECL gs_core_cvar_t* 
gs_core_cvar_get(const char* name);

GS_API_DECL void
gs_core_cvar_register(const gs_core_cvar_desc_t* desc);

GS_API_DECL void
gs_core_cvar_unregister(const char* name);

#ifdef GS_CORE_MEMORY_DBG

#define GS_CORE_MEMORY_DBG_MAX_ALLOCATIONS  35000

typedef struct gs_core_memory_alloc_s
{
    void* ptr;
    const char* file;
    uint32_t line;
    size_t sz;
} gs_core_memory_alloc_t;

GS_API_DECL void
gs_core_memory_print();

GS_API_DECL void
gs_core_memory_print_to_file(const char* path);

#endif

#endif // GS_CORE_H


/*

    CVar system - ties into console (gs_ddt)
    Want reflection system to pick up on them to use, quick way to access/mutate

    // Statically declare value to be run, reflection system will pick up on this and register internally with cvar system?

    // Statically declare, like normal, set to default value. System will set the pointer accordingly.
    gs_core_cvar(float) value = 10; 

    #define gs_core_cvar(T)         extern

    gs_core_cvar(
        float,
        .name = ,
        .desc = ,

    );

*/


