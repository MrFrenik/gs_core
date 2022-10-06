/*==============================================================================================================
	* Copyright: 2022 John Jackson
	* File: gs_core_generated.c
	* Note: This file has been generated. Any and ALL local changes will be lost.

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
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

==============================================================================================================*/

// Includes
#include "gs_core_generated.h"

#include "core/generated/gs_core_generated.h"

// Class Ids
#define gs_core_asset_texture_t_cls_id 1
#define gs_core_app_t_cls_id 2
#define gs_core_asset_t_cls_id 3
#define gs_core_asset_pipeline_t_cls_id 4
#define gs_core_asset_font_t_cls_id 5
#define gs_core_asset_mesh_t_cls_id 6
#define gs_core_asset_audio_t_cls_id 7
#define gs_core_network_rpc_t_cls_id 8
#define gs_core_obj_t_cls_id 9
#define gs_core_asset_ui_stylesheet_t_cls_id 10
#define gs_core_entities_component_t_cls_id 11
#define gs_core_asset_material_t_cls_id 12
#define gs_core_entities_system_t_cls_id 13
#define gs_core_graphics_renderpass_t_cls_id 14

/* gs_core_asset_texture_t */

GS_API_DECL uint32_t
gs_core_asset_texture_t_class_id()
{
	return gs_core_asset_texture_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_texture_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_texture_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_texture_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_texture_t_cls_id);
}

GS_API_DECL void
gs_core_asset_texture_t_init(gs_core_asset_texture_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_texture_t_class_id();
}

