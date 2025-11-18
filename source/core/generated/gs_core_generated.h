/*==============================================================================================================
	* Copyright: 2022 John Jackson
	* File: gs_core_generated.h
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

#ifndef gs_core_GENERATED_H
#define gs_core_GENERATED_H

// Includes
#include "core/gs_core_object.h"
#include "core/gs_core_gs.h"
#include "core/gs_core_object.h"
#include "..\..\source\core\/gs_core.h"
#include "..\..\source\core\/gs_core_ai.h"
#include "..\..\source\core\/gs_core_app.h"
#include "..\..\source\core\/gs_core_asset.h"
#include "..\..\source\core\/gs_core_components.h"
#include "..\..\source\core\/gs_core_crash.h"
#include "..\..\source\core\/gs_core_entity.h"
#include "..\..\source\core\/gs_core_graphics.h"
#include "..\..\source\core\/gs_core_gs.h"
#include "..\..\source\core\/gs_core_network.h"
#include "..\..\source\core\/gs_core_object.h"
#include "..\..\source\core\/gs_core_physics.h"
#include "..\..\source\core\/gs_core_util.h"
#include "..\..\source\core\/gs_core_vm.h"

//======[ Lambdas ]=======//


//======[ Components ]=======//

GS_API_DECL void
gs_core_meta_ecs_register();

gs_core_entities_component_declare(gs_core_component_transform_t);
gs_core_entities_component_declare(gs_core_component_renderable_t);
gs_core_entities_component_declare(gs_core_component_tag_t);

//======[ Systems ]=======//

gs_core_entities_system_declare(gs_core_system_renderable_t);

GS_API_DECL void
_gs_core_system_renderable_t_cb(gs_core_entity_iter_t* iter);

GS_API_DECL void
gs_core_system_renderable_t_cb(gs_core_entities_system_t* system);


//======[ Classes ]=======//

#define gs_core_entities_component_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods

#define gs_core_app_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods\
	void (* app_update)() ;\
	void (* app_init)() ;\
	void (* app_render)(gs_command_buffer_t* cb) ;\
	void (* app_shutdown)() ;\
	void (* app_editor)(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd) ;

#define gs_core_asset_handle_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods

#define gs_core_asset_texture_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods
\
\
\

#define gs_core_asset_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods\
	gs_core_asset_importer_t* (* importer)() ;

#define gs_core_asset_mesh_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_asset_material_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_asset_font_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_asset_pipeline_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_component_transform_t_vtable_t_methods\
	gs_core_entities_component_t_vtable_t_methods

#define gs_core_network_rpc_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods\
	void (* callback)(struct gs_core_network_rpc_t* rpc) ;

#define gs_core_network_rpc_reliable_t_vtable_t_methods\
	gs_core_network_rpc_t_vtable_t_methods

#define gs_core_entities_system_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods\
	void (* callback)(gs_core_entities_system_t* system) ;

#define gs_core_obj_t_vtable_t_methods\
	gs_core_vtable_t_methods\
	gs_result (* serialize)(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj) ;\
	gs_result (* net_serialize)(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj) ;\
	gs_result (* deserialize)(gs_byte_buffer_t* buffer, gs_core_obj_t* obj) ;\
	gs_result (* net_deserialize)(gs_byte_buffer_t* buffer, gs_core_obj_t* obj) ;

#define gs_core_asset_ui_stylesheet_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_component_renderable_t_vtable_t_methods\
	gs_core_entities_component_t_vtable_t_methods

#define gs_core_component_tag_t_vtable_t_methods\
	gs_core_entities_component_t_vtable_t_methods

#define gs_core_system_renderable_t_vtable_t_methods\
	gs_core_entities_system_t_vtable_t_methods
\

#define gs_core_asset_script_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_asset_audio_t_vtable_t_methods\
	gs_core_asset_t_vtable_t_methods

#define gs_core_network_rpc_unreliable_t_vtable_t_methods\
	gs_core_network_rpc_t_vtable_t_methods

#define gs_core_graphics_renderpass_t_vtable_t_methods\
	gs_core_obj_t_vtable_t_methods\
	void (* exec)(gs_command_buffer_t* cb, gs_core_graphics_renderpass_t* pass) ;

/* gs_core_entities_component_t */

GS_API_DECL uint32_t
gs_core_entities_component_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_entities_component_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_entities_component_t_info();

GS_API_DECL gs_core_entities_component_t*
gs_core_entities_component_t_new();

GS_API_DECL void
gs_core_entities_component_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_entities_component_t
_gs_core_entities_component_t_ctor();

