/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_graphics.h

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

#ifndef GS_CORE_GRAPHICS_H
#define GS_CORE_GRAPHICS_H

#include "gs_core_gs.h"
#include "gs_core_asset.h"

#define GS_CORE_GRAPHICS_MATERIAL_SLOT_MAX    8

typedef struct
{
    gs_core_asset_handle_t materials[GS_CORE_GRAPHICS_MATERIAL_SLOT_MAX];    // List of materials per mesh primitive
    gs_core_asset_handle_t mesh;                                              // Mesh to render for material
    gs_mat4 model;                                                            // Model matrix for renderable
} gs_core_graphics_renderable_t;

typedef struct
{
    gs_slot_array(gs_core_graphics_renderable_t) renderables;
    gs_slot_array(gs_camera_t) cameras;
    uint32_t camera;    // Main scene camera
} gs_core_graphics_scene_t;

typedef struct gs_core_graphics_s
{
    gs_core_graphics_scene_t scene;             // Single scene for now...
    void (* submit)(gs_command_buffer_t* cb);   // Used for .dll, so all graphics driver calls are done in main process
} gs_core_graphics_t;

//=====[ Core Graphics ]=====//

GS_API_DECL gs_core_graphics_t* 
gs_core_graphics_new();

GS_API_DECL void 
gs_core_graphics_shutdown();

GS_API_DECL gs_core_graphics_t* 
gs_core_graphics_instance();

//=====[ Core Renderable ]=====// 

GS_API_DECL gs_core_graphics_scene_t
gs_core_graphics_scene_create();

GS_API_DECL void
gs_core_graphics_scene_destroy(gs_core_graphics_scene_t* scene);

//=====[ Core Renderable ]=====//

GS_API_DECL uint32_t 
gs_core_graphics_scene_renderable_create(gs_core_graphics_scene_t* scene, const gs_core_graphics_renderable_t* renderable);

GS_API_DECL gs_core_graphics_renderable_t* 
gs_core_graphics_scene_renderable_get(gs_core_graphics_scene_t* scene, uint32_t hndl);

GS_API_DECL void 
gs_core_graphics_scene_renderable_destroy(gs_core_graphics_scene_t* scene, uint32_t hndl);

//====[ Camera ]====//

GS_API_DECL uint32_t
gs_core_graphics_scene_camera_create(gs_core_graphics_scene_t* scene, 
    const gs_camera_t* camera);

GS_API_DECL gs_camera_t*
gs_core_graphics_scene_camera_get(gs_core_graphics_scene_t* scene, 
    uint32_t hndl);

GS_API_DECL gs_camera_t* 
gs_core_graphics_scene_main_camera(gs_core_graphics_scene_t* scene); 

//====[ Render Pass ]====//

// Should be an asset, correct?

gs_core_asset_render_pass_t;

_introspect()
typedef struct gs_core_graphics_renderpass_s
{
    gs_core_base(gs_core_obj_t);
    _vtable(
        void (* exec)(gs_command_buffer_t* cb, gs_core_graphics_renderpass_t* pass) = NULL;
    )

    // Fields 
    _field() gs_core_asset_handle_t pip;  // Pipeline

} gs_core_graphics_renderpass_t;

//====[ Submit ]====//

GS_API_PRIVATE void
gs_core_graphics_submit_cb(gs_command_buffer_t* cb);

#endif // GS_CORE_GRAPHICS_H