GS_API_DECL gs_core_asset_texture_t*
gs_core_asset_texture_t_new()
{
	gs_core_asset_texture_t* obj = gs_malloc_init(gs_core_asset_texture_t);
	gs_core_asset_texture_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_texture_t
gs_core_asset_texture_t_ctor()
{
	gs_core_asset_texture_t obj = gs_default_val();
	gs_core_asset_texture_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_texture_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_texture_t_vtable_t_init(gs_core_asset_texture_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->importer = gs_core_asset_texture_importer;
	vt->serialize = gs_core_asset_texture_serialize;
	vt->deserialize = gs_core_asset_texture_deserialize;
	vt->cls_id = gs_core_asset_texture_t_class_id;
	vt->cls = gs_core_asset_texture_t_class;
}

GS_API_DECL gs_core_asset_texture_t_vtable_t
gs_core_asset_texture_t_vtable_t_ctor()
{
	gs_core_asset_texture_t_vtable_t vt = gs_default_val();
	gs_core_asset_texture_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_texture_t_vtable_t_new()
{
	gs_core_asset_texture_t_vtable_t* vt = gs_malloc_init(gs_core_asset_texture_t_vtable_t);
	gs_core_asset_texture_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_app_t */

GS_API_DECL uint32_t
gs_core_app_t_class_id()
{
	return gs_core_app_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_app_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_app_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_app_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_app_t_cls_id);
}

GS_API_DECL void
gs_core_app_t_init(gs_core_app_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_app_t_class_id();
}

GS_API_DECL gs_core_app_t*
gs_core_app_t_new()
{
	gs_core_app_t* obj = gs_malloc_init(gs_core_app_t);
	gs_core_app_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_app_t
gs_core_app_t_ctor()
{
	gs_core_app_t obj = gs_default_val();
	gs_core_app_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_app_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_app_t_vtable_t_init(gs_core_app_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->cls_id = gs_core_app_t_class_id;
	vt->cls = gs_core_app_t_class;
}

GS_API_DECL gs_core_app_t_vtable_t
gs_core_app_t_vtable_t_ctor()
{
	gs_core_app_t_vtable_t vt = gs_default_val();
	gs_core_app_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_app_t_vtable_t_new()
{
	gs_core_app_t_vtable_t* vt = gs_malloc_init(gs_core_app_t_vtable_t);
	gs_core_app_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_t */

GS_API_DECL uint32_t
gs_core_asset_t_class_id()
{
	return gs_core_asset_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_t_cls_id);
}

GS_API_DECL void
gs_core_asset_t_init(gs_core_asset_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_t_class_id();
}

GS_API_DECL gs_core_asset_t*
gs_core_asset_t_new()
{
	gs_core_asset_t* obj = gs_malloc_init(gs_core_asset_t);
	gs_core_asset_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_t
gs_core_asset_t_ctor()
{
	gs_core_asset_t obj = gs_default_val();
	gs_core_asset_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_t_vtable_t_init(gs_core_asset_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->importer = NULL;
	vt->cls_id = gs_core_asset_t_class_id;
	vt->cls = gs_core_asset_t_class;
}

GS_API_DECL gs_core_asset_t_vtable_t
gs_core_asset_t_vtable_t_ctor()
{
	gs_core_asset_t_vtable_t vt = gs_default_val();
	gs_core_asset_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_t_vtable_t_new()
{
	gs_core_asset_t_vtable_t* vt = gs_malloc_init(gs_core_asset_t_vtable_t);
	gs_core_asset_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_pipeline_t */

GS_API_DECL uint32_t
gs_core_asset_pipeline_t_class_id()
{
	return gs_core_asset_pipeline_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_pipeline_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_pipeline_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_pipeline_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_pipeline_t_cls_id);
}

GS_API_DECL void
gs_core_asset_pipeline_t_init(gs_core_asset_pipeline_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_pipeline_t_class_id();
}

GS_API_DECL gs_core_asset_pipeline_t*
gs_core_asset_pipeline_t_new()
{
	gs_core_asset_pipeline_t* obj = gs_malloc_init(gs_core_asset_pipeline_t);
	gs_core_asset_pipeline_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_pipeline_t
gs_core_asset_pipeline_t_ctor()
{
	gs_core_asset_pipeline_t obj = gs_default_val();
	gs_core_asset_pipeline_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_pipeline_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_pipeline_t_vtable_t_init(gs_core_asset_pipeline_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_pipeline_t_class_id;
	vt->cls = gs_core_asset_pipeline_t_class;
}

GS_API_DECL gs_core_asset_pipeline_t_vtable_t
gs_core_asset_pipeline_t_vtable_t_ctor()
{
	gs_core_asset_pipeline_t_vtable_t vt = gs_default_val();
	gs_core_asset_pipeline_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_pipeline_t_vtable_t_new()
{
	gs_core_asset_pipeline_t_vtable_t* vt = gs_malloc_init(gs_core_asset_pipeline_t_vtable_t);
	gs_core_asset_pipeline_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_font_t */

GS_API_DECL uint32_t
gs_core_asset_font_t_class_id()
{
	return gs_core_asset_font_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_font_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_font_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_font_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_font_t_cls_id);
}

GS_API_DECL void
gs_core_asset_font_t_init(gs_core_asset_font_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_font_t_class_id();
}

GS_API_DECL gs_core_asset_font_t*
gs_core_asset_font_t_new()
{
	gs_core_asset_font_t* obj = gs_malloc_init(gs_core_asset_font_t);
	gs_core_asset_font_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_font_t
gs_core_asset_font_t_ctor()
{
	gs_core_asset_font_t obj = gs_default_val();
	gs_core_asset_font_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_font_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_font_t_vtable_t_init(gs_core_asset_font_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_font_t_class_id;
	vt->cls = gs_core_asset_font_t_class;
}

GS_API_DECL gs_core_asset_font_t_vtable_t
gs_core_asset_font_t_vtable_t_ctor()
{
	gs_core_asset_font_t_vtable_t vt = gs_default_val();
	gs_core_asset_font_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_font_t_vtable_t_new()
{
	gs_core_asset_font_t_vtable_t* vt = gs_malloc_init(gs_core_asset_font_t_vtable_t);
	gs_core_asset_font_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_mesh_t */

GS_API_DECL uint32_t
gs_core_asset_mesh_t_class_id()
{
	return gs_core_asset_mesh_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_mesh_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_mesh_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_mesh_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_mesh_t_cls_id);
}

GS_API_DECL void
gs_core_asset_mesh_t_init(gs_core_asset_mesh_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_mesh_t_class_id();
}

GS_API_DECL gs_core_asset_mesh_t*
gs_core_asset_mesh_t_new()
{
	gs_core_asset_mesh_t* obj = gs_malloc_init(gs_core_asset_mesh_t);
	gs_core_asset_mesh_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_mesh_t
gs_core_asset_mesh_t_ctor()
{
	gs_core_asset_mesh_t obj = gs_default_val();
	gs_core_asset_mesh_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_mesh_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_mesh_t_vtable_t_init(gs_core_asset_mesh_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_mesh_t_class_id;
	vt->cls = gs_core_asset_mesh_t_class;
}

GS_API_DECL gs_core_asset_mesh_t_vtable_t
gs_core_asset_mesh_t_vtable_t_ctor()
{
	gs_core_asset_mesh_t_vtable_t vt = gs_default_val();
	gs_core_asset_mesh_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_mesh_t_vtable_t_new()
{
	gs_core_asset_mesh_t_vtable_t* vt = gs_malloc_init(gs_core_asset_mesh_t_vtable_t);
	gs_core_asset_mesh_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_audio_t */

GS_API_DECL uint32_t
gs_core_asset_audio_t_class_id()
{
	return gs_core_asset_audio_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_audio_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_audio_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_audio_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_audio_t_cls_id);
}

GS_API_DECL void
gs_core_asset_audio_t_init(gs_core_asset_audio_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_audio_t_class_id();
}

GS_API_DECL gs_core_asset_audio_t*
gs_core_asset_audio_t_new()
{
	gs_core_asset_audio_t* obj = gs_malloc_init(gs_core_asset_audio_t);
	gs_core_asset_audio_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_audio_t
gs_core_asset_audio_t_ctor()
{
	gs_core_asset_audio_t obj = gs_default_val();
	gs_core_asset_audio_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_audio_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_audio_t_vtable_t_init(gs_core_asset_audio_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_audio_t_class_id;
	vt->cls = gs_core_asset_audio_t_class;
}

GS_API_DECL gs_core_asset_audio_t_vtable_t
gs_core_asset_audio_t_vtable_t_ctor()
{
	gs_core_asset_audio_t_vtable_t vt = gs_default_val();
	gs_core_asset_audio_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_audio_t_vtable_t_new()
{
	gs_core_asset_audio_t_vtable_t* vt = gs_malloc_init(gs_core_asset_audio_t_vtable_t);
	gs_core_asset_audio_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_network_rpc_t */

GS_API_DECL uint32_t
gs_core_network_rpc_t_class_id()
{
	return gs_core_network_rpc_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_t_cls_id);
}

GS_API_DECL void
gs_core_network_rpc_t_init(gs_core_network_rpc_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_network_rpc_t_class_id();
}

GS_API_DECL gs_core_network_rpc_t*
gs_core_network_rpc_t_new()
{
	gs_core_network_rpc_t* obj = gs_malloc_init(gs_core_network_rpc_t);
	gs_core_network_rpc_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_network_rpc_t
gs_core_network_rpc_t_ctor()
{
	gs_core_network_rpc_t obj = gs_default_val();
	gs_core_network_rpc_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_network_rpc_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_network_rpc_t_vtable_t_init(gs_core_network_rpc_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->cls_id = gs_core_network_rpc_t_class_id;
	vt->cls = gs_core_network_rpc_t_class;
}

GS_API_DECL gs_core_network_rpc_t_vtable_t
gs_core_network_rpc_t_vtable_t_ctor()
{
	gs_core_network_rpc_t_vtable_t vt = gs_default_val();
	gs_core_network_rpc_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_network_rpc_t_vtable_t_new()
{
	gs_core_network_rpc_t_vtable_t* vt = gs_malloc_init(gs_core_network_rpc_t_vtable_t);
	gs_core_network_rpc_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_obj_t */

GS_API_DECL uint32_t
gs_core_obj_t_class_id()
{
	return gs_core_obj_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_obj_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_obj_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_obj_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_obj_t_cls_id);
}

GS_API_DECL void
gs_core_obj_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_obj_t_class_id();
}

GS_API_DECL gs_core_obj_t*
gs_core_obj_t_new()
{
	gs_core_obj_t* obj = gs_malloc_init(gs_core_obj_t);
	gs_core_obj_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_obj_t
gs_core_obj_t_ctor()
{
	gs_core_obj_t obj = gs_default_val();
	gs_core_obj_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_obj_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_obj_t_vtable_t_init(gs_core_obj_t_vtable_t* vt)
{
	vt->serialize = gs_core_obj_serialize_impl;
	vt->deserialize = gs_core_obj_deserialize_impl;
	vt->net_serialize = gs_core_obj_net_serialize_impl;
	vt->net_deserialize = gs_core_obj_net_deserialize_impl;
	vt->cls_id = gs_core_obj_t_class_id;
	vt->cls = gs_core_obj_t_class;
}

GS_API_DECL gs_core_obj_t_vtable_t
gs_core_obj_t_vtable_t_ctor()
{
	gs_core_obj_t_vtable_t vt = gs_default_val();
	gs_core_obj_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_obj_t_vtable_t_new()
{
	gs_core_obj_t_vtable_t* vt = gs_malloc_init(gs_core_obj_t_vtable_t);
	gs_core_obj_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_ui_stylesheet_t */

GS_API_DECL uint32_t
gs_core_asset_ui_stylesheet_t_class_id()
{
	return gs_core_asset_ui_stylesheet_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_ui_stylesheet_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_ui_stylesheet_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_ui_stylesheet_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_ui_stylesheet_t_cls_id);
}

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_init(gs_core_asset_ui_stylesheet_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_ui_stylesheet_t_class_id();
}

GS_API_DECL gs_core_asset_ui_stylesheet_t*
gs_core_asset_ui_stylesheet_t_new()
{
	gs_core_asset_ui_stylesheet_t* obj = gs_malloc_init(gs_core_asset_ui_stylesheet_t);
	gs_core_asset_ui_stylesheet_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_ui_stylesheet_t
gs_core_asset_ui_stylesheet_t_ctor()
{
	gs_core_asset_ui_stylesheet_t obj = gs_default_val();
	gs_core_asset_ui_stylesheet_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_vtable_t_init(gs_core_asset_ui_stylesheet_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_ui_stylesheet_t_class_id;
	vt->cls = gs_core_asset_ui_stylesheet_t_class;
}

GS_API_DECL gs_core_asset_ui_stylesheet_t_vtable_t
gs_core_asset_ui_stylesheet_t_vtable_t_ctor()
{
	gs_core_asset_ui_stylesheet_t_vtable_t vt = gs_default_val();
	gs_core_asset_ui_stylesheet_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_ui_stylesheet_t_vtable_t_new()
{
	gs_core_asset_ui_stylesheet_t_vtable_t* vt = gs_malloc_init(gs_core_asset_ui_stylesheet_t_vtable_t);
	gs_core_asset_ui_stylesheet_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_entities_component_t */

GS_API_DECL uint32_t
gs_core_entities_component_t_class_id()
{
	return gs_core_entities_component_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_entities_component_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_entities_component_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_entities_component_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_entities_component_t_cls_id);
}

GS_API_DECL void
gs_core_entities_component_t_init(gs_core_entities_component_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_entities_component_t_class_id();
}

GS_API_DECL gs_core_entities_component_t*
gs_core_entities_component_t_new()
{
	gs_core_entities_component_t* obj = gs_malloc_init(gs_core_entities_component_t);
	gs_core_entities_component_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_entities_component_t
gs_core_entities_component_t_ctor()
{
	gs_core_entities_component_t obj = gs_default_val();
	gs_core_entities_component_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_entities_component_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_entities_component_t_vtable_t_init(gs_core_entities_component_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->cls_id = gs_core_entities_component_t_class_id;
	vt->cls = gs_core_entities_component_t_class;
}

GS_API_DECL gs_core_entities_component_t_vtable_t
gs_core_entities_component_t_vtable_t_ctor()
{
	gs_core_entities_component_t_vtable_t vt = gs_default_val();
	gs_core_entities_component_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_entities_component_t_vtable_t_new()
{
	gs_core_entities_component_t_vtable_t* vt = gs_malloc_init(gs_core_entities_component_t_vtable_t);
	gs_core_entities_component_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_material_t */

GS_API_DECL uint32_t
gs_core_asset_material_t_class_id()
{
	return gs_core_asset_material_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_material_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_material_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_material_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_material_t_cls_id);
}

GS_API_DECL void
gs_core_asset_material_t_init(gs_core_asset_material_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_material_t_class_id();
}

GS_API_DECL gs_core_asset_material_t*
gs_core_asset_material_t_new()
{
	gs_core_asset_material_t* obj = gs_malloc_init(gs_core_asset_material_t);
	gs_core_asset_material_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_asset_material_t
gs_core_asset_material_t_ctor()
{
	gs_core_asset_material_t obj = gs_default_val();
	gs_core_asset_material_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_material_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_material_t_vtable_t_init(gs_core_asset_material_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_material_t_class_id;
	vt->cls = gs_core_asset_material_t_class;
}

GS_API_DECL gs_core_asset_material_t_vtable_t
gs_core_asset_material_t_vtable_t_ctor()
{
	gs_core_asset_material_t_vtable_t vt = gs_default_val();
	gs_core_asset_material_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_material_t_vtable_t_new()
{
	gs_core_asset_material_t_vtable_t* vt = gs_malloc_init(gs_core_asset_material_t_vtable_t);
	gs_core_asset_material_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_entities_system_t */

GS_API_DECL uint32_t
gs_core_entities_system_t_class_id()
{
	return gs_core_entities_system_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_entities_system_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_entities_system_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_entities_system_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_entities_system_t_cls_id);
}

GS_API_DECL void
gs_core_entities_system_t_init(gs_core_entities_system_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_entities_system_t_class_id();
}

GS_API_DECL gs_core_entities_system_t*
gs_core_entities_system_t_new()
{
	gs_core_entities_system_t* obj = gs_malloc_init(gs_core_entities_system_t);
	gs_core_entities_system_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_entities_system_t
gs_core_entities_system_t_ctor()
{
	gs_core_entities_system_t obj = gs_default_val();
	gs_core_entities_system_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_entities_system_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_entities_system_t_vtable_t_init(gs_core_entities_system_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->cls_id = gs_core_entities_system_t_class_id;
	vt->cls = gs_core_entities_system_t_class;
}

GS_API_DECL gs_core_entities_system_t_vtable_t
gs_core_entities_system_t_vtable_t_ctor()
{
	gs_core_entities_system_t_vtable_t vt = gs_default_val();
	gs_core_entities_system_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_entities_system_t_vtable_t_new()
{
	gs_core_entities_system_t_vtable_t* vt = gs_malloc_init(gs_core_entities_system_t_vtable_t);
	gs_core_entities_system_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_graphics_renderpass_t */

GS_API_DECL uint32_t
gs_core_graphics_renderpass_t_class_id()
{
	return gs_core_graphics_renderpass_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_graphics_renderpass_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_graphics_renderpass_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_graphics_renderpass_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_graphics_renderpass_t_cls_id);
}

GS_API_DECL void
gs_core_graphics_renderpass_t_init(gs_core_graphics_renderpass_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_graphics_renderpass_t_class_id();
}

GS_API_DECL gs_core_graphics_renderpass_t*
gs_core_graphics_renderpass_t_new()
{
	gs_core_graphics_renderpass_t* obj = gs_malloc_init(gs_core_graphics_renderpass_t);
	gs_core_graphics_renderpass_t_init(obj);
	return obj;
}

GS_API_DECL gs_core_graphics_renderpass_t
gs_core_graphics_renderpass_t_ctor()
{
	gs_core_graphics_renderpass_t obj = gs_default_val();
	gs_core_graphics_renderpass_t_init(&obj);
	return obj;
}

GS_API_DECL void
gs_core_graphics_renderpass_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_graphics_renderpass_t_vtable_t_init(gs_core_graphics_renderpass_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->execute = NULL;
	vt->cls_id = gs_core_graphics_renderpass_t_class_id;
	vt->cls = gs_core_graphics_renderpass_t_class;
}

GS_API_DECL gs_core_graphics_renderpass_t_vtable_t
gs_core_graphics_renderpass_t_vtable_t_ctor()
{
	gs_core_graphics_renderpass_t_vtable_t vt = gs_default_val();
	gs_core_graphics_renderpass_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_graphics_renderpass_t_vtable_t_new()
{
	gs_core_graphics_renderpass_t_vtable_t* vt = gs_malloc_init(gs_core_graphics_renderpass_t_vtable_t);
	gs_core_graphics_renderpass_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

//============[ Registry ]===============//

//Static Objects
static gs_core_asset_texture_t g_1 = {0};
static gs_core_app_t g_2 = {0};
static gs_core_asset_t g_3 = {0};
static gs_core_asset_pipeline_t g_4 = {0};
static gs_core_asset_font_t g_5 = {0};
static gs_core_asset_mesh_t g_6 = {0};
static gs_core_asset_audio_t g_7 = {0};
static gs_core_network_rpc_t g_8 = {0};
static gs_core_obj_t g_9 = {0};
static gs_core_asset_ui_stylesheet_t g_10 = {0};
static gs_core_entities_component_t g_11 = {0};
static gs_core_asset_material_t g_12 = {0};
static gs_core_entities_system_t g_13 = {0};
static gs_core_graphics_renderpass_t g_14 = {0};

GS_API_DECL void
gs_core_meta_register()
{
	gs_core_meta_registry_t* meta = gs_core_instance()->meta;
	gs_meta_class_t* cls = NULL;
	gs_meta_class_t* base = NULL;
	gs_core_meta_info_t* info = NULL;
	gs_core_network_rpc_t* rpc = NULL;
	uint64_t cid = 0;
	uint32_t cnt = 0;
	uint32_t idx = 0;

	/* gs_core_asset_texture_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_texture_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_texture_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_1, gs_core_base_t)->id = gs_core_asset_texture_t_class_id();
		info->instance = gs_core_cast(&g_1, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_texture_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_texture_t_vtable_t_new();
		gs_core_asset_texture_t_vtable_t_init(info->vtable);
	}

	/* gs_core_app_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_app_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_app_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_2, gs_core_base_t)->id = gs_core_app_t_class_id();
		info->instance = gs_core_cast(&g_2, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_app_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_app_t_vtable_t_new();
		gs_core_app_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_t),
			.properties = (gs_meta_property_t[]) {
				gs_meta_property(gs_core_asset_t, uint32_t, record_hndl, GS_META_PROPERTY_TYPE_INFO_U32)
			},
			.size = 1 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_3, gs_core_base_t)->id = gs_core_asset_t_class_id();
		info->instance = gs_core_cast(&g_3, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_asset_t_vtable_t_new();
		gs_core_asset_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_pipeline_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_pipeline_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_pipeline_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_4, gs_core_base_t)->id = gs_core_asset_pipeline_t_class_id();
		info->instance = gs_core_cast(&g_4, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_pipeline_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_pipeline_t_vtable_t_new();
		gs_core_asset_pipeline_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_font_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_font_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_font_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_5, gs_core_base_t)->id = gs_core_asset_font_t_class_id();
		info->instance = gs_core_cast(&g_5, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_font_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_font_t_vtable_t_new();
		gs_core_asset_font_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_mesh_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_mesh_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_mesh_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_6, gs_core_base_t)->id = gs_core_asset_mesh_t_class_id();
		info->instance = gs_core_cast(&g_6, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_mesh_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_mesh_t_vtable_t_new();
		gs_core_asset_mesh_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_audio_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_audio_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_audio_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_7, gs_core_base_t)->id = gs_core_asset_audio_t_class_id();
		info->instance = gs_core_cast(&g_7, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_audio_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_audio_t_vtable_t_new();
		gs_core_asset_audio_t_vtable_t_init(info->vtable);
	}

	/* gs_core_network_rpc_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_network_rpc_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_network_rpc_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_8, gs_core_base_t)->id = gs_core_network_rpc_t_class_id();
		info->instance = gs_core_cast(&g_8, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_network_rpc_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_network_rpc_t_vtable_t_new();
		gs_core_network_rpc_t_vtable_t_init(info->vtable);
	}

	/* gs_core_obj_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_obj_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_obj_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_9, gs_core_base_t)->id = gs_core_obj_t_class_id();
		info->instance = gs_core_cast(&g_9, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_obj_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_obj_t_vtable_t_new();
		gs_core_obj_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_ui_stylesheet_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_ui_stylesheet_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_ui_stylesheet_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_10, gs_core_base_t)->id = gs_core_asset_ui_stylesheet_t_class_id();
		info->instance = gs_core_cast(&g_10, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_ui_stylesheet_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_ui_stylesheet_t_vtable_t_new();
		gs_core_asset_ui_stylesheet_t_vtable_t_init(info->vtable);
	}

	/* gs_core_entities_component_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_entities_component_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_entities_component_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_11, gs_core_base_t)->id = gs_core_entities_component_t_class_id();
		info->instance = gs_core_cast(&g_11, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_entities_component_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_entities_component_t_vtable_t_new();
		gs_core_entities_component_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_material_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_material_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_material_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_12, gs_core_base_t)->id = gs_core_asset_material_t_class_id();
		info->instance = gs_core_cast(&g_12, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_material_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_material_t_vtable_t_new();
		gs_core_asset_material_t_vtable_t_init(info->vtable);
	}

	/* gs_core_entities_system_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_entities_system_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_entities_system_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_13, gs_core_base_t)->id = gs_core_entities_system_t_class_id();
		info->instance = gs_core_cast(&g_13, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_entities_system_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_entities_system_t_vtable_t_new();
		gs_core_entities_system_t_vtable_t_init(info->vtable);
	}

	/* gs_core_graphics_renderpass_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_graphics_renderpass_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_graphics_renderpass_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_14, gs_core_base_t)->id = gs_core_graphics_renderpass_t_class_id();
		info->instance = gs_core_cast(&g_14, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_graphics_renderpass_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_graphics_renderpass_t_vtable_t_new();
		gs_core_graphics_renderpass_t_vtable_t_init(info->vtable);
	}

}

GS_API_DECL void
gs_core_meta_unregister()
{
	gs_core_entities_t* ents = gs_core_instance()->entities;

	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_texture_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_app_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_pipeline_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_font_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_mesh_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_audio_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_network_rpc_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_obj_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_ui_stylesheet_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_entities_component_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_material_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_entities_system_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_graphics_renderpass_t_cls_id);
		gs_assert(info);
		if (info->cls) {
			gs_meta_class_unregister(&meta->registry, info->cls->id);
		}
		info->cls = NULL;
		info->base = NULL;
		if (info->vtable)
		{
			gs_free(info->vtable);
			info->vtable = NULL;
		}
	}
}

//=====[ Component/System Registration ]=====//

GS_API_DECL void
gs_core_meta_ecs_register()
{
	gs_core_entities_t* ents = gs_core_instance()->entities;

	// Components


	// Systems

}

//======[ Systems ]======//

