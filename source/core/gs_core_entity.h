/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_entity.h

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

#ifndef GS_CORE_ENTITIES_H
#define GS_CORE_ENTITIES_H

// Core Includes
#include "gs_core_gs.h"
#include "gs_core_object.h"

// Third Party Includes
#include <flecs/flecs.h>

typedef ecs_entity_t gs_core_entity_t;
typedef ecs_iter_t   gs_core_entity_iter_t;
typedef ecs_world_t  gs_core_entity_world_t;

// Component

_introspect()
typedef struct
{
    gs_core_base(gs_core_obj_t);
} gs_core_entities_component_t;

typedef void (*gs_core_entity_component_func_t)(gs_core_entity_world_t* world, gs_core_entity_t comp, 
        const gs_core_entity_t* ent, void* ptr, size_t size, int32_t cnt, void* ctx); 

typedef struct
{
    gs_core_entity_component_func_t ctor;
    gs_core_entity_component_func_t dtor;
} gs_core_entities_component_lifecycle_t;

typedef struct
{
    const char* name;
    size_t size;
    size_t alignment;
    gs_core_entities_component_lifecycle_t lifecycle;
    gs_core_entity_t id;
} gs_core_entities_component_desc_t; 

#define gs_core_entities_component_declare(T)\
    ECS_COMPONENT_DECLARE(T)

#define gs_core_entities_system_declare(T)\
    ECS_SYSTEM_DECLARE(T)

#define gs_core_entity_id(T) ecs_id(T)

#define gs_core_entities_register_component(T)\
    do {\
        gs_core_entities_register_component_internal(&(gs_core_entities_component_desc_t){\
            .id = gs_core_entity_id(T),\
            .name = gs_to_str(T),\
            .size = sizeof(T),\
            .alignment = ECS_ALIGNOF(T)\
        });\
    } while (0) 

_introspect()
typedef struct
{
    gs_core_base(gs_core_obj_t); 
    gs_core_entity_iter_t* iter;
} gs_core_entities_system_t;

// Internal system function
typedef void (*gs_core_entities_system_func_t)(gs_core_entities_system_t* system);

#define gs_core_entities_component_add(WORLD, ENT, T, ...)\
    do {\
        ecs_set_id((WORLD), (ENT), gs_core_entity_id(T), sizeof(T), &(T)__VA_ARGS__);\
        T* _COMP = ecs_get((WORLD), (ENT), T);\
        gs_core_cls_init(T, _COMP);\
    } while (0)

#define gs_core_entities_component_remove(ENT, T)\
    do {\
        T* _COMP = ecs_get((WORLD), (ENT), T);\
        gs_assert(_COMP);\
        gs_core_obj_dtor(T, _COMP);\
        gs_core_entities_component_remove_internal((WORLD), (ENT), ecs_id(T));\
    } while (0)

#define gs_core_entities_component_has(WORLD, ENT, T)\
    ecs_has((WORLD), (ENT), T)

#define gs_core_entities_component_get(WORLD, ENT, T)\
    ecs_get((WORLD), (ENT), T)

#define gs_core_entities_term(IT, T, V)\
    ecs_term((IT), T, ((V) + 1))

#define CORE_ENTITIES_SYSTEM_COMPONENT_MAX    8

typedef struct
{ 
    gs_core_entity_t component_list[CORE_ENTITIES_SYSTEM_COMPONENT_MAX]; 
} gs_core_entities_system_filter_t; 

typedef struct 
{
    const char* name; 
    gs_core_entities_system_func_t callback; 
    gs_core_entities_system_filter_t filter;
} gs_core_entities_system_desc_t; 

typedef struct gs_core_entities_s
{ 
    void* data;
    ecs_entity_t (* component_register)(const gs_core_entities_component_desc_t* desc);
    void (* component_unregister)(gs_core_entity_t comp);
    ecs_entity_t (* system_register)(const gs_core_entities_system_desc_t* desc);
    void (* system_unregister)(gs_core_entity_t system);
} gs_core_entities_t; 

GS_API_DECL gs_core_entities_t* 
gs_core_entities_new();

GS_API_DECL gs_core_entities_t* 
gs_core_entities_instance();

GS_API_DECL void 
gs_core_entities_set_instance(gs_core_entities_t* ents); 

GS_API_DECL void 
gs_core_entities_update();

GS_API_DECL void 
gs_core_entities_shutdown();

// World
GS_API_DECL gs_core_entity_world_t* 
gs_core_entities_world();

GS_API_DECL gs_core_entity_t 
gs_core_entities_component_register_internal(const gs_core_entities_component_desc_t* desc);

GS_API_DECL void 
gs_core_entities_component_unregister_internal(gs_core_entity_t comp);

GS_API_DECL gs_core_entity_t 
gs_core_entities_system_register_internal(const gs_core_entities_system_desc_t* desc);

GS_API_DECL void 
gs_core_entities_system_unregister_internal(const gs_core_entities_system_desc_t* desc);

GS_API_DECL gs_core_entity_t 
gs_core_entities_allocate();

GS_API_DECL void 
gs_core_entities_deallocate(gs_core_entity_world_t* world, gs_core_entity_t entity);

GS_API_DECL void 
gs_core_entities_component_remove_internal(gs_core_entity_world_t* world, 
    gs_core_entity_t entity, gs_core_entity_t comp); 

#endif // GS_CORE_ENTITIES_H 

















