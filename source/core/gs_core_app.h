/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_app.h

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

#ifndef CORE_APP_H
#define CORE_APP_H 

#include "core/gs_core_gs.h"
#include "core/gs_core_object.h"

// Forward decl
struct gs_core_s;

typedef enum 
{ 
    GS_CORE_APP_STATE_STOPPED = 0,
    GS_CORE_APP_STATE_PLAYING, 
    GS_CORE_APP_STATE_PAUSED
} gs_core_app_state;

_introspect()
typedef struct gs_core_app_s
{ 
    // Base
    gs_core_base(gs_core_obj_t); 

    // Vtable
    _vtable(
        void (* app_init)() = NULL;
        void (* app_update)() = NULL;
        void (* app_shutdown)() = NULL;
        void (* app_render)(gs_command_buffer_t* cb) = NULL;
        void (* app_editor)(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd) = NULL;
    )

    // Fields
    _field() float run_time;
    _field() gs_vec4 viewport; 

    struct gs_core_s* core;         // Core framework 
    gs_core_app_state state;

} gs_core_app_t; 

GS_API_DECL gs_core_app_t*
gs_core_app_instance(); 

GS_API_DECL void
gs_core_app_instance_set(gs_core_app_t* app); 

GS_API_PRIVATE void* 
_gs_app_new(gs_t* gs, struct gs_core_s* core);

GS_API_PRIVATE void
_gs_core_app_init();

GS_API_PRIVATE void
_gs_core_app_update();

GS_API_PRIVATE void
_gs_core_app_shutdown();

GS_API_PRIVATE void 
_gs_core_app_render(gs_command_buffer_t* cb);

GS_API_PRIVATE void 
_gs_core_app_editor(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd); 

GS_API_DECL void 
_gs_core_app_meta_register();

GS_API_DECL void 
_gs_core_app_meta_unregister();

#define GS_CORE_APP_DEFINE(_NAME)\
    GS_API_DECL void\
    _gs_core_app_meta_register()\
    {\
        _NAME##_meta_register();\
        _NAME##_meta_ecs_register();\
    }\
\
    GS_API_DECL void\
    _gs_core_app_meta_unregister()\
    {\
        _NAME##_meta_unregister();\
    }\
\
    GS_API_DECL void*\
    _gs_core_app_new(gs_t* gs, struct gs_core_s* core)\
    {\
        gs_core_instance_set(core);\
        gs_set_instance(gs);\
        _NAME##_t* _app = gs_core_cls_new(_NAME##_t);\
        gs_core_app_instance_set(_app);\
        gs_core_cast(_app, gs_core_app_t)->core = gs_core_new();\
        return _app;\
    }\
\
    GS_API_DECL void\
    _gs_core_app_free(void* app)\
    {\
        if (app) gs_free(app);\
    }

#ifdef GS_CORE_APP_STANDALONE
    #define GS_CORE_APP_MAIN(_NAME)\
        int32_t main(int32_t argc, char** argv)\
        {\
            _NAME##_t* _app = gs_core_os_malloc_init(sizeof(_NAME##_t));\
            gs_core_cls_init(_NAME##_t, _app);\
            gs_t* inst = gs_create((gs_app_desc_t){\
                .user_data = _app,\
                .window.width = 800,\
                .window.height = 600,\
                .window.title = gs_to_str(_NAME),\
                .window.num_samples = 16,\
                .init = _gs_core_app_init,\
                .update = _gs_core_app_update,\
                .shutdown = _gs_core_app_shutdown\
            });\
\
            while (gs_app()->is_running) {\
                gs_frame();\
            }\
\
            gs_free(inst);\
\
            return 0;\
        }
#else
    #define GS_CORE_APP_MAIN(_NAME) gs_empty_instruction()
#endif


#endif // CORE_APP_H 












