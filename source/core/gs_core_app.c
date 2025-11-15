/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_app.c

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

#include "core/gs_core_app.h"
#include "core/gs_core_entity.h"

static gs_core_app_t* g_app = NULL;

GS_API_DECL gs_core_app_t*
gs_core_app_instance()
{
    return g_app;
}

GS_API_DECL void
gs_core_app_instance_set(gs_core_app_t* app)
{
    g_app = app;
}

GS_API_PRIVATE void
_gs_core_app_init()
{ 
	gs_core_app_t* app = NULL;

    // Register meta information
    #ifdef GS_CORE_APP_STANDALONE

		app = gs_user_data(gs_core_app_t);
		gs_core_app_instance_set(app);
        
        // Store command-line args from gs context
        gs_t* gs = gs_instance();
        app->argc = gs->ctx.app.argc;
        app->argv = gs->ctx.app.argv;
        
        gs_core_cast(app, gs_core_app_t)->core = gs_core_new(); 
        _gs_core_app_meta_register();

        // Set to playing
        app->state = GS_CORE_APP_STATE_PLAYING;

        // Need to get framebuffer and window size for placing gui elements within scene view 
        gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
        app->viewport = gs_v4(0.f, 0.f, fbs.x, fbs.y);
    #endif

	// Core app instance 
	app = gs_core_app_instance();

    // Init app
    gs_core_cast_vt(app, gs_core_app_t)->app_init();
}

GS_API_PRIVATE void
_gs_core_app_update()
{ 
	// Core app instance 
	gs_core_app_t* app = gs_core_app_instance();

    // If in standalone, then set the viewport manually
    // Need to get framebuffer and window size for placing gui elements within scene view 
    #ifdef GS_CORE_APP_STANDALONE
        gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
        app->viewport = gs_v4(0.f, 0.f, fbs.x, fbs.y);
    #endif 

    // Update network
    // gs_core_network_update();

    // Update application
    if (app->state == GS_CORE_APP_STATE_PLAYING) { 
        gs_core_cast_vt(app, gs_core_app_t)->app_update(); 
    }

    // Update entities
    gs_core_entities_update();

    // Render application
    #ifdef GS_CORE_APP_STANDALONE
		_gs_core_app_render(&gs_core_instance()->cb);
    #endif 
}

GS_API_PRIVATE void 
_gs_core_app_render(gs_command_buffer_t* cb)
{ 
	// Core app instance 
	gs_core_app_t* app = gs_core_app_instance();

    // Render application
    gs_core_cast_vt(app, gs_core_app_t)->app_render(cb);

    // Submit command buffer for GPU
    #ifdef GS_CORE_APP_STANDALONE 
        gs_graphics_command_buffer_submit(cb);
    #endif
}

GS_API_PRIVATE void
_gs_core_app_shutdown()
{ 
	// Core app instance 
	gs_core_app_t* app = gs_core_app_instance();

    // Shutdown app
    gs_core_cast_vt(app, gs_core_app_t)->app_shutdown();

    // Shutdown core (make this a pound define that can be easily used)
    #ifdef GS_CORE_APP_STANDALONE 

        // Unregister meta information 
        _gs_core_app_meta_unregister();

        // Free core
        gs_core_t* core = gs_core_instance();
        gs_core_free(core); 

    #endif
} 

GS_API_PRIVATE void 
_gs_core_app_editor(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd)
{ 
	// Core app instance 
	gs_core_app_t* app = gs_core_app_instance(); 
    gs_core_t* core = app->core;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());

    // Set viewport of application to use for rendering
    // app->viewport = gs_v4(cmd->viewport.x, fbs.y - cmd->viewport.h - cmd->viewport.y, cmd->viewport.w, cmd->viewport.h);
    app->viewport = gs_v4(cmd->viewport.x, cmd->viewport.y, cmd->viewport.w, cmd->viewport.h);
    
    // Render scene 
    gs_core_cast_vt(app, gs_core_app_t)->app_render(&ctx->gsi.commands); 
}



