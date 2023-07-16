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

// Core Includes
#include "gs_core_entity.h"
#include "gs_core_components.h"

// Third Party Includes
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
    ecs_set_component_actions_w_id(ecs->world, comp, &(EcsComponentLifecycle) {
        .ctor = _gs_core_entities_component_ctor_internal,
        .dtor = _gs_core_entities_component_dtor_internal
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
    ecs_entity_t e = ecs_new_w_id(ecs->world, 0);
    gs_core_entities_component_add(ecs->world, e, gs_core_component_tag_t, {0});
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

GS_API_PRIVATE void 
_gs_core_entities_component_ctor_internal(gs_core_entity_world_t* world, gs_core_entity_t comp, 
        const gs_core_entity_t* ent, void* ptr, size_t size, int32_t cnt, void* ctx) 
{ 
    // Get raw component data as object, call init on vtable
    gs_core_obj_t* cp = ecs_get_id(world, *ent, comp);
    if (cp) {
        gs_core_cast_vt(cp, gs_core_obj_t)->obj_init(cp);
    }
}

GS_API_PRIVATE void 
_gs_core_entities_component_dtor_internal(gs_core_entity_world_t* world, gs_core_entity_t comp, 
        const gs_core_entity_t* ent, void* ptr, size_t size, int32_t cnt, void* ctx)
{ 
    // Get raw component data as object, call dtor on vtable
    gs_core_obj_t* cp = ecs_get_id(world, *ent, comp);
    if (cp) {
        gs_core_cast_vt(cp, gs_core_obj_t)->obj_dtor(cp);
    }
}

GS_API_DECL gs_result
gs_core_entities_entity_net_serialize(gs_byte_buffer_t* buffer, gs_core_entity_t ent)
{
    gs_core_entities_t* em = gs_core_entities_instance(); 
    gs_core_entity_data_t* ecs = CORE_ECS();

    // Store position for writing in number of components for later rewind
    uint32_t nc_position = buffer->position;

    // Write in number of components for this entity (to be counted and then corrected)
    uint32_t nc = 0;
    gs_byte_buffer_write(buffer, uint32_t, nc);

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(em->components);
        gs_hash_table_iter_valid(em->components, it);
        gs_hash_table_iter_advance(em->components, it) 
    )
    { 
        uint64_t cid = gs_hash_table_iter_getk(em->components, it);
        gs_core_entity_t comp_id = gs_hash_table_iter_get(em->components, it);

        // If entity has component, increment count, write it, and serialize component data
        if (gs_core_entities_component_has_id(ecs->world, ent, comp_id))
        { 
            nc++;
            gs_byte_buffer_write(buffer, uint64_t, cid);
            gs_byte_buffer_write(buffer, uint32_t, comp_id);
            gs_core_obj_t* comp = gs_core_entities_component_get_id(ecs->world, ent, comp_id);
            gs_core_obj_net_serialize(buffer, comp);
        } 
    } 

    // Adjust component number 
    *(uint32_t*)(buffer->data + nc_position) = nc;

    return GS_RESULT_SUCCESS;
}

GS_API_DECL gs_result
gs_core_entities_entity_net_deserialize(gs_byte_buffer_t* buffer, gs_core_entity_t ent)
{ 
    gs_core_entities_t* em = gs_core_entities_instance();
    gs_core_entity_data_t* ecs = CORE_ECS();

    // Read in number of components
    gs_byte_buffer_readc(buffer, uint32_t, nc); 

    for (uint32_t i = 0; i < nc; ++i)
    { 
        // Read class id and comp id
        gs_byte_buffer_readc(buffer, uint64_t, cid);
        gs_byte_buffer_readc(buffer, uint32_t, comp_id);

        // If doesn't have component, need to add
        if (!gs_core_entities_component_has_id(ecs->world, ent, comp_id))
        { 
            // Need a way to handle this case...
            gs_core_entities_component_add_cid(ecs->world, ent, cid, {0});
        }

        // Deserialize component data
        gs_core_obj_t* comp = gs_core_entities_component_get_id(ecs->world, ent, comp_id);
        gs_core_obj_net_deserialize(buffer, comp);
    }

    return GS_RESULT_SUCCESS;
}









