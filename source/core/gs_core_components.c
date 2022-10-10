/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_components.c

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

#include "gs_core_components.h"

GS_API_DECL void 
gs_core_system_renderable_cb(gs_core_entities_system_t* system)
{ 
    gs_core_t* core = gs_core_instance();
    gs_core_system_renderable_t* sdata = (gs_core_system_renderable_t*)system;
    gs_core_entity_iter_t* iter = system->iter; 
	gs_core_graphics_renderable_t* rend = NULL;

    // Set renderable's model mtx using transform
    for (uint32_t i = 0; i < iter->count; ++i)
    {
        gs_core_component_transform_t* tc = &sdata->transform[i];
        gs_core_component_renderable_t* rc = &sdata->renderable[i];

        // Get renderable from graphics scene using handle
        rend = gs_core_graphics_scene_renderable_get(&core->gfx->scene, rc->hndl);
        rend->model = gs_vqs_to_mat4(&tc->transform);
    } 
}


