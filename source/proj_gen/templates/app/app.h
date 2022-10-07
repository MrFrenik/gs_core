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

} %APP%_t; 

GS_API_DECL void 
%APP%_init(void* app); 

GS_API_DECL void 
%APP%_update(void* app); 

GS_API_DECL void 
%APP%_shutdown(void* app); 

GS_API_DECL void 
%APP%_render(void* app, gs_command_buffer_t* cb); 

// Components/Systems

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_component_t);
    _dtor( 
        gs_core_graphics_scene_renderable_destroy(&gs_core_instance()->gfx->scene, this->hndl);
    ) 
    _field() uint32_t hndl;
} %APP%_component_renderable_t;

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_component_t);
    _field() gs_vqs transform;
} %APP%_component_transform_t;

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_system_t);
    _field() %APP%_component_renderable_t* renderable;
    _field() %APP%_component_transform_t* transform;
} %APP%_system_renderable_t;

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_system_t);
    _field() %APP%_component_transform_t* transform;
} %APP%_system_transform_t;

#endif
