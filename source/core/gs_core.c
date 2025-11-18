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

//==== CVar commands (needs to be generated) ====//

GS_API_DECL void
gs_core_ddt_help_cmd(int32_t argc, char** argv)
{ 
    gs_ddt_t* ddt = gs_core_ddt_instance(); 
    for (uint32_t i = 0; i < ddt->commands_len; ++i) { 
        gs_ddt_command_t* cmd = &ddt->commands[i];
        if (cmd->name) gs_ddt_printf(ddt, "* - %s, ", cmd->name);
        if (cmd->desc) gs_ddt_printf(ddt, "%40s\n", cmd->desc);
    }
}

GS_API_DECL void
gs_core_ddt_clear_cmd(int32_t argc, char** argv)
{ 
    gs_ddt_t* ddt = gs_core_ddt_instance(); 
    memset(ddt->tb, 0, sizeof(ddt->tb));
}

GS_API_DECL void
gs_core_ddt_cvar_print(const gs_core_cvar_t* cvar)
{
    if (!cvar || !cvar->val) return;
    gs_ddt_t* ddt = gs_core_ddt_instance();
    if (cvar->name) gs_ddt_printf(ddt, "[%s]: ", cvar->name);
    switch (cvar->type)
    {
        case GS_CORE_CVAR_INT:    {int32_t v = *(int32_t*)cvar->val; gs_ddt_printf(ddt, "%d\n", v);} break;
        case GS_CORE_CVAR_BOOL:   {bool v = *(bool*)cvar->val; gs_ddt_printf(ddt, "%d\n", v);} break;
        case GS_CORE_CVAR_FLOAT:  {float v = *(float*)cvar->val; gs_ddt_printf(ddt, "%.2f\n", v);} break;
        case GS_CORE_CVAR_FLOAT2: {gs_vec2* v = (gs_vec2*)cvar->val; gs_ddt_printf(ddt, "%.2f %.2f\n", v->x, v->y);} break;
        case GS_CORE_CVAR_FLOAT3: {gs_vec3* v = (gs_vec3*)cvar->val; gs_ddt_printf(ddt, "%.2f %.2f %.2f\n", v->x, v->y, v->z);} break;
        case GS_CORE_CVAR_FLOAT4: {gs_vec4* v = (gs_vec4*)cvar->val; gs_ddt_printf(ddt, "%.2f %.2f %.2f %.2f\n", v->x, v->y, v->z, v->w);} break;
    };
}

// Not sure about this...
GS_API_DECL void
gs_core_ddt_cvar_cmd(int32_t argc, char** argv)
{
    /*
        CVAR_%s_cmd(...)
        {
            // Grabs ddt instance, calls this command to set the cvar?
        }
    */

    if (argc < 1) {
        return;
    }

    // Name
    const char* name = argv[0];
    gs_core_cvar_t* cvar = gs_core_cvar_get(name);
    if (!cvar) return;

    // Print cvar current value
    if (argc < 2) {
        goto cvar_print; 
    }

    gs_ddt_t* ddt = gs_core_ddt_instance();

    #define CVAR_SET(CVAR, VAL) memcpy((CVAR)->val, &(VAL), sizeof((VAL)))

    // Switch on the type to determine how to parse args passed in?
    switch (cvar->type)
    {
        default: break;

        case GS_CORE_CVAR_INT: {
            int32_t v = atoi(argv[1]);
            CVAR_SET(cvar, v);
        } break;
        case GS_CORE_CVAR_BOOL: {
            bool v = (bool)atoi(argv[1]);
            CVAR_SET(cvar, v);
        } break;
        case GS_CORE_CVAR_FLOAT:  {
            float v = atof(argv[1]);
            CVAR_SET(cvar, v);
        } break;
        case GS_CORE_CVAR_FLOAT2: {
            if (argc > 2) {
                gs_vec2 v = (gs_vec2){atof(argv[1]), atof(argv[2])};
                CVAR_SET(cvar, v);
            }
        } break;
        case GS_CORE_CVAR_FLOAT3: {
            if (argc > 3) {
                gs_vec3 v = (gs_vec3){atof(argv[1]), atof(argv[2]), atof(argv[3])};
                CVAR_SET(cvar, v);
            }
        } break;
        case GS_CORE_CVAR_FLOAT4: {
            if (argc > 4) {
                gs_vec4 v = (gs_vec4){atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4])};
                CVAR_SET(cvar, v); 
            }
        } break;
    }

    cvar_print: 
        gs_core_ddt_cvar_print(cvar);
}