GS_API_DECL void
gs_core_entities_component_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_entities_component_t_vtable_s
{
	gs_core_entities_component_t_vtable_t_methods
} gs_core_entities_component_t_vtable_t;

GS_API_DECL void
gs_core_entities_component_t_vtable_t_init(gs_core_entities_component_t_vtable_t* vt);

GS_API_DECL gs_core_entities_component_t_vtable_t
gs_core_entities_component_t_vtable_t_ctor();


/* gs_core_app_t */

GS_API_DECL uint32_t
gs_core_app_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_app_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_app_t_info();

GS_API_DECL gs_core_app_t*
gs_core_app_t_new();

GS_API_DECL void
gs_core_app_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_app_t
_gs_core_app_t_ctor();

GS_API_DECL void
gs_core_app_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_app_t_vtable_s
{
	gs_core_app_t_vtable_t_methods
} gs_core_app_t_vtable_t;

GS_API_DECL void
gs_core_app_t_vtable_t_init(gs_core_app_t_vtable_t* vt);

GS_API_DECL gs_core_app_t_vtable_t
gs_core_app_t_vtable_t_ctor();


/* gs_core_asset_handle_t */

GS_API_DECL uint32_t
gs_core_asset_handle_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_handle_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_handle_t_info();

GS_API_DECL gs_core_asset_handle_t*
gs_core_asset_handle_t_new();

GS_API_DECL void
gs_core_asset_handle_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_handle_t
_gs_core_asset_handle_t_ctor();

GS_API_DECL void
gs_core_asset_handle_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_handle_t_vtable_s
{
	gs_core_asset_handle_t_vtable_t_methods
} gs_core_asset_handle_t_vtable_t;

GS_API_DECL void
gs_core_asset_handle_t_vtable_t_init(gs_core_asset_handle_t_vtable_t* vt);

GS_API_DECL gs_core_asset_handle_t_vtable_t
gs_core_asset_handle_t_vtable_t_ctor();


/* gs_core_asset_texture_t */

GS_API_DECL uint32_t
gs_core_asset_texture_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_texture_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_texture_t_info();

GS_API_DECL gs_core_asset_texture_t*
gs_core_asset_texture_t_new();

GS_API_DECL void
gs_core_asset_texture_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_texture_t
_gs_core_asset_texture_t_ctor();

GS_API_DECL void
gs_core_asset_texture_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_texture_t_vtable_s
{
	gs_core_asset_texture_t_vtable_t_methods
} gs_core_asset_texture_t_vtable_t;

GS_API_DECL void
gs_core_asset_texture_t_vtable_t_init(gs_core_asset_texture_t_vtable_t* vt);

GS_API_DECL gs_core_asset_texture_t_vtable_t
gs_core_asset_texture_t_vtable_t_ctor();


/* gs_core_asset_t */

GS_API_DECL uint32_t
gs_core_asset_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_t_info();

GS_API_DECL gs_core_asset_t*
gs_core_asset_t_new();

GS_API_DECL void
gs_core_asset_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_t
_gs_core_asset_t_ctor();

GS_API_DECL void
gs_core_asset_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_t_vtable_s
{
	gs_core_asset_t_vtable_t_methods
} gs_core_asset_t_vtable_t;

GS_API_DECL void
gs_core_asset_t_vtable_t_init(gs_core_asset_t_vtable_t* vt);

GS_API_DECL gs_core_asset_t_vtable_t
gs_core_asset_t_vtable_t_ctor();


/* gs_core_asset_mesh_t */

GS_API_DECL uint32_t
gs_core_asset_mesh_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_mesh_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_mesh_t_info();

GS_API_DECL gs_core_asset_mesh_t*
gs_core_asset_mesh_t_new();

GS_API_DECL void
gs_core_asset_mesh_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_mesh_t
_gs_core_asset_mesh_t_ctor();

GS_API_DECL void
gs_core_asset_mesh_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_mesh_t_vtable_s
{
	gs_core_asset_mesh_t_vtable_t_methods
} gs_core_asset_mesh_t_vtable_t;

GS_API_DECL void
gs_core_asset_mesh_t_vtable_t_init(gs_core_asset_mesh_t_vtable_t* vt);

GS_API_DECL gs_core_asset_mesh_t_vtable_t
gs_core_asset_mesh_t_vtable_t_ctor();


/* gs_core_asset_material_t */

GS_API_DECL uint32_t
gs_core_asset_material_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_material_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_material_t_info();

GS_API_DECL gs_core_asset_material_t*
gs_core_asset_material_t_new();

GS_API_DECL void
gs_core_asset_material_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_material_t
_gs_core_asset_material_t_ctor();

