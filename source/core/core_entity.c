/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core_entity.c

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

#include "core_entity.h"
#include <flecs/flecs.h>
#include <flecs/flecs.c>

#define CORE_ECS() ((core_entity_data_t*)g_entities->data)

typedef struct
{
    ecs_world_t* world;
} core_entity_data_t; 

static core_entities_t* g_entities = NULL;

GS_API_DECL core_entities_t* core_entities_new()
{
    if (g_entities) return g_entities;

    core_entities_t* ents = gs_malloc_init(core_entities_t); 
    g_entities = ents; 
    ents->data = gs_malloc_init(core_entity_data_t);

    CORE_ECS()->world = ecs_init();

    return ents;
}

GS_API_DECL core_entities_t* core_entities_instance()
{
    return g_entities;
} 

GS_API_DECL void core_entities_update()
{
    ecs_progress(CORE_ECS()->world, 0);
}

GS_API_DECL void core_entities_shutdown()
{
} 

GS_API_DECL core_entity_world_t* core_entities_world()
{
    core_entity_data_t* ecs = CORE_ECS(); 
    return ecs->world;
}

GS_API_DECL core_entity_t core_entities_register_component_internal(const core_entities_component_desc_t* desc)
{ 
    core_entity_data_t* ecs = CORE_ECS(); 
    ecs_entity_t comp = ecs_component_init(ecs->world, &(ecs_component_desc_t){
        .entity.name = desc->name, 
        .size = desc->size, 
        .alignment = desc->alignment
    }); 
    return comp;
}

GS_API_DECL void core_entities_register_system(const core_entities_system_desc_t* desc)
{
    core_entity_data_t* ecs = CORE_ECS(); 

    // Construct descriptor
    ecs_system_desc_t sdesc = {
        .entity = {desc->name, .add = {EcsOnUpdate}},
        .callback = desc->callback
    };
    for (uint32_t i = 0; desc->filter.component_list[i] != NULL; ++i) {
        sdesc.query.filter.terms[i].id = desc->filter.component_list[i];
    } 

    // Init system
    ecs_system_init(ecs->world, &sdesc);
}

GS_API_DECL core_entity_t core_entities_allocate()
{
    core_entity_data_t* ecs = CORE_ECS(); 

    // Allocate a new entity
    ecs_entity_t e = ecs_new(ecs->world, 0); 
    return (core_entity_t)e;
}

GS_API_DECL void 
core_entities_deallocate(core_entity_world_t* world, core_entity_t entity)
{
    core_entity_data_t* ecs = CORE_ECS();

    // TODO(john): Iterate through all components for entity, then call destructor manually
    // Or need to set a generated dtor for this entity
    ecs_delete(world, entity);
} 

GS_API_DECL void core_entities_component_remove_internal(core_entity_world_t* world, 
    core_entity_t entity, core_entity_t comp)
{
} 







