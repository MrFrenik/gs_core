/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core_graphics.h

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

#ifndef CORE_GRAPHICS_H
#define CORE_GRAPHICS_H

#include "core_gs.h"
#include "core_asset.h"

typedef struct
{
    core_asset_handle_t material;
    core_asset_handle_t mesh;
    gs_mat4 model;
} core_graphics_renderable_t;

typedef struct
{
    gs_slot_array(core_graphics_renderable_t) renderables;
    gs_slot_array(gs_camera_t) cameras;
    uint32_t camera;    // Main scene camera
} core_graphics_scene_t;

typedef struct core_graphics_s
{
    core_graphics_scene_t scene;   // Single scene for now...
} core_graphics_t;

//=====[ Core Graphics ]=====//

GS_API_DECL core_graphics_t* 
core_graphics_new();

GS_API_DECL void 
core_graphics_shutdown();

GS_API_DECL core_graphics_t* 
core_graphics_instance();

//=====[ Core Renderable ]=====// 

GS_API_DECL core_graphics_scene_t
core_graphics_scene_create();

GS_API_DECL void
core_graphics_scene_destroy(core_graphics_scene_t* scene);

//=====[ Core Renderable ]=====//

GS_API_DECL uint32_t 
core_graphics_scene_renderable_create(core_graphics_scene_t* scene, const core_graphics_renderable_t* renderable);

GS_API_DECL void 
core_graphics_scene_renderable_destroy(core_graphics_scene_t* scene, uint32_t hndl);

//====[ Camera ]====//

GS_API_DECL uint32_t
core_graphics_scene_camera_create(core_graphics_scene_t* scene, 
    const gs_camera_t* camera);

GS_API_DECL gs_camera_t*
core_graphics_scene_camera_get(core_graphics_scene_t* scene, 
    uint32_t hndl);

GS_API_DECL gs_camera_t* 
core_graphics_scene_main_camera(core_graphics_scene_t* scene);

#endif // CORE_GRAPHICS_H









