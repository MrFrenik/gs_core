/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_object.h

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

#ifndef GS_CORE_OBJECT_H
#define GS_CORE_OBJECT_H 

#include <gs/gs.h>
#include <gs/util/gs_meta.h>

// Macros
#define gs_core_base(T)        T _base
#define gs_core_cast(OBJ, T)   ((T*)(OBJ)) 

// Static ref
#define gs_core_obj_static_ref_w_cls_id(ID)   (gs_core_meta_static_ref_w_cls_id((ID))
#define gs_core_obj_static_ref_w_info(INFO)   (gs_core_meta_static_ref_w_cls_id((INFO)->cid))

// Info
#define gs_core_info_derived_from(INFO, T)  (gs_core_info_derived_from_internal((INFO), gs_core_cls_cid(T)))
#define gs_core_info_base_of(INFO, T)       (gs_core_info_base_of_internal((INFO), gs_core_cls_cid(T)))
#define gs_core_info_w_cls_id(ID)           (gs_core_meta_obj_info_w_cls_id((ID)))

// Class id
#define gs_core_cls_cid(T)       T##_class_id()
#define gs_core_cls_info(T)      T##_info()
#define gs_core_cls_init(T, OBJ) T##_init(OBJ)
#define gs_core_cls_new(T)       T##_new()
#define gs_core_cls_ctor(T)      T##_ctor()

// VTable
#define gs_core_cast_vt(OBJ, T)                       (gs_core_cast(gs_core_obj_info(OBJ)->vtable, T##_vtable_t))
#define gs_core_vt(T)                                 T##_vtable_t

// Object
#define gs_core_obj_id(OBJ)                      (gs_core_cast((OBJ), gs_core_base_t)->id)
#define gs_core_obj_cls_id(OBJ)                  (gs_core_cast((OBJ), gs_core_base_t)->id)
#define gs_core_obj_info(OBJ)                    (gs_core_meta_obj_info((OBJ)))
#define gs_core_obj_cls(OBJ)                     (gs_core_obj_info((OBJ))->cls)
#define gs_core_obj_cid(OBJ)                     (gs_core_obj_vt((OBJ))->cls_id())
#define gs_core_obj_vt(OBJ)                      (gs_core_cast_vt((OBJ), gs_core_obj_t)) 
#define gs_core_obj_serialize(BUFFER, OBJ)       (gs_core_obj_vt((OBJ))->serialize((BUFFER), (OBJ)))
#define gs_core_obj_deserialize(BUFFER, OBJ)     (gs_core_obj_vt((OBJ))->deserialize((BUFFER), (OBJ)))
#define gs_core_obj_net_serialize(BUFFER, OBJ)   (gs_core_obj_vt((OBJ))->net_serialize((BUFFER), (OBJ)))
#define gs_core_obj_net_deserialize(BUFFER, OBJ) (gs_core_obj_vt((OBJ))->net_deserialize((BUFFER), (OBJ)))

// Meta
#define _introspect(...)        gs_empty_instruction()
#define _callback(T)            gs_empty_instruction()
#define _field(...)             gs_empty_instruction()
#define _ignore(...)            gs_empty_instruction()
#define _serialize(...)         gs_empty_instruction()
#define _deserialize(...)       gs_empty_instruction()
#define _rpc(...)               gs_empty_instruction()
#define _net_serialize(...)     gs_empty_instruction()
#define _net_deserialize(...)   gs_empty_instruction()
#define _ctor(...)              gs_empty_instruction()
#define _dtor(...)              gs_empty_instruction()
#define _vtable(...)            gs_empty_instruction()

// Forward Decls.
struct gs_core_meta_info_s;
struct gs_core_obj_s;
struct gs_core_vtable_s;

/*
typedef struct
{ 
    const gs_meta_class_t* (* class)();
    gs_result (* serialize)(gs_byte_buffer_t* buffer, const struct gs_core_obj_s* obj);
    gs_result (* deserialize)(gs_byte_buffer_t* buffer, struct gs_core_obj_s* obj);
    gs_result (* net_serialize)(gs_byte_buffer_t* buffer, const struct gs_core_obj_s* obj);
    gs_result (* net_deserialize)(gs_byte_buffer_t* buffer, struct gs_core_obj_s* obj);
    uint32_t (* class_id)();
    const struct gs_core_meta_info_s* (* info)();
} gs_core_vtable_t;
*/

typedef struct gs_core_base_s
{
    uint32_t id;
} gs_core_base_t; 

#define gs_core_vtable_t_methods\
    const gs_meta_class_t* (* cls)();\
    uint32_t (* cls_id)();\
    void (* obj_init)(struct gs_core_obj_s*);\
    void (* obj_dtor)(struct gs_core_obj_s*);\
    const struct gs_core_meta_info_s* (* info)();

typedef struct gs_core_vtable_s
{
    gs_core_vtable_t_methods
} gs_core_vtable_t;

typedef struct gs_core_meta_info_s
{ 
    const gs_meta_class_t* cls;
    const struct gs_core_meta_info_s* base;
    struct gs_core_obj_s* instance;
    void* vtable;
    uint32_t cid;
    size_t size;
} gs_core_meta_info_t;

_introspect()
typedef struct gs_core_obj_s
{ 
    gs_core_base(gs_core_base_t); 

    // Methods
    _vtable( 
        gs_result (* serialize)(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj) = gs_core_obj_serialize_impl; 
        gs_result (* deserialize)(gs_byte_buffer_t* buffer, gs_core_obj_t* obj) = gs_core_obj_deserialize_impl; 
        gs_result (* net_serialize)(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj) = gs_core_obj_net_serialize_impl;
        gs_result (* net_deserialize)(gs_byte_buffer_t* buffer, gs_core_obj_t* obj) = gs_core_obj_net_deserialize_impl;
    )

} gs_core_obj_t;

//===[ Meta Generated ]===// 

typedef struct gs_core_meta_registry_s
{
    gs_slot_array(gs_core_meta_info_t)     info;
    gs_meta_registry_t                  registry; 
} gs_core_meta_registry_t;

// TODO(john): Turn all of these into function pointers that can be reloaded with meta generation

GS_API_DECL gs_core_meta_registry_t* 
gs_core_meta_registry_instance(); 

GS_API_DECL const gs_core_meta_info_t* 
gs_core_meta_obj_info(const gs_core_obj_t* obj); 

GS_API_DECL const gs_core_meta_info_t* 
gs_core_meta_obj_info_w_cls_id(uint32_t id); 

GS_API_DECL gs_core_obj_t*
gs_core_meta_static_ref_w_cls_id(uint32_t id); 

//===[ Object Internal / Defaults ]===//

GS_API_DECL bool
gs_core_info_base_of_internal(const gs_core_meta_info_t* info, uint32_t compare); 

GS_API_DECL bool
gs_core_info_derived_from_internal(const gs_core_meta_info_t* info, uint32_t compare); 

GS_API_DECL gs_result 
gs_core_obj_serialize_impl(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj);

GS_API_DECL gs_result 
gs_core_obj_deserialize_impl(gs_byte_buffer_t* buffer, gs_core_obj_t* obj); 

GS_API_DECL gs_result
gs_core_obj_net_serialize_impl(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj);

GS_API_DECL gs_result
gs_core_obj_net_deserialize_impl(gs_byte_buffer_t* buffer, gs_core_obj_t* obj);

//====[ Debug ]====//

GS_API_DECL void
gs_core_obj_print(const gs_core_obj_t* obj);


#endif // GS_CORE_OBJECT_H


































