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
#include "gs_core_app.h"
#include "core/generated/gs_core_generated.h"

#define GS_CORE_REFL_CLASS_MAX     100 

extern void* gs_core_os_malloc(size_t sz); 
extern void* gs_core_os_malloc_init(size_t sz);
extern void gs_core_os_free(void* ptr); 
extern void* gs_core_os_realloc(void* ptr, size_t sz); 
extern void* gs_core_os_calloc(size_t sz);
extern char* gs_core_os_strdup(const char* str);

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

    gs_gui_context_t* (* gs_gui_context_new)(uint32_t hndl);

} gs_core_t; 

GS_API_DECL gs_core_t* 
gs_core_new();

GS_API_DECL void
gs_core_free(gs_core_t* core);

GS_API_DECL gs_core_t* 
gs_core_instance();

GS_API_DECL void
gs_core_instance_set(gs_core_t* core); 

#endif // GS_CORE_H


