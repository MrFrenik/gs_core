/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core_asset.h

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

#ifndef CORE_ASSET_H
#define CORE_ASSET_H 

#include "core_gs.h"
#include "core_object.h"

#define CORE_ASSET_IMPORTER_FILE_EXTENSIONS_MAX   10
#define CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH     32 
#define CORE_ASSET_STR_MAX                        1024

typedef enum
{
    CORE_ASSET_STATE_UNLOADED = 0x00,
    CORE_ASSET_STATE_LOADED
} core_asset_state_t; 

typedef struct 
{ 
    uint32_t hndl;      // Slot array handle to raw asset in importer
    uint32_t importer;  // Slot array handle to raw importer data in asset manager
} core_asset_handle_t;

_introspect()
typedef struct
{ 
    core_base(core_obj_t);

    _field()
    uint32_t record_hndl;

    core_asset_state_t state;

} core_asset_t;

GS_API_DECL core_asset_t* 
core_asset_handle_get(const core_asset_handle_t* hndl);

//====[ Texture ]====//

_introspect()
typedef struct
{ 
    core_base(core_asset_t);

    gs_gfxt_texture_t texture;

} core_texture_t; 

//====[ Mesh ]====//

_introspect()
typedef struct
{
    core_base(core_asset_t);

    gs_gfxt_mesh_t mesh;

} core_mesh_t;

//====[ Material ]====//

_introspect()
typedef struct
{ 
    core_base(core_asset_t);

    gs_gfxt_material_t material; 

} core_material_t; 

//====[ Pipeline ]====//

_introspect()
typedef struct
{ 
    core_base(core_asset_t);

    gs_gfxt_pipeline_t pipeline; 

} core_pipeline_t;

//====[ Font ]====//

_introspect()
typedef struct
{ 
    core_base(core_asset_t);

    gs_asset_font_t font;

} core_font_t; 

//====[ Audio ]====//

_introspect()
typedef struct 
{ 
    core_base(core_asset_t);

    gs_asset_audio_t audio;

} core_audio_t; 

//====[ UIStyleSheet ]====//

_introspect()
typedef struct
{ 
    core_base(core_asset_t);

    gs_gui_style_sheet_t style_sheet;

} core_ui_stylesheet_t;

//====[ Asset Record ]====//

typedef struct
{ 
    uint32_t hndl;                  // Handle to asset slot array in storage
    char path[CORE_ASSET_STR_MAX];  // Absolute path to asset on disk
    gs_uuid_t uuid;                 // UUID for asset
    char name[CORE_ASSET_STR_MAX];  // Qualified name for asset
} core_asset_record_t; 

typedef struct
{
    union
    {
        struct 
        {
            gs_graphics_texture_desc_t desc;
        } texture;

        struct
        {
            uint32_t point_size;
        } font;

        struct 
        { 
            gs_gfxt_mesh_import_options_t desc;
        } mesh; 

        struct
        {
            gs_gui_context_t* ctx;
        } gui;
    };
} core_asset_import_options_t;

typedef struct
{
    core_asset_t* (* load_resource_from_file)(const char* path, core_asset_import_options_t* options);
    void (* register_defaults)();
    const char* resource_file_extensions[5];
    const char* asset_file_extension;
    uint64_t class_id;
} core_asset_importer_desc_t; 

typedef struct
{
    core_asset_t* (* load_resource_from_file)(const char* path, void* import_options);
    void (* register_defaults)();
    gs_slot_array(core_asset_record_t) records;                       // Loaded asset records
    gs_slot_array(core_asset_t*) assets;                             // Slot array of raw asset data 
    gs_hash_table(uint64_t, uint32_t) uuid2id;                      // Mapping from uuid to record slot id
    gs_hash_table(uint64_t, uint32_t) name2id;                      // Mapping from qualified name to record slot id
    char file_extension[CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH];     // File extension for asset
    uint64_t class_id;                                              // Class ID
} core_asset_importer_t; 

//=====[ Texture Importer ]=====//

GS_API_DECL core_asset_t* 
core_texture_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_texture_importer_register_defaults();

//=====[ Mesh Importer ]=====//

GS_API_DECL core_asset_t* 
core_mesh_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_mesh_importer_register_defaults(); 

//=====[ Pipeline Importer ]=====//

GS_API_DECL core_asset_t* 
core_pipeline_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_pipeline_importer_register_defaults();

//=====[ Font Importer ]=====//

GS_API_DECL core_asset_t* 
core_font_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_font_importer_register_defaults(); 

//=====[ Audio Importer ]=====//

GS_API_DECL core_asset_t* 
core_audio_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_audio_importer_register_defaults();

//=====[ UIStyleSheet Importer ]=====//

GS_API_DECL core_asset_t* 
core_ui_stylesheet_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options);

GS_API_DECL void 
core_ui_stylesheet_importer_register_defaults();

//=====[ Assets ]=====//

typedef struct core_assets_s
{ 
    char root_path[CORE_ASSET_STR_MAX];
    gs_slot_array(core_asset_importer_t) importers;
    gs_hash_table(uint64_t, uint32_t) cid2importer; // Mapping from class id to importer data
    gs_hash_table(uint64_t, uint32_t) fe2importer;  // Mapping file extension to importer data 
} core_assets_t;

GS_API_DECL core_assets_t* 
core_assets_new(const char* root_path);

GS_API_DECL core_assets_t* 
core_assets_instance();

GS_API_DECL core_asset_handle_t 
core_assets_import(const char* path, core_asset_import_options_t* options, bool serialize); 

GS_API_DECL void 
core_assets_serialize_asset(const char* path, const core_asset_t* in);

GS_API_DECL void 
core_assets_deserialize_asset(const char* path, core_asset_t* out);

#endif // CORE_ASSET_H





