//==== CVars ====//

GS_API_DECL gs_core_cvar_t* 
gs_core_cvar_get(const char* name)
{
    gs_core_t* core = gs_core_instance();
    uint64_t hash = gs_hash_str64(name);
    if (gs_hash_table_exists(core->cvars.registry, hash)) {
        return gs_hash_table_getp(core->cvars.registry, hash);
    }
    return NULL;
}

GS_API_DECL void
gs_core_cvar_register(const gs_core_cvar_desc_t* desc)
{ 
    gs_core_t* core = gs_core_instance();
    if (!desc || !desc->name || !desc->val) {
        gs_log_warning("Invalid cvar desc.");
        return;
    }
    uint64_t hash = gs_hash_str64(desc->name);
    if (!gs_hash_table_exists(core->cvars.registry, hash)) 
    {
        gs_core_cvar_t cvar = {.type = desc->type, .val = desc->val};
        gs_snprintf(cvar.name, GS_CORE_CVAR_STR_MAX, "%s", desc->name);
        gs_ddt_command_t cmd = {0};
        cmd.func = gs_core_ddt_cvar_cmd;
        gs_snprintf(cmd.name, GS_DDT_STR_MAX, "%s", cvar.name);
        gs_snprintf(cmd.desc, GS_DDT_STR_MAX, "%s", desc->desc);
        cvar.cmd_hndl = gs_core_ddt_cmd_register(cmd);
        gs_hash_table_insert(core->cvars.registry, hash, cvar);
    }
}

GS_API_DECL void
gs_core_cvar_unregister(const char* name)
{
    gs_core_t* core = gs_core_instance();
    uint64_t hash = gs_hash_str64(name);
    if (gs_hash_table_exists(core->cvars.registry, hash)) 
    {
        // Unregister from cmds and registry
        gs_core_cvar_t* cvar = gs_hash_table_getp(core->cvars.registry, hash); 
        gs_core_ddt_cmd_unregister(cvar->cmd_hndl);
        gs_hash_table_erase(core->cvars.registry, hash, cvar);
    } 
}

//==== General API ====//

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

    core->mem_lock = SCHED_PIPE_INVALID;

    // Init all gs structures
    core->cb = gs_command_buffer_new();
    core->gsi = gs_immediate_draw_new();
    core->gui = gs_gui_new(gs_platform_main_window()); 
    core->ddt.ddt = (gs_ddt_t) { 
        .tb = "",
        .cb = "", 
        .commands = NULL,
        .commands_len = 0, 
        .size = 0.4f,
        .open_speed = 40.f, 
        .close_speed = 40.f, 
        .autoscroll = 1
    };

    // Register api for dll boundary
    core->gs_gui_context_new = gs_core_gs_gui_context_new;
    core->gs_gui_renderpass_submit = gs_core_gs_gui_renderpass_submit; 
    core->gs_gui_begin = gs_gui_begin;
    core->gs_gui_end = gs_gui_end; 
    core->gs_gui_window_begin_ex = gs_gui_window_begin_ex;
    core->gs_gui_window_end = gs_gui_window_end;

    // Task scheduler
    uintptr_t needed_memory;
    gs_scheduler_init(&core->sched.sched, &needed_memory,
                                GS_SCHED_DEFAULT, 0);
    core->sched.mem = gs_calloc(needed_memory, 1);
    gs_scheduler_start(&core->sched.sched, core->sched.mem);

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

    // Register core terminal commands
    gs_core_ddt_cmd_register((gs_ddt_command_t){
        .func = gs_core_ddt_help_cmd,
        .name = "help",
        .desc = "Displays all available registered commands/cvars"
    });

    gs_core_ddt_cmd_register((gs_ddt_command_t){
        .func = gs_core_ddt_clear_cmd,
        .name = "clear",
        .desc = "Clears all console text"
    });

    return core;
}