GS_API_DECL void
gs_core_asset_material_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_material_t_vtable_s
{
	gs_core_asset_material_t_vtable_t_methods
} gs_core_asset_material_t_vtable_t;

GS_API_DECL void
gs_core_asset_material_t_vtable_t_init(gs_core_asset_material_t_vtable_t* vt);

GS_API_DECL gs_core_asset_material_t_vtable_t
gs_core_asset_material_t_vtable_t_ctor();


/* gs_core_asset_font_t */

GS_API_DECL uint32_t
gs_core_asset_font_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_font_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_font_t_info();

GS_API_DECL gs_core_asset_font_t*
gs_core_asset_font_t_new();

GS_API_DECL void
gs_core_asset_font_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_font_t
_gs_core_asset_font_t_ctor();

GS_API_DECL void
gs_core_asset_font_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_font_t_vtable_s
{
	gs_core_asset_font_t_vtable_t_methods
} gs_core_asset_font_t_vtable_t;

GS_API_DECL void
gs_core_asset_font_t_vtable_t_init(gs_core_asset_font_t_vtable_t* vt);

GS_API_DECL gs_core_asset_font_t_vtable_t
gs_core_asset_font_t_vtable_t_ctor();


/* gs_core_asset_pipeline_t */

GS_API_DECL uint32_t
gs_core_asset_pipeline_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_pipeline_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_pipeline_t_info();

GS_API_DECL gs_core_asset_pipeline_t*
gs_core_asset_pipeline_t_new();

GS_API_DECL void
gs_core_asset_pipeline_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_pipeline_t
_gs_core_asset_pipeline_t_ctor();

GS_API_DECL void
gs_core_asset_pipeline_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_pipeline_t_vtable_s
{
	gs_core_asset_pipeline_t_vtable_t_methods
} gs_core_asset_pipeline_t_vtable_t;

GS_API_DECL void
gs_core_asset_pipeline_t_vtable_t_init(gs_core_asset_pipeline_t_vtable_t* vt);

GS_API_DECL gs_core_asset_pipeline_t_vtable_t
gs_core_asset_pipeline_t_vtable_t_ctor();


/* gs_core_component_transform_t */

GS_API_DECL uint32_t
gs_core_component_transform_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_component_transform_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_transform_t_info();

GS_API_DECL gs_core_component_transform_t*
gs_core_component_transform_t_new();

GS_API_DECL void
gs_core_component_transform_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_component_transform_t
_gs_core_component_transform_t_ctor();

GS_API_DECL void
gs_core_component_transform_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_component_transform_t_vtable_s
{
	gs_core_component_transform_t_vtable_t_methods
} gs_core_component_transform_t_vtable_t;

GS_API_DECL void
gs_core_component_transform_t_vtable_t_init(gs_core_component_transform_t_vtable_t* vt);

GS_API_DECL gs_core_component_transform_t_vtable_t
gs_core_component_transform_t_vtable_t_ctor();


/* gs_core_network_rpc_t */

GS_API_DECL uint32_t
gs_core_network_rpc_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_t_info();

GS_API_DECL gs_core_network_rpc_t*
gs_core_network_rpc_t_new();

GS_API_DECL void
gs_core_network_rpc_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_network_rpc_t
_gs_core_network_rpc_t_ctor();

GS_API_DECL void
gs_core_network_rpc_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_network_rpc_t_vtable_s
{
	gs_core_network_rpc_t_vtable_t_methods
} gs_core_network_rpc_t_vtable_t;

GS_API_DECL void
gs_core_network_rpc_t_vtable_t_init(gs_core_network_rpc_t_vtable_t* vt);

GS_API_DECL gs_core_network_rpc_t_vtable_t
gs_core_network_rpc_t_vtable_t_ctor();


/* gs_core_network_rpc_reliable_t */

GS_API_DECL uint32_t
gs_core_network_rpc_reliable_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_reliable_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_reliable_t_info();

GS_API_DECL gs_core_network_rpc_reliable_t*
gs_core_network_rpc_reliable_t_new();

GS_API_DECL void
gs_core_network_rpc_reliable_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_network_rpc_reliable_t
_gs_core_network_rpc_reliable_t_ctor();

GS_API_DECL void
gs_core_network_rpc_reliable_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_network_rpc_reliable_t_vtable_s
{
	gs_core_network_rpc_reliable_t_vtable_t_methods
} gs_core_network_rpc_reliable_t_vtable_t;

GS_API_DECL void
gs_core_network_rpc_reliable_t_vtable_t_init(gs_core_network_rpc_reliable_t_vtable_t* vt);

