/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_components.h

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

#ifndef GS_CORE_COMPONENTS_H
#define GS_CORE_COMPONENTS_H 

// Core Includes
#include "gs_core_entity.h" 

// Components

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_component_t);

    _dtor( 
        gs_core_graphics_scene_renderable_destroy(&gs_core_instance()->gfx->scene, this->hndl);
    ) 

    _field() 
    uint32_t hndl;

} gs_core_component_renderable_t;

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_component_t);

    _field() 
    gs_vqs transform;

} gs_core_component_transform_t;

// Systems

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_entities_system_t);

    _field() 
    gs_core_component_renderable_t* renderable;

    _field() 
    gs_core_component_transform_t* transform;

} gs_core_system_renderable_t;

#endif GS_CORE_COMPONENTS_H
