#ifndef %APP%_H
#define %APP%_H

// Core Includes
#include <core/gs_core.h>

_introspect()
typedef struct %APP%_s
{ 
    gs_core_base(gs_core_app_t); 

    _vtable(
        _override: init     = %APP%_init;
        _override: update   = %APP%_update;
        _override: shutdown = %APP%_shutdown;
        _override: render   = %APP%_render;
    ) 

} %APP%_t; 

GS_API_DECL void 
%APP%_init(void* app); 

GS_API_DECL void 
%APP%_update(void* app); 

GS_API_DECL void 
%APP%_shutdown(void* app); 

GS_API_DECL void 
%APP%_render(void* app, gs_command_buffer_t* cb); 

#endif
