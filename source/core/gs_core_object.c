/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_object.c

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

#include "gs_core.h"

// Global meta registry from core
#define META() gs_core_instance()->meta 

GS_API_DECL gs_core_meta_registry_t* 
gs_core_meta_registry_new()
{ 
    if (META())
    {
        return META();
    }

    gs_core_meta_registry_t* meta = gs_malloc_init(gs_core_meta_registry_t);

    // Reserve space for enough elements (should be in a config just in case)
    gs_slot_array_reserve(meta->info, GS_CORE_REFL_CLASS_MAX);
    gs_hash_table_reserve(meta->registry.classes, uint64_t, gs_meta_class_t, GS_CORE_REFL_CLASS_MAX);

    // Insert null info
    for (uint32_t i = 0; i < GS_CORE_REFL_CLASS_MAX; ++i)
    {
        gs_core_meta_info_t empty = gs_default_val();
        gs_slot_array_insert(meta->info, empty);
    } 

    return meta;
}

GS_API_DECL gs_core_meta_registry_t* 
gs_core_meta_registry_instance()
{
    return META();
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_meta_obj_info(const gs_core_obj_t* obj)
{
    uint32_t id = gs_core_obj_id(obj);
	gs_core_t* core = gs_core_instance();
	gs_assert(core && core->meta);
	gs_core_meta_registry_t* meta = core->meta;
    gs_assert(gs_slot_array_handle_valid(meta->info, id));
    return gs_slot_array_getp(meta->info, id);
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_meta_obj_info_w_cls_id(uint32_t cls_id)
{
    gs_assert(gs_slot_array_handle_valid(META()->info, cls_id));
    return gs_slot_array_getp(META()->info, cls_id);
}

GS_API_DECL gs_core_obj_t*
gs_core_meta_static_ref_w_cls_id(uint32_t id)
{
    gs_assert(gs_slot_array_handle_valid(META()->info, id));
    return gs_slot_array_getp(META()->info, id)->instance;
}

//===[ Object Internal / Defaults ]===//

GS_API_DECL bool
gs_core_info_instance_of_internal(const gs_core_meta_info_t* info, uint32_t compare)
{
    if (!info || !compare) return false;

    // If the same class
    return (info->cid == compare);
}

GS_API_DECL bool
gs_core_info_base_of_internal(const gs_core_meta_info_t* info, uint32_t compare)
{
    if (!info || !compare) return false;

    // If the same class
    if (info->cid == compare) return true;

    // While has base
    const gs_core_meta_info_t* base = info->base;
    while (base)
    {
        if (base->cid == compare) return true;
        base = base->base;
    }

    return false;
} 

GS_API_DECL bool
gs_core_info_derived_from_internal(const gs_core_meta_info_t* info, uint32_t compare)
{
    if (!info || !compare) return false;

    // If the same class
    if (info->cid == compare) return false;

    // While has base
    const gs_core_meta_info_t* base = info->base;
    while (base)
    {
        if (base->cid == compare) return true;
        base = base->base;
    }

    return false;
}

GS_API_DECL gs_result 
gs_core_obj_serialize_impl(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj)
{ 
    const gs_meta_class_t* cls = gs_core_obj_cls(obj);

    //====[ Body ====//

    // Count of properties in array 
    gs_byte_buffer_write(buffer, uint32_t, cls->property_count);

    // Iterate through all of the properties, write to buffer
    for (uint32_t i = 0; i < cls->property_count; ++i)
    {
        const gs_meta_property_t* prop = &cls->properties[i];

        // Write the property name as hash 64 bit string
        gs_byte_buffer_write(buffer, uint64_t, gs_hash_str64(prop->name));

        // Write property type
        gs_byte_buffer_write(buffer, uint16_t, (uint16_t)prop->type.id);

        // Write property size
        gs_byte_buffer_write(buffer, size_t, prop->size);

    #define PROP_WRITE(T)\
        do {\
            const T* v = gs_meta_getvp(obj, T, prop);\
            gs_byte_buffer_write(buffer, T, *v);\
        } while (0)

        switch (prop->type.id)
        {
            case GS_META_PROPERTY_TYPE_S8:     PROP_WRITE(int8_t);  break;
            case GS_META_PROPERTY_TYPE_S16:    PROP_WRITE(int16_t); break;
            case GS_META_PROPERTY_TYPE_S32:    PROP_WRITE(int32_t); break;
            case GS_META_PROPERTY_TYPE_S64:    PROP_WRITE(int64_t); break;
            case GS_META_PROPERTY_TYPE_U8:     PROP_WRITE(uint8_t);  break;
            case GS_META_PROPERTY_TYPE_U16:    PROP_WRITE(uint16_t); break;
            case GS_META_PROPERTY_TYPE_U32:    PROP_WRITE(uint32_t); break;
            case GS_META_PROPERTY_TYPE_U64:    PROP_WRITE(uint64_t); break;
            case GS_META_PROPERTY_TYPE_F32:    PROP_WRITE(float); break;
            case GS_META_PROPERTY_TYPE_F64:    PROP_WRITE(double); break;
            case GS_META_PROPERTY_TYPE_VEC2:   PROP_WRITE(gs_vec2); break;
            case GS_META_PROPERTY_TYPE_VEC3:   PROP_WRITE(gs_vec3); break;
            case GS_META_PROPERTY_TYPE_VEC4:   PROP_WRITE(gs_vec4); break;
            case GS_META_PROPERTY_TYPE_QUAT:   PROP_WRITE(gs_quat); break;
            case GS_META_PROPERTY_TYPE_VQS:    PROP_WRITE(gs_vqs); break;
            case GS_META_PROPERTY_TYPE_ENUM:   PROP_WRITE(uint32_t); break;
            case GS_META_PROPERTY_TYPE_SIZE_T: PROP_WRITE(size_t); break;
            case GS_META_PROPERTY_TYPE_COLOR:  PROP_WRITE(gs_color_t); break;
            case GS_META_PROPERTY_TYPE_UUID:   PROP_WRITE(gs_uuid_t); break;
            case GS_META_PROPERTY_TYPE_OBJ:
            {
            } break;
        } 
    } 

    return GS_RESULT_SUCCESS;
}

GS_API_DECL gs_result 
gs_core_obj_deserialize_impl(gs_byte_buffer_t* buffer, gs_core_obj_t* obj)
{
    const gs_meta_class_t* cls = gs_core_obj_cls(obj); 

    //====[ Body ====//

    // Count of properties in array 
    gs_byte_buffer_readc(buffer, uint32_t, cnt); 

    // Iterate through all of the properties, write to buffer
    for (uint32_t i = 0; i < cnt; ++i)
    {
        // Write the property name as hash 64 bit string
        gs_byte_buffer_readc(buffer, uint64_t, hash_id);

        // Write property type
        gs_byte_buffer_readc(buffer, uint16_t, type_id);

        // Write property size
        gs_byte_buffer_readc(buffer, size_t, size);

        // Get property from hash table
        const gs_meta_property_t* prop = gs_hash_table_get(cls->property_map, hash_id);

        // Make sure exists and matches, or we skip ahead in size...
        if (
            prop->type.id != type_id || 
            prop->size != size
        )
        {
            // skip bytes then continue...
            gs_byte_buffer_advance_position(buffer, size);
            continue;
        }

    #define PROP_READ(T)\
        do {\
            gs_byte_buffer_readc(buffer, T, v);\
            gs_meta_setv(obj, T, prop, v);\
        } while (0)

        switch (prop->type.id)
        {
            case GS_META_PROPERTY_TYPE_S8:     PROP_READ(int8_t);  break;
            case GS_META_PROPERTY_TYPE_S16:    PROP_READ(int16_t); break;
            case GS_META_PROPERTY_TYPE_S32:    PROP_READ(int32_t); break;
            case GS_META_PROPERTY_TYPE_S64:    PROP_READ(int64_t); break;
            case GS_META_PROPERTY_TYPE_U8:     PROP_READ(uint8_t);  break;
            case GS_META_PROPERTY_TYPE_U16:    PROP_READ(uint16_t); break;
            case GS_META_PROPERTY_TYPE_U32:    PROP_READ(uint32_t); break;
            case GS_META_PROPERTY_TYPE_U64:    PROP_READ(uint64_t); break;
            case GS_META_PROPERTY_TYPE_F32:    PROP_READ(float); break;
            case GS_META_PROPERTY_TYPE_F64:    PROP_READ(double); break;
            case GS_META_PROPERTY_TYPE_VEC2:   PROP_READ(gs_vec2); break;
            case GS_META_PROPERTY_TYPE_VEC3:   PROP_READ(gs_vec3); break;
            case GS_META_PROPERTY_TYPE_VEC4:   PROP_READ(gs_vec4); break;
            case GS_META_PROPERTY_TYPE_QUAT:   PROP_READ(gs_quat); break;
            case GS_META_PROPERTY_TYPE_VQS:    PROP_READ(gs_vqs); break;
            case GS_META_PROPERTY_TYPE_ENUM:   PROP_READ(uint32_t); break;
            case GS_META_PROPERTY_TYPE_SIZE_T: PROP_READ(size_t); break;
            case GS_META_PROPERTY_TYPE_COLOR:  PROP_READ(gs_color_t); break;
            case GS_META_PROPERTY_TYPE_UUID:   PROP_READ(gs_uuid_t); break; 
            case GS_META_PROPERTY_TYPE_OBJ:
            {
            } break;
        } 
    } 

    return GS_RESULT_SUCCESS;
} 

GS_API_DECL gs_result
gs_core_obj_net_serialize_impl(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj)
{
    const gs_meta_class_t* cls = gs_core_obj_cls(obj);
    gs_assert(cls);

    //====[ Body ====// 

    // Since these packets are transient, the number of class properties is understood

    // Iterate through all of the properties, write to buffer
    for (uint32_t i = 0; i < cls->property_count; ++i)
    {
        const gs_meta_property_t* prop = &cls->properties[i];

        // Need custom packing for these
    #define PROP_WRITE(T)\
        do {\
            const T* v = gs_meta_getvp(obj, T, prop);\
            gs_byte_buffer_write(buffer, T, *v);\
        } while (0)

        switch (prop->type.id)
        {
            case GS_META_PROPERTY_TYPE_S8:     PROP_WRITE(int8_t);  break;
            case GS_META_PROPERTY_TYPE_S16:    PROP_WRITE(int16_t); break;
            case GS_META_PROPERTY_TYPE_S32:    PROP_WRITE(int32_t); break;
            case GS_META_PROPERTY_TYPE_S64:    PROP_WRITE(int64_t); break;
            case GS_META_PROPERTY_TYPE_U8:     PROP_WRITE(uint8_t);  break;
            case GS_META_PROPERTY_TYPE_U16:    PROP_WRITE(uint16_t); break;
            case GS_META_PROPERTY_TYPE_U32:    PROP_WRITE(uint32_t); break;
            case GS_META_PROPERTY_TYPE_U64:    PROP_WRITE(uint64_t); break;
            case GS_META_PROPERTY_TYPE_F32:    PROP_WRITE(float); break;
            case GS_META_PROPERTY_TYPE_F64:    PROP_WRITE(double); break;
            case GS_META_PROPERTY_TYPE_VEC2:   PROP_WRITE(gs_vec2); break;
            case GS_META_PROPERTY_TYPE_VEC3:   PROP_WRITE(gs_vec3); break;
            case GS_META_PROPERTY_TYPE_VEC4:   PROP_WRITE(gs_vec4); break;
            case GS_META_PROPERTY_TYPE_QUAT:   PROP_WRITE(gs_quat); break;
            case GS_META_PROPERTY_TYPE_VQS:    PROP_WRITE(gs_vqs); break;
            case GS_META_PROPERTY_TYPE_ENUM:   PROP_WRITE(uint32_t); break;
            case GS_META_PROPERTY_TYPE_SIZE_T: PROP_WRITE(size_t); break;
            case GS_META_PROPERTY_TYPE_COLOR:  PROP_WRITE(gs_color_t); break;
            case GS_META_PROPERTY_TYPE_UUID:   PROP_WRITE(gs_uuid_t); break;
            case GS_META_PROPERTY_TYPE_OBJ:
            {
            } break;
        } 
    } 

    return GS_RESULT_SUCCESS;
}

GS_API_DECL gs_result
gs_core_obj_net_deserialize_impl(gs_byte_buffer_t* buffer, gs_core_obj_t* obj)
{
    const gs_meta_class_t* cls = gs_core_obj_cls(obj);
    gs_assert(cls);

    //====[ Body ====// 

    // Since these packets are transient, the number of class properties is understood

    // Iterate through all of the properties, write to buffer
    for (uint32_t i = 0; i < cls->property_count; ++i)
    {
        const gs_meta_property_t* prop = &cls->properties[i];

        // Need custom packing for these
    #define PROP_READ(T)\
        do {\
            T* v = gs_meta_getvp(obj, T, prop);\
            gs_byte_buffer_read(buffer, T, v);\
        } while (0)

        switch (prop->type.id)
        {
            case GS_META_PROPERTY_TYPE_S8:     PROP_READ(int8_t);  break;
            case GS_META_PROPERTY_TYPE_S16:    PROP_READ(int16_t); break;
            case GS_META_PROPERTY_TYPE_S32:    PROP_READ(int32_t); break;
            case GS_META_PROPERTY_TYPE_S64:    PROP_READ(int64_t); break;
            case GS_META_PROPERTY_TYPE_U8:     PROP_READ(uint8_t);  break;
            case GS_META_PROPERTY_TYPE_U16:    PROP_READ(uint16_t); break;
            case GS_META_PROPERTY_TYPE_U32:    PROP_READ(uint32_t); break;
            case GS_META_PROPERTY_TYPE_U64:    PROP_READ(uint64_t); break;
            case GS_META_PROPERTY_TYPE_F32:    PROP_READ(float); break;
            case GS_META_PROPERTY_TYPE_F64:    PROP_READ(double); break;
            case GS_META_PROPERTY_TYPE_VEC2:   PROP_READ(gs_vec2); break;
            case GS_META_PROPERTY_TYPE_VEC3:   PROP_READ(gs_vec3); break;
            case GS_META_PROPERTY_TYPE_VEC4:   PROP_READ(gs_vec4); break;
            case GS_META_PROPERTY_TYPE_QUAT:   PROP_READ(gs_quat); break;
            case GS_META_PROPERTY_TYPE_VQS:    PROP_READ(gs_vqs); break;
            case GS_META_PROPERTY_TYPE_ENUM:   PROP_READ(uint32_t); break;
            case GS_META_PROPERTY_TYPE_SIZE_T: PROP_READ(size_t); break;
            case GS_META_PROPERTY_TYPE_COLOR:  PROP_READ(gs_color_t); break;
            case GS_META_PROPERTY_TYPE_UUID:   PROP_READ(gs_uuid_t); break;
            case GS_META_PROPERTY_TYPE_OBJ:
            {
            } break;
        } 
    } 

    return GS_RESULT_SUCCESS;
}

//====[ Debug ]====//

GS_API_DECL void
gs_core_obj_print(const gs_core_obj_t* obj)
{
    if (!obj || !gs_core_obj_id(obj)) return; 

    const gs_meta_class_t* cls = gs_core_obj_cls(obj);

    gs_println("cls: %s", cls->name);

    // For each property
    for (uint32_t i = 0; i < cls->property_count; ++i)
    {
        gs_meta_property_t* prop = &cls->properties[i];

        switch (prop->type.id)
        {
            case GS_META_PROPERTY_TYPE_S8:
            {
                gs_println("%s (%s): %d", prop->type.name, prop->name, gs_meta_getv(obj, int8_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_S16:
            {
                gs_println("%s (%s): %d", prop->type.name, prop->name, gs_meta_getv(obj, int16_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_S32:
            {
                gs_println("%s (%s): %d", prop->type.name, prop->name, gs_meta_getv(obj, int32_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_S64:
            {
                gs_println("%s (%s): %d", prop->type.name, prop->name, gs_meta_getv(obj, int64_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_U8:
            {
                gs_println("%s (%s): %zu", prop->type.name, prop->name, gs_meta_getv(obj, uint8_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_U16:
            {
                gs_println("%s (%s): %zu", prop->type.name, prop->name, gs_meta_getv(obj, uint16_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_U32:
            {
                gs_println("%s (%s): %zu", prop->type.name, prop->name, gs_meta_getv(obj, uint32_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_U64:
            {
                gs_println("%s (%s): %zu", prop->type.name, prop->name, gs_meta_getv(obj, uint64_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_F32:
            {
                gs_println("%s (%s): %.2f", prop->type.name, prop->name, gs_meta_getv(obj, float, prop));
            } break;

            case GS_META_PROPERTY_TYPE_F64:
            {
                gs_println("%s (%s): %.2f", prop->type.name, prop->name, gs_meta_getv(obj, double, prop));
            } break; 

            case GS_META_PROPERTY_TYPE_VEC2:
            {
                const gs_vec2* v = gs_meta_getvp(obj, gs_vec2, prop);
                gs_println("%s (%s): <%.2f, %.2f>", prop->type.name, prop->name, v->x, v->y);
            } break;

            case GS_META_PROPERTY_TYPE_VEC3:
            {
                const gs_vec3* v = gs_meta_getvp(obj, gs_vec3, prop);
                gs_println("%s (%s): <%.2f, %.2f, %.2f>", prop->type.name, prop->name, v->x, v->y, v->z);
            } break;

            case GS_META_PROPERTY_TYPE_VEC4:
            {
                const gs_vec4* v = gs_meta_getvp(obj, gs_vec4, prop);
                gs_println("%s (%s): <%.2f, %.2f, %.2f, %.2f>", prop->type.name, 
                    prop->name, v->x, v->y, v->z, v->w);
            } break;

            case GS_META_PROPERTY_TYPE_QUAT:
            {
                const gs_quat* v = gs_meta_getvp(obj, gs_quat, prop);
                gs_println("%s (%s): <%.2f, %.2f, %.2f, %.2f>", prop->type.name, 
                    prop->name, v->x, v->y, v->z, v->w);
            } break;

            case GS_META_PROPERTY_TYPE_MAT4:
            {
                const gs_mat4* v = gs_meta_getvp(obj, gs_mat4, prop);
                gs_println("%s (%s):", prop->type.name, prop->name);
                gs_println("\t<%.2f, %.2f, %.2f, %.2f>", v->m[0][0], v->m[0][1], v->m[0][2], v->m[0][3]);
                gs_println("\t<%.2f, %.2f, %.2f, %.2f>", v->m[1][0], v->m[1][1], v->m[1][2], v->m[1][3]);
                gs_println("\t<%.2f, %.2f, %.2f, %.2f>", v->m[2][0], v->m[2][1], v->m[2][2], v->m[2][3]);
                gs_println("\t<%.2f, %.2f, %.2f, %.2f>", v->m[3][0], v->m[3][1], v->m[3][2], v->m[3][3]);
            } break;

            case GS_META_PROPERTY_TYPE_VQS:
            {
                const gs_vqs* v = gs_meta_getvp(obj, gs_vqs, prop);
                const gs_vec3* tns = &v->translation;
                const gs_quat* rot = &v->rotation;
                const gs_vec3* scl = &v->scale;
                gs_println("%s (%s):", prop->type.name, prop->name);
                gs_println("\ttranslation: <%.2f, %.2f, %.2f>", tns->x, tns->y, tns->z);
                gs_println("\trotation: <%.2f, %.2f, %.2f, %.2f>", rot->x, rot->y, rot->z, rot->w);
                gs_println("\tscale: <%.2f, %.2f, %.2f>", scl->x, scl->y, scl->z);
            } break; 

            case GS_META_PROPERTY_TYPE_UUID:
            {
                gs_transient_buffer(TMP, 32);
                const gs_uuid_t* v = gs_meta_getvp(obj, gs_uuid_t, prop);
                gs_platform_uuid_to_string(TMP, v);            
                gs_println("%s (%s): %s", prop->type.name, prop->name, TMP);
            } break;

            case GS_META_PROPERTY_TYPE_SIZE_T:
            {
                gs_println("%s (%s): %zu", prop->type.name, prop->name, gs_meta_getv(obj, size_t, prop));
            } break;

            case GS_META_PROPERTY_TYPE_COLOR:
            {
                const gs_color_t* v = gs_meta_getvp(obj, gs_color_t, prop);
                gs_println("%s (%s): <%u, %u, %u, %u>", 
                    prop->type.name, prop->name, (uint32_t)v->r, (uint32_t)v->g, (uint32_t)v->b, (uint32_t)v->a);
            } break;

            case GS_META_PROPERTY_TYPE_OBJ:
            {
                const gs_core_obj_t* v = gs_meta_getvp(obj, gs_core_obj_t, prop);
                gs_core_obj_print(v);
            } break; 
        }
    }
}








