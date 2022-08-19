/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core.c

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

#include "core/core.h"

GS_API_DECL core_t* 
core_new()
{
    core_t* core = gs_malloc_init(core_t);

    // Init all gs structures
    core->cb = gs_command_buffer_new();
    core->gsi = gs_immediate_draw_new();
    core->gui = gs_gui_new(gs_platform_main_window()); 

    // Register all application meta information
    core_meta_register();

    // Assets
    core->assets = core_assets_new(
        gs_platform_dir_exists("./assets") ? "./assets" : "../assets");

    // Physics
    core->physics = core_physics_new(); 

    // Entities
    core->entities = core_entities_new(); 

    // Networking
    core->network = core_network_new();

    // AI
    core->ai = core_ai_new();

    // Graphics
    core->gfx = core_graphics_new(); 

    // Register Components/Systems
    meta_ecs_register();

    return core;
}

GS_API_DECL void
core_free(core_t* core)
{ 
    // Free gs structures
    gs_immediate_draw_free(&core->gsi);
    gs_command_buffer_free(&core->cb);
    gs_gui_free(&core->gui);

    // Shutdown network
    core_network_shutdown();

    gs_free(core);
}
