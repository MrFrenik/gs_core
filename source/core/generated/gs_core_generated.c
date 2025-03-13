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

// CVars

//======[ Lambdas ]=======//


// Class Ids
#define gs_core_asset_handle_t_cls_id 1
#define gs_core_app_t_cls_id 2
#define gs_core_asset_t_cls_id 3
#define gs_core_asset_texture_t_cls_id 4
#define gs_core_asset_pipeline_t_cls_id 5
#define gs_core_asset_mesh_t_cls_id 6
#define gs_core_asset_font_t_cls_id 7
#define gs_core_obj_t_cls_id 8
#define gs_core_asset_ui_stylesheet_t_cls_id 9
#define gs_core_entities_component_t_cls_id 10
#define gs_core_asset_material_t_cls_id 11
#define gs_core_component_tag_t_cls_id 12
#define gs_core_system_renderable_t_cls_id 13
#define gs_core_component_transform_t_cls_id 14
#define gs_core_asset_script_t_cls_id 15
#define gs_core_asset_audio_t_cls_id 16
#define gs_core_component_renderable_t_cls_id 17
#define gs_core_network_rpc_t_cls_id 18
#define gs_core_network_rpc_unreliable_t_cls_id 19
#define gs_core_entities_system_t_cls_id 20
#define gs_core_graphics_renderpass_t_cls_id 21
#define gs_core_network_rpc_reliable_t_cls_id 22

/* gs_core_asset_handle_t */

GS_API_DECL uint32_t
gs_core_asset_handle_t_class_id()
{
	return gs_core_asset_handle_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_handle_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_handle_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_handle_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_handle_t_cls_id);
}