GS_API_DECL void
gs_core_free(gs_core_t* core)
{ 
    // Free gs structures
    gs_immediate_draw_free(&core->gsi);
    gs_command_buffer_free(&core->cb);
    gs_gui_free(&core->gui);
    gs_scheduler_stop(&core->sched, 1);
    gs_free(core->sched.mem);

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

GS_API_DECL gs_scheduler_t* 
gs_core_scheduler_instance()
{
    gs_core_t* core = gs_core_instance();
    return core ? &core->sched.sched : NULL;
}

GS_API_DECL gs_ddt_t*
gs_core_ddt_instance()
{
    gs_core_t* core = gs_core_instance();
    return core ? &core->ddt.ddt : NULL;
}

GS_API_DECL uint32_t
gs_core_ddt_cmd_register(gs_ddt_command_t cmd)
{
    gs_core_t* core = gs_core_instance();
    uint32_t hndl = gs_slot_array_insert(core->ddt.commands, cmd);
    core->ddt.ddt.commands = core->ddt.commands->data;
    core->ddt.ddt.commands_len = gs_slot_array_size(core->ddt.commands);
    return hndl;
}

GS_API_DECL void
gs_core_ddt_cmd_unregister(uint32_t hndl)
{
    gs_core_t* core = gs_core_instance();
    if (gs_slot_array_handle_valid(core->ddt.commands, hndl)) {
        gs_slot_array_erase(core->ddt.commands, hndl);
    }
    core->ddt.ddt.commands = core->ddt.commands->data;
    core->ddt.ddt.commands_len = gs_slot_array_size(core->ddt.commands);
}

GS_API_DECL void 
__gs_core_sched_async_thread_runner(void *user_data, gs_scheduler_t *s, gs_sched_task_partition_t p, uint32_t thread_num)
{
    gs_core_sched_async_t* async = (gs_core_sched_async_t*)user_data;
    gs_scheduler_add(async->sched, &async->rtask, async->run, async->user_data, async->tcnt, async->part);
    while (~async->flags & GS_CORE_ASYNC_DONE) {
        if (gs_sched_task_done(&async->rtask)) {
            async->flags = GS_CORE_ASYNC_DONE;
            gs_scheduler_join(async->sched, &async->rtask);
            break;
        }
    }
}

GS_API_DECL void
gs_core_sched_async_dispatch(gs_core_sched_async_t* async)
{
    if (!async || gs_core_sched_async_running(async)) return;
    gs_core_t* core = gs_core_instance();
    async->sched = gs_core_scheduler_instance();
    async->flags = GS_CORE_ASYNC_RUNNING;
    gs_scheduler_add(async->sched, &async->atask, __gs_core_sched_async_thread_runner, async, 1, 1);
}

GS_API_DECL bool
gs_core_sched_async_running(gs_core_sched_async_t* async)
{
    return (async && (async->flags & GS_CORE_ASYNC_RUNNING));
}

GS_API_DECL bool
gs_core_sched_async_finish(gs_core_sched_async_t* async)
{ 
    if (!async || gs_core_sched_async_running(async) || !async->flags) return false;
    if (async->cb) {
        async->cb(async);
    }
    async->flags = 0x00;
    gs_scheduler_join(gs_core_scheduler_instance(), &async->atask);
    return true;
}


