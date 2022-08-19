/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core_graphics.c

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

#include "core_graphics.h"

static core_graphics_t* g_graphics = NULL;

//====[ Core Graphics ]====//

GS_API_DECL core_graphics_t*
core_graphics_new()
{
    if (g_graphics) return g_graphics; 

    core_graphics_t* gfx = gs_malloc_init(core_graphics_t);
    gfx->scene = core_graphics_scene_create();
    gs_camera_t cam = gs_camera_perspective();
    gfx->scene.camera = core_graphics_scene_camera_create(&gfx->scene, &cam);
    g_graphics = gfx;
    return gfx;
}

GS_API_DECL void 
core_graphics_shutdown()
{
    // ...
}

GS_API_DECL core_graphics_t* 
core_graphics_instance()
{
    return g_graphics;
}

//====[ Graphics Scene ]====//

GS_API_DECL core_graphics_scene_t
core_graphics_scene_create()
{
    core_graphics_scene_t scene = {0};
    scene.camera = GS_SLOT_ARRAY_INVALID_HANDLE;
    return scene;
}

GS_API_DECL void
core_graphics_scene_destroy(core_graphics_scene_t* scene)
{
    gs_slot_array_free(scene->renderables);
    gs_slot_array_free(scene->cameras);
}

//====[ Renderable ]====//

GS_API_DECL uint32_t 
core_graphics_scene_renderable_create(core_graphics_scene_t* scene, const core_graphics_renderable_t* renderable)
{
    return gs_slot_array_insert(scene->renderables, *renderable);
}

GS_API_DECL void
core_graphics_scene_renderable_destroy(core_graphics_scene_t* scene, uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->renderables, hndl))
    {
        gs_slot_array_erase(scene->renderables, hndl);
    }
}

//====[ Camera ]====// 

GS_API_DECL uint32_t
core_graphics_scene_camera_create(core_graphics_scene_t* scene, 
    const gs_camera_t* camera)
{
    return gs_slot_array_insert(scene->cameras, *camera);
}

GS_API_DECL void
core_graphics_scene_camera_destroy(core_graphics_scene_t* scene, 
    uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->cameras, hndl))
    {
        gs_slot_array_erase(scene->cameras, hndl);
    }
}

GS_API_DECL gs_camera_t*
core_graphics_scene_camera_get(core_graphics_scene_t* scene, 
    uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->cameras, hndl))
    {
        return gs_slot_array_getp(scene->cameras, hndl);   
    }

    return NULL;
}

GS_API_DECL gs_camera_t* 
core_graphics_scene_main_camera(core_graphics_scene_t* scene) 
{
    return gs_slot_array_getp(scene->cameras, 0);
}