GS_API_DECL gs_core_network_rpc_reliable_t_vtable_t
gs_core_network_rpc_reliable_t_vtable_t_ctor();


/* gs_core_entities_system_t */

GS_API_DECL uint32_t
gs_core_entities_system_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_entities_system_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_entities_system_t_info();

GS_API_DECL gs_core_entities_system_t*
gs_core_entities_system_t_new();

GS_API_DECL void
gs_core_entities_system_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_entities_system_t
_gs_core_entities_system_t_ctor();

GS_API_DECL void
gs_core_entities_system_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_entities_system_t_vtable_s
{
	gs_core_entities_system_t_vtable_t_methods
} gs_core_entities_system_t_vtable_t;

GS_API_DECL void
gs_core_entities_system_t_vtable_t_init(gs_core_entities_system_t_vtable_t* vt);

GS_API_DECL gs_core_entities_system_t_vtable_t
gs_core_entities_system_t_vtable_t_ctor();


/* gs_core_obj_t */

GS_API_DECL uint32_t
gs_core_obj_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_obj_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_obj_t_info();

GS_API_DECL gs_core_obj_t*
gs_core_obj_t_new();

GS_API_DECL void
gs_core_obj_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_obj_t
_gs_core_obj_t_ctor();

GS_API_DECL void
gs_core_obj_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_obj_t_vtable_s
{
	gs_core_obj_t_vtable_t_methods
} gs_core_obj_t_vtable_t;

GS_API_DECL void
gs_core_obj_t_vtable_t_init(gs_core_obj_t_vtable_t* vt);

GS_API_DECL gs_core_obj_t_vtable_t
gs_core_obj_t_vtable_t_ctor();


/* gs_core_asset_ui_stylesheet_t */

GS_API_DECL uint32_t
gs_core_asset_ui_stylesheet_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_ui_stylesheet_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_ui_stylesheet_t_info();

GS_API_DECL gs_core_asset_ui_stylesheet_t*
gs_core_asset_ui_stylesheet_t_new();

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_ui_stylesheet_t
_gs_core_asset_ui_stylesheet_t_ctor();

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_ui_stylesheet_t_vtable_s
{
	gs_core_asset_ui_stylesheet_t_vtable_t_methods
} gs_core_asset_ui_stylesheet_t_vtable_t;

GS_API_DECL void
gs_core_asset_ui_stylesheet_t_vtable_t_init(gs_core_asset_ui_stylesheet_t_vtable_t* vt);

GS_API_DECL gs_core_asset_ui_stylesheet_t_vtable_t
gs_core_asset_ui_stylesheet_t_vtable_t_ctor();


/* gs_core_component_renderable_t */

GS_API_DECL uint32_t
gs_core_component_renderable_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_component_renderable_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_renderable_t_info();

GS_API_DECL gs_core_component_renderable_t*
gs_core_component_renderable_t_new();

GS_API_DECL void
gs_core_component_renderable_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_component_renderable_t
_gs_core_component_renderable_t_ctor();

GS_API_DECL void
gs_core_component_renderable_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_component_renderable_t_vtable_s
{
	gs_core_component_renderable_t_vtable_t_methods
} gs_core_component_renderable_t_vtable_t;

GS_API_DECL void
gs_core_component_renderable_t_vtable_t_init(gs_core_component_renderable_t_vtable_t* vt);

GS_API_DECL gs_core_component_renderable_t_vtable_t
gs_core_component_renderable_t_vtable_t_ctor();


/* gs_core_component_tag_t */

GS_API_DECL uint32_t
gs_core_component_tag_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_component_tag_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_component_tag_t_info();

GS_API_DECL gs_core_component_tag_t*
gs_core_component_tag_t_new();

GS_API_DECL void
gs_core_component_tag_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_component_tag_t
_gs_core_component_tag_t_ctor();

GS_API_DECL void
gs_core_component_tag_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_component_tag_t_vtable_s
{
	gs_core_component_tag_t_vtable_t_methods
} gs_core_component_tag_t_vtable_t;

GS_API_DECL void
gs_core_component_tag_t_vtable_t_init(gs_core_component_tag_t_vtable_t* vt);

GS_API_DECL gs_core_component_tag_t_vtable_t
gs_core_component_tag_t_vtable_t_ctor();


/* gs_core_system_renderable_t */

GS_API_DECL uint32_t
gs_core_system_renderable_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_system_renderable_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_system_renderable_t_info();

GS_API_DECL gs_core_system_renderable_t*
gs_core_system_renderable_t_new();

GS_API_DECL void
gs_core_system_renderable_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_system_renderable_t
_gs_core_system_renderable_t_ctor();

