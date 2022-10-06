/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_entity.c

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

#include "gs_core_entity.h"
#include <flecs/flecs.h>
#include <flecs/flecs.c>

#define ECS()      gs_core_instance()->entities
#define CORE_ECS() ((gs_core_entity_data_t*)(ECS()->data))

typedef struct
{
    ecs_world_t* world;
	ecs_os_api_t api;
} gs_core_entity_data_t; 

GS_API_DECL gs_core_entities_t* 
gs_core_entities_new()
{
	// Bind existing api for defaults then return existing
	if (ECS())
	{
        gs_core_entities_set_instance(ECS());
		return ECS();
	}

    gs_core_entities_t* ents = gs_malloc_init(gs_core_entities_t); 
    ents->data = gs_malloc_init(gs_core_entity_data_t); 

    // Set up os api for flecs
	gs_core_entity_data_t* data = (gs_core_entity_data_t*)ents->data;
    ecs_os_set_api_defaults(); 
    data->api = ecs_os_api; 
    ecs_os_set_api(&data->api);

    ((gs_core_entity_data_t*)(ents->data))->world = ecs_init();
    ents->component_register = gs_core_entities_component_register_internal;
    ents->component_unregister = gs_core_entities_component_unregister_internal;
    ents->system_register = gs_core_entities_system_register_internal;
    ents->system_unregister = gs_core_entities_system_unregister_internal;
    return ents;
}

GS_API_DECL void 
gs_core_entities_set_instance(gs_core_entities_t* ents)
{ 
    // Set up os api for flecs
    gs_core_entity_data_t* data = (gs_core_entity_data_t*)ents->data;
    ecs_os_set_api(&data->api);
}

GS_API_DECL gs_core_entities_t* 
gs_core_entities_instance()
{
    return ECS();
} 

GS_API_DECL void 
gs_core_entities_update()
{
    ecs_progress(CORE_ECS()->world, gs_platform_delta_time());
}

GS_API_DECL void 
gs_core_entities_shutdown()
{
    ecs_fini(CORE_ECS()->world);
} 

GS_API_DECL gs_core_entity_world_t* 
gs_core_entities_world()
{
    gs_core_entity_data_t* ecs = CORE_ECS(); 
    return ecs->world;
}

GS_API_DECL gs_core_entity_t 
gs_core_entities_component_register_internal(const gs_core_entities_component_desc_t* desc)
{ 
    gs_core_entity_data_t* ecs = CORE_ECS(); 
    ecs_entity_t comp = ecs_component_init(ecs->world, &(ecs_component_desc_t){
        .entity.name = desc->name, 
        .size = desc->size, 
        .alignment = desc->alignment
    }); 
    return (gs_core_entity_t)comp;
}

GS_API_DECL void 
gs_core_entities_component_unregister_internal(gs_core_entity_t comp)
{ 
    gs_core_entity_data_t* ecs = CORE_ECS(); 
    // ecs_remove_pair(ecs->world, comp, EcsOnDelete, EcsThrow);
    // ecs_delete(ecs->world, comp);
}

GS_API_DECL gs_core_entity_t 
gs_core_entities_system_register_internal(const gs_core_entities_system_desc_t* desc)
{
    gs_core_entity_data_t* ecs = CORE_ECS(); 

    // Construct descriptor
    ecs_system_desc_t sdesc = {
        .entity = {.name = desc->name, .add = {EcsOnUpdate}},
        .callback = desc->callback
    };
    for (uint32_t i = 0; (void*)desc->filter.component_list[i] != NULL; ++i) {
        sdesc.query.filter.terms[i].id = desc->filter.component_list[i];
    } 

    // Init system
    return (gs_core_entity_t)ecs_system_init(ecs->world, &sdesc);
}

GS_API_DECL void
gs_core_entities_system_unregister_internal(gs_core_entity_t system)
{
    gs_core_entity_data_t* ecs = CORE_ECS();
    ecs_delete(ecs->world, system);
}

GS_API_DECL gs_core_entity_t 
gs_core_entities_allocate()
{
    gs_core_entity_data_t* ecs = CORE_ECS(); 

    // Allocate a new entity
    ecs_entity_t e = ecs_new(ecs->world, 0); 
    return (gs_core_entity_t)e;
}

GS_API_DECL void 
gs_core_entities_deallocate(gs_core_entity_world_t* world, gs_core_entity_t entity)
{
    gs_core_entity_data_t* ecs = CORE_ECS();

    // TODO(john): Iterate through all components for entity, then call destructor manually
    // Or need to set a generated dtor for this entity
    ecs_delete(world, entity);
} 

GS_API_DECL void 
gs_core_entities_component_remove_internal(gs_core_entity_world_t* world, 
    gs_core_entity_t entity, gs_core_entity_t comp)
{
} 







