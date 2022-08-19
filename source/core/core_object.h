/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: sm_object.h

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

#ifndef CORE_OBJECT_H
#define CORE_OBJECT_H 

#include <gs/gs.h>
#include <gs/util/gs_meta.h>

// Macros
#define core_base(T)        T _base
#define core_cast(OBJ, T)   ((T*)(OBJ)) 

// Static ref
#define core_obj_static_ref_w_cls_id(INFO)   (core_meta_static_ref_w_cls_id((INFO))
#define core_obj_static_ref_w_info(INFO)     (core_meta_static_ref_w_cls_id((INFO)->cid))

// Info
#define core_info_base_of(INFO, T)  (core_info_base_of_internal((INFO), core_cls_cid(T)))
#define core_info_w_cls_id(ID)      (core_meta_obj_info_w_cls_id((ID)))

// Class id
#define core_cls_cid(T)       T##_class_id()
#define core_cls_info(T)      T##_info()
#define core_cls_init(T, OBJ) T##_init(OBJ)
#define core_cls_new(T)       T##_new()
#define core_cls_ctor(T)      T##_ctor()

// Object
#define core_obj_cls_id(OBJ)                  (core_cast((OBJ), core_obj_t)->id)
#define core_obj_info(OBJ)                    (core_meta_obj_info((OBJ)))
#define core_obj_cls(OBJ)                     (core_obj_info((OBJ))->cls)
#define core_obj_vt(OBJ)                      (core_obj_info((OBJ))->vtable)
#define core_obj_cid(OBJ)                     (core_obj_vt((OBJ)).class_id())
#define core_obj_dtor(OBJ)                    (core_obj_vt((OBJ)).dtor((OBJ)))
#define core_obj_serialize(BUFFER, OBJ)       (core_obj_vt((OBJ)).serialize((BUFFER), (OBJ)))
#define core_obj_deserialize(BUFFER, OBJ)     (core_obj_vt((OBJ)).deserialize((BUFFER), (OBJ)))
#define core_obj_net_serialize(BUFFER, OBJ)   (core_obj_vt((OBJ)).net_serialize((BUFFER), (OBJ)))
#define core_obj_net_deserialize(BUFFER, OBJ) (core_obj_vt((OBJ)).net_deserialize((BUFFER), (OBJ)))

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

// Forward Decls.
struct core_meta_info_s;
struct core_obj_s;

typedef struct
{ 
    gs_meta_class_t* (* class)();
    gs_result (* serialize)(gs_byte_buffer_t* buffer, const struct core_obj_s* obj);
    gs_result (* deserialize)(gs_byte_buffer_t* buffer, struct core_obj_s* obj);
    gs_result (* net_serialize)(gs_byte_buffer_t* buffer, const struct core_obj_s* obj);
    gs_result (* net_deserialize)(gs_byte_buffer_t* buffer, struct core_obj_s* obj);
    uint64_t (* class_id)();
    struct core_meta_info_s* (* info)();
} core_vtable_t;

typedef struct core_meta_info_s
{ 
    gs_meta_class_t* cls;
    struct core_meta_info_s* base;
    core_vtable_t vtable;
    uint32_t cid;
    size_t size;
} core_meta_info_t;

_introspect()
typedef struct core_obj_s
{ 
    uint32_t id;
} core_obj_t;

//===[ Meta Generated ]===//

GS_API_DECL const core_meta_info_t* 
core_meta_obj_info(core_obj_t* obj);

GS_API_DECL const core_meta_info_t* 
core_meta_obj_info_w_cls_id(uint32_t id);

GS_API_DECL core_obj_t*
core_meta_static_ref_w_cls_id(uint32_t id);

//===[ Object Internal / Defaults ]===//

GS_API_DECL bool
core_info_base_of_internal(const core_meta_info_t* info, uint32_t compare); 

GS_API_DECL gs_result 
core_obj_serialize_default(gs_byte_buffer_t* buffer, const core_obj_t* obj);

GS_API_DECL gs_result 
core_obj_deserialize_default(gs_byte_buffer_t* buffer, core_obj_t* obj); 

GS_API_DECL gs_result
core_obj_net_serialize_default(gs_byte_buffer_t* buffer, const core_obj_t* obj);

GS_API_DECL gs_result
core_obj_net_deserialize_default(gs_byte_buffer_t* buffer, core_obj_t* obj);

//====[ Debug ]====//

GS_API_DECL void
core_obj_print(const core_obj_t* obj);


#endif // CORE_OBJECT_H


