GS_API_DECL void
gs_core_system_renderable_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_system_renderable_t_vtable_s
{
	gs_core_system_renderable_t_vtable_t_methods
} gs_core_system_renderable_t_vtable_t;

GS_API_DECL void
gs_core_system_renderable_t_vtable_t_init(gs_core_system_renderable_t_vtable_t* vt);

GS_API_DECL gs_core_system_renderable_t_vtable_t
gs_core_system_renderable_t_vtable_t_ctor();


/* gs_core_asset_script_t */

GS_API_DECL uint32_t
gs_core_asset_script_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_script_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_script_t_info();

GS_API_DECL gs_core_asset_script_t*
gs_core_asset_script_t_new();

GS_API_DECL void
gs_core_asset_script_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_script_t
_gs_core_asset_script_t_ctor();

GS_API_DECL void
gs_core_asset_script_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_script_t_vtable_s
{
	gs_core_asset_script_t_vtable_t_methods
} gs_core_asset_script_t_vtable_t;

GS_API_DECL void
gs_core_asset_script_t_vtable_t_init(gs_core_asset_script_t_vtable_t* vt);

GS_API_DECL gs_core_asset_script_t_vtable_t
gs_core_asset_script_t_vtable_t_ctor();


/* gs_core_asset_audio_t */

GS_API_DECL uint32_t
gs_core_asset_audio_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_asset_audio_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_asset_audio_t_info();

GS_API_DECL gs_core_asset_audio_t*
gs_core_asset_audio_t_new();

GS_API_DECL void
gs_core_asset_audio_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_asset_audio_t
_gs_core_asset_audio_t_ctor();

GS_API_DECL void
gs_core_asset_audio_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_asset_audio_t_vtable_s
{
	gs_core_asset_audio_t_vtable_t_methods
} gs_core_asset_audio_t_vtable_t;

GS_API_DECL void
gs_core_asset_audio_t_vtable_t_init(gs_core_asset_audio_t_vtable_t* vt);

GS_API_DECL gs_core_asset_audio_t_vtable_t
gs_core_asset_audio_t_vtable_t_ctor();


/* gs_core_network_rpc_unreliable_t */

GS_API_DECL uint32_t
gs_core_network_rpc_unreliable_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_network_rpc_unreliable_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_network_rpc_unreliable_t_info();

GS_API_DECL gs_core_network_rpc_unreliable_t*
gs_core_network_rpc_unreliable_t_new();

GS_API_DECL void
gs_core_network_rpc_unreliable_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_network_rpc_unreliable_t
_gs_core_network_rpc_unreliable_t_ctor();

GS_API_DECL void
gs_core_network_rpc_unreliable_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_network_rpc_unreliable_t_vtable_s
{
	gs_core_network_rpc_unreliable_t_vtable_t_methods
} gs_core_network_rpc_unreliable_t_vtable_t;

GS_API_DECL void
gs_core_network_rpc_unreliable_t_vtable_t_init(gs_core_network_rpc_unreliable_t_vtable_t* vt);

GS_API_DECL gs_core_network_rpc_unreliable_t_vtable_t
gs_core_network_rpc_unreliable_t_vtable_t_ctor();


/* gs_core_graphics_renderpass_t */

GS_API_DECL uint32_t
gs_core_graphics_renderpass_t_class_id();

GS_API_DECL const gs_meta_class_t*
gs_core_graphics_renderpass_t_class();

GS_API_DECL const gs_core_meta_info_t*
gs_core_graphics_renderpass_t_info();

GS_API_DECL gs_core_graphics_renderpass_t*
gs_core_graphics_renderpass_t_new();

GS_API_DECL void
gs_core_graphics_renderpass_t_init(gs_core_obj_t* obj);

GS_API_DECL gs_core_graphics_renderpass_t
_gs_core_graphics_renderpass_t_ctor();

GS_API_DECL void
gs_core_graphics_renderpass_t_dtor(gs_core_obj_t* obj);

typedef struct gs_core_graphics_renderpass_t_vtable_s
{
	gs_core_graphics_renderpass_t_vtable_t_methods
} gs_core_graphics_renderpass_t_vtable_t;

GS_API_DECL void
gs_core_graphics_renderpass_t_vtable_t_init(gs_core_graphics_renderpass_t_vtable_t* vt);

GS_API_DECL gs_core_graphics_renderpass_t_vtable_t
gs_core_graphics_renderpass_t_vtable_t_ctor();


//========================//

//============[ Registry ]===============//

GS_API_DECL void
gs_core_meta_register();

GS_API_DECL void
gs_core_meta_ecs_register();

GS_API_DECL void
gs_core_meta_unregister();

#endif // GENERATED_H

