/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_graphics.c

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

#include "core/gs_core_graphics.h" 
#include "core/gs_core.h"

//====[ Core Graphics ]====//

GS_API_DECL gs_core_graphics_t*
gs_core_graphics_new()
{
    gs_core_t* core = gs_core_instance();
    if (core && core->gfx) return core->gfx; 

    gs_core_graphics_t* gfx = gs_malloc_init(gs_core_graphics_t);
    gfx->scene = gs_core_graphics_scene_create();
    gs_camera_t cam = gs_camera_perspective();
    cam.transform.position = gs_v3(0.f, 0.f, 5.f);
    gfx->scene.camera = gs_core_graphics_scene_camera_create(&gfx->scene, &cam);
    gfx->submit = gs_core_graphics_submit_cb;
    return gfx;
}

GS_API_DECL void 
gs_core_graphics_shutdown()
{
    gs_core_graphics_t* gfx = gs_core_graphics_instance();
    gs_slot_array_free(gfx->scene.renderables);
    gs_slot_array_free(gfx->scene.cameras);
}

GS_API_DECL gs_core_graphics_t* 
gs_core_graphics_instance()
{
    return gs_core_instance()->gfx;
}

//====[ Graphics Scene ]====//

GS_API_DECL gs_core_graphics_scene_t
gs_core_graphics_scene_create()
{
    gs_core_graphics_scene_t scene = {0};
    scene.camera = GS_SLOT_ARRAY_INVALID_HANDLE;
    return scene;
}

GS_API_DECL void
gs_core_graphics_scene_destroy(gs_core_graphics_scene_t* scene)
{
    gs_slot_array_free(scene->renderables);
    gs_slot_array_free(scene->cameras);
}

//====[ Renderable ]====//

GS_API_DECL uint32_t 
gs_core_graphics_scene_renderable_create(gs_core_graphics_scene_t* scene, const gs_core_graphics_renderable_t* renderable)
{
    return gs_slot_array_insert(scene->renderables, *renderable);
}

GS_API_DECL void
gs_core_graphics_scene_renderable_destroy(gs_core_graphics_scene_t* scene, uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->renderables, hndl))
    {
        gs_slot_array_erase(scene->renderables, hndl);
    }
}

GS_API_DECL gs_core_graphics_renderable_t* 
gs_core_graphics_scene_renderable_get(gs_core_graphics_scene_t* scene, uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->renderables, hndl)) return gs_slot_array_getp(scene->renderables, hndl);
    return NULL;
}

//====[ Camera ]====// 

GS_API_DECL uint32_t
gs_core_graphics_scene_camera_create(gs_core_graphics_scene_t* scene, 
    const gs_camera_t* camera)
{
    return gs_slot_array_insert(scene->cameras, *camera);
}

GS_API_DECL void
gs_core_graphics_scene_camera_destroy(gs_core_graphics_scene_t* scene, 
    uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->cameras, hndl))
    {
        gs_slot_array_erase(scene->cameras, hndl);
    }
}

GS_API_DECL gs_camera_t*
gs_core_graphics_scene_camera_get(gs_core_graphics_scene_t* scene, 
    uint32_t hndl)
{
    if (gs_slot_array_handle_valid(scene->cameras, hndl))
    {
        return gs_slot_array_getp(scene->cameras, hndl);   
    }

    return NULL;
}

GS_API_DECL gs_camera_t* 
gs_core_graphics_scene_main_camera(gs_core_graphics_scene_t* scene) 
{
    return gs_slot_array_getp(scene->cameras, 0);
}

GS_API_PRIVATE void
gs_core_graphics_submit_cb(gs_command_buffer_t* cb)
{
    gs_graphics_command_buffer_submit(cb);
}















