/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core.c

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

#include "core/gs_core.h"

// Global instance
gs_core_t* g_core;

GS_API_DECL gs_gui_context_t*
gs_core_gs_gui_context_new(uint32_t window_hndl) 
{
    gs_gui_context_t* ctx = gs_malloc_init(gs_gui_context_t);
    gs_gui_init(ctx, window_hndl);
    return ctx;
}

GS_API_DECL void
gs_core_gs_gui_renderpass_submit(gs_gui_context_t* ctx, gs_command_buffer_t* cb, gs_graphics_clear_action_t* action) 
{
    gs_gui_renderpass_submit_ex(ctx, cb, action);
}

GS_API_DECL gs_core_t* 
gs_core_new()
{
    // Return global instance if  already exists
    if (g_core)
    {
		// Set up entity static data
		gs_core_entities_set_instance(g_core->entities); 

		// Set network static data
		gs_core_network_set_instance(g_core->network);

		// Set up gsi static data
		gs_immediate_draw_static_data_set(g_core->gsi.data);

        return g_core;
    }

    // Allocate and set global instance
    gs_core_t* core = gs_malloc_init(gs_core_t);
    g_core = core;

    // Init all gs structures
    core->cb = gs_command_buffer_new();
    core->gsi = gs_immediate_draw_new();
    core->gui = gs_gui_new(gs_platform_main_window()); 

    // Register api for dll boundary
    core->gs_gui_context_new = gs_core_gs_gui_context_new;
    core->gs_gui_renderpass_submit = gs_core_gs_gui_renderpass_submit; 
    core->gs_gui_begin = gs_gui_begin;
    core->gs_gui_end = gs_gui_end; 
    core->gs_gui_window_begin_ex = gs_gui_window_begin_ex;
    core->gs_gui_window_end = gs_gui_window_end;

    // Construct meta registry
    core->meta = gs_core_meta_registry_new(); 

    // Register meta information
    gs_core_meta_register();

    // Assets
    core->assets = gs_core_assets_new(
        gs_platform_dir_exists("./assets") ? "./assets" : gs_platform_dir_exists("../assets") ? "../assets" : "../../assets");

    // Physics
    core->physics = gs_core_physics_new(); 

    // Entities
    core->entities = gs_core_entities_new(); 

    // Networking
    core->network = gs_core_network_new();

    // AI
    core->ai = gs_core_ai_new();

    // Graphics
    core->gfx = gs_core_graphics_new();

    // Register core ecs meta information
    gs_core_meta_ecs_register();

    // Register default entity so id = 0 will be considered invalid
    gs_core_entities_allocate();

    return core;
}

GS_API_DECL void
gs_core_free(gs_core_t* core)
{ 
    // Free gs structures
    gs_immediate_draw_free(&core->gsi);
    gs_command_buffer_free(&core->cb);
    gs_gui_free(&core->gui);

    // Unregister meta
    gs_core_meta_unregister();

    // Shutdown all systems
    gs_core_network_shutdown(core->network);
    gs_core_entities_shutdown();
    gs_core_physics_shutdown();
    gs_core_graphics_shutdown();
    gs_core_assets_shutdown();
    
    gs_free(core->entities);
    gs_free(core->gfx);
    gs_free(core->assets);
    gs_free(core->ai);
    gs_free(core->network);
    gs_free(core->physics);

    gs_free(core);
}

GS_API_DECL gs_core_t* 
gs_core_instance()
{
    return g_core;
}

GS_API_DECL void
gs_core_instance_set(gs_core_t* core)
{
    g_core = core;
}