GS_API_DECL void
gs_core_asset_handle_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_handle_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_handle_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_handle_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_handle_t*
gs_core_asset_handle_t_new()
{
	gs_core_asset_handle_t* obj = gs_malloc_init(gs_core_asset_handle_t);
	gs_core_asset_handle_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_handle_t
gs_core_asset_handle_t_ctor()
{
	gs_core_asset_handle_t obj = gs_default_val();
	gs_core_asset_handle_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_handle_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_handle_t_vtable_t_init(gs_core_asset_handle_t_vtable_t* vt)
{
	gs_core_obj_t_vtable_t_init((gs_core_obj_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_handle_t_class_id;
	vt->cls = gs_core_asset_handle_t_class;
	vt->init = gs_core_asset_handle_t_init;
	vt->dtor = gs_core_asset_handle_t_dtor;
}

GS_API_DECL gs_core_asset_handle_t_vtable_t
gs_core_asset_handle_t_vtable_t_ctor()
{
	gs_core_asset_handle_t_vtable_t vt = gs_default_val();
	gs_core_asset_handle_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_handle_t_vtable_t_new()
{
	gs_core_asset_handle_t_vtable_t* vt = gs_malloc_init(gs_core_asset_handle_t_vtable_t);
	gs_core_asset_handle_t_vtable_t_init(vt);
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
gs_core_app_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_app_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_app_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_app_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_app_t*
gs_core_app_t_new()
{
	gs_core_app_t* obj = gs_malloc_init(gs_core_app_t);
	gs_core_app_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_app_t
gs_core_app_t_ctor()
{
	gs_core_app_t obj = gs_default_val();
	gs_core_app_t_init((gs_core_obj_t*)&obj);
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
	vt->app_shutdown = NULL;
	vt->app_init = NULL;
	vt->app_update = NULL;
	vt->app_editor = NULL;
	vt->app_render = NULL;
	vt->cls_id = gs_core_app_t_class_id;
	vt->cls = gs_core_app_t_class;
	vt->init = gs_core_app_t_init;
	vt->dtor = gs_core_app_t_dtor;
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
gs_core_asset_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_t*
gs_core_asset_t_new()
{
	gs_core_asset_t* obj = gs_malloc_init(gs_core_asset_t);
	gs_core_asset_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_t
gs_core_asset_t_ctor()
{
	gs_core_asset_t obj = gs_default_val();
	gs_core_asset_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_t_init;
	vt->dtor = gs_core_asset_t_dtor;
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
gs_core_asset_texture_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_texture_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_texture_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_texture_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_texture_t*
gs_core_asset_texture_t_new()
{
	gs_core_asset_texture_t* obj = gs_malloc_init(gs_core_asset_texture_t);
	gs_core_asset_texture_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_texture_t
gs_core_asset_texture_t_ctor()
{
	gs_core_asset_texture_t obj = gs_default_val();
	gs_core_asset_texture_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_texture_t_init;
	vt->dtor = gs_core_asset_texture_t_dtor;
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
gs_core_asset_pipeline_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_pipeline_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_pipeline_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_pipeline_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_pipeline_t*
gs_core_asset_pipeline_t_new()
{
	gs_core_asset_pipeline_t* obj = gs_malloc_init(gs_core_asset_pipeline_t);
	gs_core_asset_pipeline_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_pipeline_t
gs_core_asset_pipeline_t_ctor()
{
	gs_core_asset_pipeline_t obj = gs_default_val();
	gs_core_asset_pipeline_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_pipeline_t_init;
	vt->dtor = gs_core_asset_pipeline_t_dtor;
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
gs_core_asset_mesh_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_mesh_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_mesh_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_mesh_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_mesh_t*
gs_core_asset_mesh_t_new()
{
	gs_core_asset_mesh_t* obj = gs_malloc_init(gs_core_asset_mesh_t);
	gs_core_asset_mesh_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_mesh_t
gs_core_asset_mesh_t_ctor()
{
	gs_core_asset_mesh_t obj = gs_default_val();
	gs_core_asset_mesh_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_mesh_t_init;
	vt->dtor = gs_core_asset_mesh_t_dtor;
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
gs_core_asset_font_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_font_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_font_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_font_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_font_t*
gs_core_asset_font_t_new()
{
	gs_core_asset_font_t* obj = gs_malloc_init(gs_core_asset_font_t);
	gs_core_asset_font_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_font_t
gs_core_asset_font_t_ctor()
{
	gs_core_asset_font_t obj = gs_default_val();
	gs_core_asset_font_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_font_t_init;
	vt->dtor = gs_core_asset_font_t_dtor;
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
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_obj_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_obj_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_obj_t*
gs_core_obj_t_new()
{
	gs_core_obj_t* obj = gs_malloc_init(gs_core_obj_t);
	gs_core_obj_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_obj_t
gs_core_obj_t_ctor()
{
	gs_core_obj_t obj = gs_default_val();
	gs_core_obj_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_obj_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_obj_t_vtable_t_init(gs_core_obj_t_vtable_t* vt)
{
	vt->post_init = NULL;
	vt->net_serialize = gs_core_obj_net_serialize_impl;
	vt->serialize = gs_core_obj_serialize_impl;
	vt->net_deserialize = gs_core_obj_net_deserialize_impl;
	vt->deserialize = gs_core_obj_deserialize_impl;
	vt->cls_id = gs_core_obj_t_class_id;
	vt->cls = gs_core_obj_t_class;
	vt->init = gs_core_obj_t_init;
	vt->dtor = gs_core_obj_t_dtor;
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
gs_core_asset_ui_stylesheet_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_ui_stylesheet_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_ui_stylesheet_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_ui_stylesheet_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_ui_stylesheet_t*
gs_core_asset_ui_stylesheet_t_new()
{
	gs_core_asset_ui_stylesheet_t* obj = gs_malloc_init(gs_core_asset_ui_stylesheet_t);
	gs_core_asset_ui_stylesheet_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_ui_stylesheet_t
gs_core_asset_ui_stylesheet_t_ctor()
{
	gs_core_asset_ui_stylesheet_t obj = gs_default_val();
	gs_core_asset_ui_stylesheet_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_ui_stylesheet_t_init;
	vt->dtor = gs_core_asset_ui_stylesheet_t_dtor;
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
gs_core_entities_component_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_entities_component_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_entities_component_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_entities_component_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_entities_component_t*
gs_core_entities_component_t_new()
{
	gs_core_entities_component_t* obj = gs_malloc_init(gs_core_entities_component_t);
	gs_core_entities_component_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_entities_component_t
gs_core_entities_component_t_ctor()
{
	gs_core_entities_component_t obj = gs_default_val();
	gs_core_entities_component_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_entities_component_t_init;
	vt->dtor = gs_core_entities_component_t_dtor;
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
gs_core_asset_material_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_material_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_material_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_material_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_material_t*
gs_core_asset_material_t_new()
{
	gs_core_asset_material_t* obj = gs_malloc_init(gs_core_asset_material_t);
	gs_core_asset_material_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_material_t
gs_core_asset_material_t_ctor()
{
	gs_core_asset_material_t obj = gs_default_val();
	gs_core_asset_material_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_material_t_init;
	vt->dtor = gs_core_asset_material_t_dtor;
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

/* gs_core_component_tag_t */

GS_API_DECL uint32_t
gs_core_component_tag_t_class_id()
{
	return gs_core_component_tag_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_component_tag_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_tag_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_tag_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_tag_t_cls_id);
}

GS_API_DECL void
gs_core_component_tag_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_component_tag_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_component_tag_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_component_tag_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_component_tag_t*
gs_core_component_tag_t_new()
{
	gs_core_component_tag_t* obj = gs_malloc_init(gs_core_component_tag_t);
	gs_core_component_tag_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_component_tag_t
gs_core_component_tag_t_ctor()
{
	gs_core_component_tag_t obj = gs_default_val();
	gs_core_component_tag_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_component_tag_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_component_tag_t_vtable_t_init(gs_core_component_tag_t_vtable_t* vt)
{
	gs_core_entities_component_t_vtable_t_init((gs_core_entities_component_t_vtable_t*)vt);
	vt->cls_id = gs_core_component_tag_t_class_id;
	vt->cls = gs_core_component_tag_t_class;
	vt->init = gs_core_component_tag_t_init;
	vt->dtor = gs_core_component_tag_t_dtor;
}

GS_API_DECL gs_core_component_tag_t_vtable_t
gs_core_component_tag_t_vtable_t_ctor()
{
	gs_core_component_tag_t_vtable_t vt = gs_default_val();
	gs_core_component_tag_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_component_tag_t_vtable_t_new()
{
	gs_core_component_tag_t_vtable_t* vt = gs_malloc_init(gs_core_component_tag_t_vtable_t);
	gs_core_component_tag_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_system_renderable_t */

GS_API_DECL uint32_t
gs_core_system_renderable_t_class_id()
{
	return gs_core_system_renderable_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_system_renderable_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_system_renderable_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_system_renderable_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_system_renderable_t_cls_id);
}

GS_API_DECL void
gs_core_system_renderable_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_system_renderable_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_system_renderable_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_system_renderable_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_system_renderable_t*
gs_core_system_renderable_t_new()
{
	gs_core_system_renderable_t* obj = gs_malloc_init(gs_core_system_renderable_t);
	gs_core_system_renderable_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_system_renderable_t
gs_core_system_renderable_t_ctor()
{
	gs_core_system_renderable_t obj = gs_default_val();
	gs_core_system_renderable_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_system_renderable_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_system_renderable_t_vtable_t_init(gs_core_system_renderable_t_vtable_t* vt)
{
	gs_core_entities_system_t_vtable_t_init((gs_core_entities_system_t_vtable_t*)vt);
	vt->callback = gs_core_system_renderable_cb;
	vt->cls_id = gs_core_system_renderable_t_class_id;
	vt->cls = gs_core_system_renderable_t_class;
	vt->init = gs_core_system_renderable_t_init;
	vt->dtor = gs_core_system_renderable_t_dtor;
}

GS_API_DECL gs_core_system_renderable_t_vtable_t
gs_core_system_renderable_t_vtable_t_ctor()
{
	gs_core_system_renderable_t_vtable_t vt = gs_default_val();
	gs_core_system_renderable_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_system_renderable_t_vtable_t_new()
{
	gs_core_system_renderable_t_vtable_t* vt = gs_malloc_init(gs_core_system_renderable_t_vtable_t);
	gs_core_system_renderable_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_component_transform_t */

GS_API_DECL uint32_t
gs_core_component_transform_t_class_id()
{
	return gs_core_component_transform_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_component_transform_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_transform_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_transform_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_transform_t_cls_id);
}

GS_API_DECL void
gs_core_component_transform_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_component_transform_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_component_transform_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_component_transform_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_component_transform_t*
gs_core_component_transform_t_new()
{
	gs_core_component_transform_t* obj = gs_malloc_init(gs_core_component_transform_t);
	gs_core_component_transform_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_component_transform_t
gs_core_component_transform_t_ctor()
{
	gs_core_component_transform_t obj = gs_default_val();
	gs_core_component_transform_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_component_transform_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_component_transform_t_vtable_t_init(gs_core_component_transform_t_vtable_t* vt)
{
	gs_core_entities_component_t_vtable_t_init((gs_core_entities_component_t_vtable_t*)vt);
	vt->cls_id = gs_core_component_transform_t_class_id;
	vt->cls = gs_core_component_transform_t_class;
	vt->init = gs_core_component_transform_t_init;
	vt->dtor = gs_core_component_transform_t_dtor;
}

GS_API_DECL gs_core_component_transform_t_vtable_t
gs_core_component_transform_t_vtable_t_ctor()
{
	gs_core_component_transform_t_vtable_t vt = gs_default_val();
	gs_core_component_transform_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_component_transform_t_vtable_t_new()
{
	gs_core_component_transform_t_vtable_t* vt = gs_malloc_init(gs_core_component_transform_t_vtable_t);
	gs_core_component_transform_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

/* gs_core_asset_script_t */

GS_API_DECL uint32_t
gs_core_asset_script_t_class_id()
{
	return gs_core_asset_script_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_asset_script_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_script_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_script_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_asset_script_t_cls_id);
}

GS_API_DECL void
gs_core_asset_script_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_script_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_script_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_script_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_script_t*
gs_core_asset_script_t_new()
{
	gs_core_asset_script_t* obj = gs_malloc_init(gs_core_asset_script_t);
	gs_core_asset_script_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_script_t
gs_core_asset_script_t_ctor()
{
	gs_core_asset_script_t obj = gs_default_val();
	gs_core_asset_script_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_asset_script_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_asset_script_t_vtable_t_init(gs_core_asset_script_t_vtable_t* vt)
{
	gs_core_asset_t_vtable_t_init((gs_core_asset_t_vtable_t*)vt);
	vt->cls_id = gs_core_asset_script_t_class_id;
	vt->cls = gs_core_asset_script_t_class;
	vt->init = gs_core_asset_script_t_init;
	vt->dtor = gs_core_asset_script_t_dtor;
}

GS_API_DECL gs_core_asset_script_t_vtable_t
gs_core_asset_script_t_vtable_t_ctor()
{
	gs_core_asset_script_t_vtable_t vt = gs_default_val();
	gs_core_asset_script_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_asset_script_t_vtable_t_new()
{
	gs_core_asset_script_t_vtable_t* vt = gs_malloc_init(gs_core_asset_script_t_vtable_t);
	gs_core_asset_script_t_vtable_t_init(vt);
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
gs_core_asset_audio_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_asset_audio_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_asset_audio_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_asset_audio_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_asset_audio_t*
gs_core_asset_audio_t_new()
{
	gs_core_asset_audio_t* obj = gs_malloc_init(gs_core_asset_audio_t);
	gs_core_asset_audio_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_asset_audio_t
gs_core_asset_audio_t_ctor()
{
	gs_core_asset_audio_t obj = gs_default_val();
	gs_core_asset_audio_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_asset_audio_t_init;
	vt->dtor = gs_core_asset_audio_t_dtor;
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

/* gs_core_component_renderable_t */

GS_API_DECL uint32_t
gs_core_component_renderable_t_class_id()
{
	return gs_core_component_renderable_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_component_renderable_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_renderable_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_renderable_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_component_renderable_t_cls_id);
}

GS_API_DECL void
gs_core_component_renderable_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_component_renderable_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_component_renderable_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_component_renderable_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_component_renderable_t*
gs_core_component_renderable_t_new()
{
	gs_core_component_renderable_t* obj = gs_malloc_init(gs_core_component_renderable_t);
	gs_core_component_renderable_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_component_renderable_t
gs_core_component_renderable_t_ctor()
{
	gs_core_component_renderable_t obj = gs_default_val();
	gs_core_component_renderable_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_component_renderable_t_dtor(gs_core_obj_t* obj)
{
	gs_core_component_renderable_t* this = (gs_core_component_renderable_t*)obj;
	gs_core_graphics_scene_renderable_destroy(&gs_core_instance()->gfx->scene, this->hndl);
    
}

GS_API_DECL void
gs_core_component_renderable_t_vtable_t_init(gs_core_component_renderable_t_vtable_t* vt)
{
	gs_core_entities_component_t_vtable_t_init((gs_core_entities_component_t_vtable_t*)vt);
	vt->cls_id = gs_core_component_renderable_t_class_id;
	vt->cls = gs_core_component_renderable_t_class;
	vt->init = gs_core_component_renderable_t_init;
	vt->dtor = gs_core_component_renderable_t_dtor;
}

GS_API_DECL gs_core_component_renderable_t_vtable_t
gs_core_component_renderable_t_vtable_t_ctor()
{
	gs_core_component_renderable_t_vtable_t vt = gs_default_val();
	gs_core_component_renderable_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_component_renderable_t_vtable_t_new()
{
	gs_core_component_renderable_t_vtable_t* vt = gs_malloc_init(gs_core_component_renderable_t_vtable_t);
	gs_core_component_renderable_t_vtable_t_init(vt);
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
gs_core_network_rpc_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_network_rpc_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_network_rpc_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_network_rpc_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_network_rpc_t*
gs_core_network_rpc_t_new()
{
	gs_core_network_rpc_t* obj = gs_malloc_init(gs_core_network_rpc_t);
	gs_core_network_rpc_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_network_rpc_t
gs_core_network_rpc_t_ctor()
{
	gs_core_network_rpc_t obj = gs_default_val();
	gs_core_network_rpc_t_init((gs_core_obj_t*)&obj);
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
	vt->callback = gs_core_network_rpc_cb_default;
	vt->cls_id = gs_core_network_rpc_t_class_id;
	vt->cls = gs_core_network_rpc_t_class;
	vt->init = gs_core_network_rpc_t_init;
	vt->dtor = gs_core_network_rpc_t_dtor;
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

/* gs_core_network_rpc_unreliable_t */

GS_API_DECL uint32_t
gs_core_network_rpc_unreliable_t_class_id()
{
	return gs_core_network_rpc_unreliable_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_unreliable_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_unreliable_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_unreliable_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_unreliable_t_cls_id);
}

GS_API_DECL void
gs_core_network_rpc_unreliable_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_network_rpc_unreliable_t_class_id();
	gs_core_network_rpc_t* rpc = gs_core_cast(obj, gs_core_network_rpc_t);
	rpc->delivery = GS_CORE_NETWORK_DELIVERY_UNRELIABLE;
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_network_rpc_unreliable_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_network_rpc_unreliable_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_network_rpc_unreliable_t*
gs_core_network_rpc_unreliable_t_new()
{
	gs_core_network_rpc_unreliable_t* obj = gs_malloc_init(gs_core_network_rpc_unreliable_t);
	gs_core_network_rpc_unreliable_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_network_rpc_unreliable_t
gs_core_network_rpc_unreliable_t_ctor()
{
	gs_core_network_rpc_unreliable_t obj = gs_default_val();
	gs_core_network_rpc_unreliable_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_network_rpc_unreliable_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_network_rpc_unreliable_t_vtable_t_init(gs_core_network_rpc_unreliable_t_vtable_t* vt)
{
	gs_core_network_rpc_t_vtable_t_init((gs_core_network_rpc_t_vtable_t*)vt);
	vt->cls_id = gs_core_network_rpc_unreliable_t_class_id;
	vt->cls = gs_core_network_rpc_unreliable_t_class;
	vt->init = gs_core_network_rpc_unreliable_t_init;
	vt->dtor = gs_core_network_rpc_unreliable_t_dtor;
}

GS_API_DECL gs_core_network_rpc_unreliable_t_vtable_t
gs_core_network_rpc_unreliable_t_vtable_t_ctor()
{
	gs_core_network_rpc_unreliable_t_vtable_t vt = gs_default_val();
	gs_core_network_rpc_unreliable_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_network_rpc_unreliable_t_vtable_t_new()
{
	gs_core_network_rpc_unreliable_t_vtable_t* vt = gs_malloc_init(gs_core_network_rpc_unreliable_t_vtable_t);
	gs_core_network_rpc_unreliable_t_vtable_t_init(vt);
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
gs_core_entities_system_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_entities_system_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_entities_system_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_entities_system_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_entities_system_t*
gs_core_entities_system_t_new()
{
	gs_core_entities_system_t* obj = gs_malloc_init(gs_core_entities_system_t);
	gs_core_entities_system_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_entities_system_t
gs_core_entities_system_t_ctor()
{
	gs_core_entities_system_t obj = gs_default_val();
	gs_core_entities_system_t_init((gs_core_obj_t*)&obj);
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
	vt->callback = NULL;
	vt->cls_id = gs_core_entities_system_t_class_id;
	vt->cls = gs_core_entities_system_t_class;
	vt->init = gs_core_entities_system_t_init;
	vt->dtor = gs_core_entities_system_t_dtor;
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
gs_core_graphics_renderpass_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_graphics_renderpass_t_class_id();
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_graphics_renderpass_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_graphics_renderpass_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_graphics_renderpass_t*
gs_core_graphics_renderpass_t_new()
{
	gs_core_graphics_renderpass_t* obj = gs_malloc_init(gs_core_graphics_renderpass_t);
	gs_core_graphics_renderpass_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_graphics_renderpass_t
gs_core_graphics_renderpass_t_ctor()
{
	gs_core_graphics_renderpass_t obj = gs_default_val();
	gs_core_graphics_renderpass_t_init((gs_core_obj_t*)&obj);
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
	vt->init = gs_core_graphics_renderpass_t_init;
	vt->dtor = gs_core_graphics_renderpass_t_dtor;
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

/* gs_core_network_rpc_reliable_t */

GS_API_DECL uint32_t
gs_core_network_rpc_reliable_t_class_id()
{
	return gs_core_network_rpc_reliable_t_cls_id;
}

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_reliable_t_class()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_reliable_t_cls_id)->cls;
}

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_reliable_t_info()
{
	return gs_core_meta_obj_info_w_cls_id(gs_core_network_rpc_reliable_t_cls_id);
}

GS_API_DECL void
gs_core_network_rpc_reliable_t_init(gs_core_obj_t* obj)
{
	gs_core_cast(obj, gs_core_base_t)->id = gs_core_network_rpc_reliable_t_class_id();
	gs_core_network_rpc_t* rpc = gs_core_cast(obj, gs_core_network_rpc_t);
	rpc->delivery = GS_CORE_NETWORK_DELIVERY_RELIABLE;
	if (gs_core_instance() && gs_core_instance()->meta)
	{
		gs_core_network_rpc_reliable_t_vtable_t* vt = gs_core_cast_vt(obj, gs_core_network_rpc_reliable_t);
		if (vt && vt->post_init) vt->post_init(gs_core_cast(obj, gs_core_obj_t));
	}
}

GS_API_DECL gs_core_network_rpc_reliable_t*
gs_core_network_rpc_reliable_t_new()
{
	gs_core_network_rpc_reliable_t* obj = gs_malloc_init(gs_core_network_rpc_reliable_t);
	gs_core_network_rpc_reliable_t_init(gs_core_cast(obj, gs_core_obj_t));
	return obj;
}

GS_API_DECL gs_core_network_rpc_reliable_t
gs_core_network_rpc_reliable_t_ctor()
{
	gs_core_network_rpc_reliable_t obj = gs_default_val();
	gs_core_network_rpc_reliable_t_init((gs_core_obj_t*)&obj);
	return obj;
}

GS_API_DECL void
gs_core_network_rpc_reliable_t_dtor(gs_core_obj_t* obj)
{
}

GS_API_DECL void
gs_core_network_rpc_reliable_t_vtable_t_init(gs_core_network_rpc_reliable_t_vtable_t* vt)
{
	gs_core_network_rpc_t_vtable_t_init((gs_core_network_rpc_t_vtable_t*)vt);
	vt->cls_id = gs_core_network_rpc_reliable_t_class_id;
	vt->cls = gs_core_network_rpc_reliable_t_class;
	vt->init = gs_core_network_rpc_reliable_t_init;
	vt->dtor = gs_core_network_rpc_reliable_t_dtor;
}

GS_API_DECL gs_core_network_rpc_reliable_t_vtable_t
gs_core_network_rpc_reliable_t_vtable_t_ctor()
{
	gs_core_network_rpc_reliable_t_vtable_t vt = gs_default_val();
	gs_core_network_rpc_reliable_t_vtable_t_init(&vt);
	return vt;
}

GS_API_DECL gs_core_vtable_t*
gs_core_network_rpc_reliable_t_vtable_t_new()
{
	gs_core_network_rpc_reliable_t_vtable_t* vt = gs_malloc_init(gs_core_network_rpc_reliable_t_vtable_t);
	gs_core_network_rpc_reliable_t_vtable_t_init(vt);
	return (gs_core_vtable_t*)vt;
}

//============[ Registry ]===============//

//Static Objects
static gs_core_asset_handle_t g_1 = {0};
static gs_core_app_t g_2 = {0};
static gs_core_asset_t g_3 = {0};
static gs_core_asset_texture_t g_4 = {0};
static gs_core_asset_pipeline_t g_5 = {0};
static gs_core_asset_mesh_t g_6 = {0};
static gs_core_asset_font_t g_7 = {0};
static gs_core_obj_t g_8 = {0};
static gs_core_asset_ui_stylesheet_t g_9 = {0};
static gs_core_entities_component_t g_10 = {0};
static gs_core_asset_material_t g_11 = {0};
static gs_core_component_tag_t g_12 = {0};
static gs_core_system_renderable_t g_13 = {0};
static gs_core_component_transform_t g_14 = {0};
static gs_core_asset_script_t g_15 = {0};
static gs_core_asset_audio_t g_16 = {0};
static gs_core_component_renderable_t g_17 = {0};
static gs_core_network_rpc_t g_18 = {0};
static gs_core_network_rpc_unreliable_t g_19 = {0};
static gs_core_entities_system_t g_20 = {0};
static gs_core_graphics_renderpass_t g_21 = {0};
static gs_core_network_rpc_reliable_t g_22 = {0};

GS_API_DECL void
gs_core_meta_register()
{
	// CVars
	gs_core_meta_registry_t* meta = gs_core_instance()->meta;
	gs_meta_class_t* cls = NULL;
	gs_meta_class_t* base = NULL;
	gs_core_meta_info_t* info = NULL;
	gs_core_network_rpc_t* rpc = NULL;
	uint64_t cid = 0;
	uint32_t cnt = 0;
	uint32_t idx = 0;

	/* gs_core_asset_handle_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_handle_t),
			.properties = (gs_meta_property_t[]) {
				gs_meta_property(gs_core_asset_handle_t, uint32_t, hndl, GS_META_PROPERTY_TYPE_INFO_U32),
				gs_meta_property(gs_core_asset_handle_t, uint32_t, importer, GS_META_PROPERTY_TYPE_INFO_U32)
			},
			.size = 2 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_handle_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_1, gs_core_base_t)->id = gs_core_asset_handle_t_class_id();
		info->instance = gs_core_cast(&g_1, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_handle_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_asset_handle_t_vtable_t_new();
		gs_core_asset_handle_t_vtable_t_init(info->vtable);
	}

	/* gs_core_app_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_app_t),
			.properties = (gs_meta_property_t[]) {
				gs_meta_property(gs_core_app_t, float, run_time, GS_META_PROPERTY_TYPE_INFO_F32),
				gs_meta_property(gs_core_app_t, gs_vec4, viewport, GS_META_PROPERTY_TYPE_INFO_VEC4)
			},
			.size = 2 * sizeof(gs_meta_property_t)
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
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
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
		gs_core_cast(&g_4, gs_core_base_t)->id = gs_core_asset_texture_t_class_id();
		info->instance = gs_core_cast(&g_4, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_texture_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_texture_t_vtable_t_new();
		gs_core_asset_texture_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_5, gs_core_base_t)->id = gs_core_asset_pipeline_t_class_id();
		info->instance = gs_core_cast(&g_5, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_pipeline_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_pipeline_t_vtable_t_new();
		gs_core_asset_pipeline_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_7, gs_core_base_t)->id = gs_core_asset_font_t_class_id();
		info->instance = gs_core_cast(&g_7, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_font_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_font_t_vtable_t_new();
		gs_core_asset_font_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_8, gs_core_base_t)->id = gs_core_obj_t_class_id();
		info->instance = gs_core_cast(&g_8, gs_core_obj_t);
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
		gs_core_cast(&g_9, gs_core_base_t)->id = gs_core_asset_ui_stylesheet_t_class_id();
		info->instance = gs_core_cast(&g_9, gs_core_obj_t);
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
		gs_core_cast(&g_10, gs_core_base_t)->id = gs_core_entities_component_t_class_id();
		info->instance = gs_core_cast(&g_10, gs_core_obj_t);
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
		gs_core_cast(&g_11, gs_core_base_t)->id = gs_core_asset_material_t_class_id();
		info->instance = gs_core_cast(&g_11, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_material_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_material_t_vtable_t_new();
		gs_core_asset_material_t_vtable_t_init(info->vtable);
	}

	/* gs_core_component_tag_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_component_tag_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_component_tag_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_12, gs_core_base_t)->id = gs_core_component_tag_t_class_id();
		info->instance = gs_core_cast(&g_12, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_component_tag_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_entities_component_t);
		info->base = gs_core_cls_info(gs_core_entities_component_t);
		info->vtable = gs_core_component_tag_t_vtable_t_new();
		gs_core_component_tag_t_vtable_t_init(info->vtable);
	}

	/* gs_core_system_renderable_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_system_renderable_t),
			.properties = (gs_meta_property_t[]) {
				gs_meta_property(gs_core_system_renderable_t, gs_core_component_renderable_t*, renderable, GS_META_PROPERTY_TYPE_INFO_OBJ),
				gs_meta_property(gs_core_system_renderable_t, gs_core_component_transform_t*, transform, GS_META_PROPERTY_TYPE_INFO_OBJ)
			},
			.size = 2 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_system_renderable_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_13, gs_core_base_t)->id = gs_core_system_renderable_t_class_id();
		info->instance = gs_core_cast(&g_13, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_system_renderable_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_entities_system_t);
		info->base = gs_core_cls_info(gs_core_entities_system_t);
		info->vtable = gs_core_system_renderable_t_vtable_t_new();
		gs_core_system_renderable_t_vtable_t_init(info->vtable);
	}

	/* gs_core_component_transform_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_component_transform_t),
			.properties = (gs_meta_property_t[]) {
				gs_meta_property(gs_core_component_transform_t, gs_vqs, transform, GS_META_PROPERTY_TYPE_INFO_VQS)
			},
			.size = 1 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_component_transform_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_14, gs_core_base_t)->id = gs_core_component_transform_t_class_id();
		info->instance = gs_core_cast(&g_14, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_component_transform_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_entities_component_t);
		info->base = gs_core_cls_info(gs_core_entities_component_t);
		info->vtable = gs_core_component_transform_t_vtable_t_new();
		gs_core_component_transform_t_vtable_t_init(info->vtable);
	}

	/* gs_core_asset_script_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_asset_script_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_asset_script_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_15, gs_core_base_t)->id = gs_core_asset_script_t_class_id();
		info->instance = gs_core_cast(&g_15, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_script_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_script_t_vtable_t_new();
		gs_core_asset_script_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_16, gs_core_base_t)->id = gs_core_asset_audio_t_class_id();
		info->instance = gs_core_cast(&g_16, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_asset_audio_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_asset_t);
		info->base = gs_core_cls_info(gs_core_asset_t);
		info->vtable = gs_core_asset_audio_t_vtable_t_new();
		gs_core_asset_audio_t_vtable_t_init(info->vtable);
	}

	/* gs_core_component_renderable_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_component_renderable_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_component_renderable_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_17, gs_core_base_t)->id = gs_core_component_renderable_t_class_id();
		info->instance = gs_core_cast(&g_17, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_component_renderable_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_entities_component_t);
		info->base = gs_core_cls_info(gs_core_entities_component_t);
		info->vtable = gs_core_component_renderable_t_vtable_t_new();
		gs_core_component_renderable_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_18, gs_core_base_t)->id = gs_core_network_rpc_t_class_id();
		info->instance = gs_core_cast(&g_18, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_network_rpc_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_network_rpc_t_vtable_t_new();
		gs_core_network_rpc_t_vtable_t_init(info->vtable);
	}

	/* gs_core_network_rpc_unreliable_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_network_rpc_unreliable_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_network_rpc_unreliable_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_19, gs_core_base_t)->id = gs_core_network_rpc_unreliable_t_class_id();
		rpc = gs_core_cast(&g_19, gs_core_network_rpc_t);
		rpc->delivery = GS_CORE_NETWORK_DELIVERY_UNRELIABLE;
		info->instance = gs_core_cast(&g_19, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_network_rpc_unreliable_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_network_rpc_t);
		info->base = gs_core_cls_info(gs_core_network_rpc_t);
		info->vtable = gs_core_network_rpc_unreliable_t_vtable_t_new();
		gs_core_network_rpc_unreliable_t_vtable_t_init(info->vtable);
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
		gs_core_cast(&g_20, gs_core_base_t)->id = gs_core_entities_system_t_class_id();
		info->instance = gs_core_cast(&g_20, gs_core_obj_t);
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
		gs_core_cast(&g_21, gs_core_base_t)->id = gs_core_graphics_renderpass_t_class_id();
		info->instance = gs_core_cast(&g_21, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_graphics_renderpass_t);
		info->cid = hndl;
		info->base = NULL;
		info->vtable = gs_core_graphics_renderpass_t_vtable_t_new();
		gs_core_graphics_renderpass_t_vtable_t_init(info->vtable);
	}

	/* gs_core_network_rpc_reliable_t */
	{
		cid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){
			.name = gs_to_str(gs_core_network_rpc_reliable_t),
			.properties = (gs_meta_property_t[]){0},
			.size = 0 * sizeof(gs_meta_property_t)
		}));

		gs_core_meta_info_t ci = {0};
		uint32_t hndl = gs_core_network_rpc_reliable_t_class_id();
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);
		gs_core_cast(&g_22, gs_core_base_t)->id = gs_core_network_rpc_reliable_t_class_id();
		rpc = gs_core_cast(&g_22, gs_core_network_rpc_t);
		rpc->delivery = GS_CORE_NETWORK_DELIVERY_RELIABLE;
		info->instance = gs_core_cast(&g_22, gs_core_obj_t);
		info->cls = gs_meta_class_get(&meta->registry, gs_core_network_rpc_reliable_t);
		info->cid = hndl;
		uint32_t bid = gs_core_cls_cid(gs_core_network_rpc_t);
		info->base = gs_core_cls_info(gs_core_network_rpc_t);
		info->vtable = gs_core_network_rpc_reliable_t_vtable_t_new();
		gs_core_network_rpc_reliable_t_vtable_t_init(info->vtable);
	}

}

GS_API_DECL void
gs_core_meta_unregister()
{
	gs_core_entities_t* ents = gs_core_instance()->entities;

	// CVars

	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_handle_t_cls_id);
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
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_component_tag_t_cls_id);
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
		ents->component_unregister(gs_core_entities_component_id(gs_core_component_tag_t));
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_system_renderable_t_cls_id);
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
		ents->system_unregister(gs_core_entities_system_id(gs_core_system_renderable_t));
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_component_transform_t_cls_id);
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
		ents->component_unregister(gs_core_entities_component_id(gs_core_component_transform_t));
	}
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_asset_script_t_cls_id);
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
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_component_renderable_t_cls_id);
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
		ents->component_unregister(gs_core_entities_component_id(gs_core_component_renderable_t));
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
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_network_rpc_unreliable_t_cls_id);
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
	{
		gs_core_meta_registry_t* meta = gs_core_instance()->meta;
		gs_assert(meta);
		gs_core_meta_info_t* info = gs_slot_array_getp(meta->info, gs_core_network_rpc_reliable_t_cls_id);
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

	{
		gs_core_entity_t comp = ents->component_register(&(gs_core_entities_component_desc_t){
			.name = gs_to_str(gs_core_component_tag_t),
			.size = sizeof(gs_core_component_tag_t),
			.alignment = ECS_ALIGNOF(gs_core_component_tag_t)
		});
		gs_hash_table_insert(ents->components, gs_core_component_tag_t_cls_id, comp);
	}
	{
		gs_core_entity_t comp = ents->component_register(&(gs_core_entities_component_desc_t){
			.name = gs_to_str(gs_core_component_transform_t),
			.size = sizeof(gs_core_component_transform_t),
			.alignment = ECS_ALIGNOF(gs_core_component_transform_t)
		});
		gs_hash_table_insert(ents->components, gs_core_component_transform_t_cls_id, comp);
	}
	{
		gs_core_entity_t comp = ents->component_register(&(gs_core_entities_component_desc_t){
			.name = gs_to_str(gs_core_component_renderable_t),
			.size = sizeof(gs_core_component_renderable_t),
			.alignment = ECS_ALIGNOF(gs_core_component_renderable_t)
		});
		gs_hash_table_insert(ents->components, gs_core_component_renderable_t_cls_id, comp);
	}

	// Systems

	/* gs_core_system_renderable_t */
	{
		gs_core_entity_t system = ents->system_register(&(gs_core_entities_system_desc_t){
			.name = gs_to_str(gs_core_system_renderable_t),
			.callback = _gs_core_system_renderable_t_cb,
			.filter.component_list = {
				gs_hash_table_get(ents->components, gs_core_cls_cid(gs_core_component_renderable_t)),
				gs_hash_table_get(ents->components, gs_core_cls_cid(gs_core_component_transform_t))
			}
		});
		gs_hash_table_insert(ents->systems, gs_core_system_renderable_t_cls_id, system);
	}
}

//======[ Systems ]======//

GS_API_DECL void
_gs_core_system_renderable_t_cb(gs_core_entity_t* iter)
{
	gs_core_app_t* app = gs_core_app_instance();
	gs_core_system_renderable_t sdata = {._base.iter = iter};
	gs_core_cls_init(gs_core_system_renderable_t, &sdata);
	gs_core_cast(&sdata, gs_core_entities_system_t)->tick |= GS_CORE_ENTITIES_TICK_ON_PLAY;
	sdata.renderable = gs_core_entities_term((gs_core_entity_iter_t*)iter, gs_core_component_renderable_t, 0);
	sdata.transform = gs_core_entities_term((gs_core_entity_iter_t*)iter, gs_core_component_transform_t, 1);
	gs_core_entities_system_tick_flags tick = gs_core_cast(&sdata, gs_core_entities_system_t)->tick;
	bool can_tick = (tick & GS_CORE_ENTITIES_TICK_ALWAYS) ||
		(tick & GS_CORE_ENTITIES_TICK_ON_PLAY && app->state == GS_CORE_APP_STATE_PLAYING) ||
		(tick & GS_CORE_ENTITIES_TICK_ON_PAUSE && app->state == GS_CORE_APP_STATE_PAUSED) ||
		(tick & GS_CORE_ENTITIES_TICK_ON_STOP && app->state == GS_CORE_APP_STATE_STOPPED);
	if (can_tick && gs_core_cast_vt(&sdata, gs_core_system_renderable_t)->callback)
	{
		gs_core_cast_vt(&sdata, gs_core_system_renderable_t)->callback(&sdata);
	}
}
