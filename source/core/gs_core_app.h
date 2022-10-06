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

#include "core/gs_core_object.h"

// Forward decl
struct gs_core_s;

_introspect()
typedef struct gs_core_app_s
{ 
    gs_core_base(gs_core_obj_t); 
    struct gs_core_s* core;         // Core framework 
    gs_vec4 viewport;
} gs_core_app_t; 

GS_API_DECL void* 
_gs_app_new(gs_t* gs, struct gs_core_s* core);

#define GS_CORE_APP_META_REGISTER(NAME)\
    NAME##_meta_register();\
    NAME##_meta_ecs_register();

#define GS_CORE_APP_DEFINE(__T)\
    GS_API_DECL void*\
    _gs_app_new(gs_t* gs, struct gs_core_s* core)\
    {\
        gs_core_instance_set(core);\
        gs_set_instance(gs);\
        __T* _app = gs_malloc_init(__T);\
        return _app;\
    }\
\
    GS_API_DECL void\
    _gs_app_free(void* app)\
    {\
        if (app) gs_free(app);\
    }

#endif // CORE_APP_H 












