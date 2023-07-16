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

    gs_core_entity_t ent;
    gs_core_asset_handle_t mat_hndl;
    gs_core_asset_handle_t tex_hndl;
    gs_core_asset_handle_t pip_hndl;

} %APP%_t; 

GS_API_DECL void 
%APP%_init(); 

GS_API_DECL void 
%APP%_update(); 

GS_API_DECL void 
%APP%_shutdown(); 

GS_API_DECL void 
%APP%_render(gs_command_buffer_t* cb); 

// Components/Systems 

#endif
