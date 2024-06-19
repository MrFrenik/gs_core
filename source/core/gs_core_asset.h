/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_asset.h

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

#ifndef GS_CORE_ASSET_H
#define GS_CORE_ASSET_H 

#include "gs_core_gs.h"
#include "gs_core_object.h"

#define GS_CORE_ASSET_IMPORTER_FILE_EXTENSIONS_MAX   10
#define GS_CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH     32 
#define GS_CORE_ASSET_STR_MAX                        1024

// Forward Decls.
struct gs_core_asset_importer_s;
struct gs_core_asset_record_s;

typedef enum
{
    GS_CORE_ASSET_STATE_UNLOADED = 0x00,
    GS_CORE_ASSET_STATE_LOADED
} gs_core_asset_state_t; 

_introspect()
typedef struct 
{ 
    gs_core_base(gs_core_obj_t);

    _field()
    uint32_t hndl;      // Slot array handle to raw asset in importer

    _field()
    uint32_t importer;  // Slot array handle to raw importer data in asset manager

} gs_core_asset_handle_t;

enum {
    GS_CORE_ASSET_FLAG_IS_DEFAULT = (1 << 0)
}; 

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_obj_t);

    _vtable(
        gs_core_asset_importer_t* (* importer)() = NULL;
    )

    uint32_t record_hndl;
    gs_core_asset_state_t state;
    uint32_t flags;

} gs_core_asset_t;

GS_API_DECL gs_core_asset_t* 
gs_core_asset_handle_get(const gs_core_asset_handle_t* hndl); 

GS_API_DECL gs_core_asset_handle_t
gs_core_asset_handle_from_asset(const gs_core_asset_t* asset);

GS_API_DECL uint64_t 
gs_core_asset_handle_cls_id(const gs_core_asset_handle_t* hndl);

GS_API_DECL gs_meta_class_t* 
gs_core_asset_handle_cls(const gs_core_asset_handle_t* hndl);

GS_API_DECL struct gs_core_asset_record_s*
gs_core_asset_handle_record(const gs_core_asset_handle_t* hndl);

GS_API_DECL struct gs_core_asset_record_s*
gs_core_asset_handle_name(const gs_core_asset_handle_t* hndl);

GS_API_DECL struct gs_core_asset_importer_s*
gs_core_asset_handle_importer(const gs_core_asset_handle_t* hndl);

GS_API_DECL gs_core_asset_handle_t
gs_core_asset_handle_from_record(const struct gs_core_asset_record_s* record);

//====[ Texture ]====//

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_asset_t);

    _vtable(
        _override: importer = gs_core_asset_texture_importer;
        _override: serialize = gs_core_asset_texture_serialize;
        _override: deserialize = gs_core_asset_texture_deserialize;
    )

    gs_graphics_texture_desc_t desc;  // I don't like it, it's redundant, but I need it for now...
    gs_gfxt_texture_t resource;

} gs_core_asset_texture_t; 

GS_API_DECL struct gs_core_asset_importer_s*
gs_core_asset_texture_importer();

GS_API_DECL gs_result
gs_core_asset_texture_serialize(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj);

GS_API_DECL gs_result
gs_core_asset_texture_deserialize(gs_byte_buffer_t* buffer, gs_core_obj_t* obj);

//====[ Mesh ]====//

_introspect()
typedef struct
{
    gs_core_base(gs_core_asset_t);

    gs_gfxt_mesh_t resource;

} gs_core_asset_mesh_t;

//====[ Material ]====//

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_asset_t);

    gs_gfxt_material_t resource;

} gs_core_asset_material_t; 

//====[ Pipeline ]====//

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_asset_t);

    gs_gfxt_pipeline_t resource; 

} gs_core_asset_pipeline_t;

//====[ Font ]====//

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_asset_t);

    gs_asset_font_t resource;

} gs_core_asset_font_t; 

//====[ Audio ]====//

_introspect()
typedef struct 
{ 
    gs_core_base(gs_core_asset_t);

    gs_asset_audio_t resource;

} gs_core_asset_audio_t; 

//====[ UIStyleSheet ]====//

_introspect()
typedef struct
{ 
    gs_core_base(gs_core_asset_t);

    gs_gui_style_sheet_t resource;

} gs_core_asset_ui_stylesheet_t;

//====[ Asset Record ]====//

typedef struct gs_core_asset_record_s
{ 
    uint32_t hndl;                      // Handle to asset slot array in storage
    char path[GS_CORE_ASSET_STR_MAX];   // Absolute path to asset on disk
    gs_uuid_t uuid;                     // UUID for asset
    char name[GS_CORE_ASSET_STR_MAX];   // Qualified name for asset 
    uint32_t importer;                  // Slot array handle to raw importer data in asset manager
} gs_core_asset_record_t;

enum {
    GS_CORE_ASSET_IMPORT_OPT_NO_SERIALIZE	        = (1 << 0),
    GS_CORE_ASSET_IMPORT_OPT_ABSOLUTE_IMPORT_PATH   = (1 << 1)
};

typedef struct
{
    union
    {
        struct 
        {
            gs_graphics_texture_desc_t* desc;
            b32 keep_data;                      // Whether or not to keep texture data around for CPU access
        } texture;

        struct
        {
            uint32_t point_size;
        } font;

        struct 
        { 
            gs_gfxt_mesh_import_options_t* desc;
        } mesh; 

        struct
        {
            gs_gui_context_t* ctx;
        } gui;
    };

    const char* import_path;        // Path for importing
    const char* save_dir;           // Relative directory for saving asset
    uint32_t flags;                 // Flags for importing

} gs_core_asset_import_options_t;

typedef struct
{
    gs_core_asset_t* (* load_resource_from_file)(const char* path, gs_core_asset_import_options_t* options);
    void (* free_asset)(gs_core_asset_handle_t* hndl);
    gs_core_asset_t* (* register_default)();
    const char* resource_file_extensions[5];
    const char* asset_file_extension;
    uint64_t class_id;
    const char* save_dir;
    const char* file_name;
} gs_core_asset_importer_desc_t;

typedef struct gs_core_asset_importer_s
{
    gs_core_asset_t* (* load_resource_from_file)(const char* path, void* import_options);
    gs_core_asset_t* (* register_default)();
    void (* free_asset)(gs_core_asset_t* asset);
    gs_slot_array(gs_core_asset_record_t) records;                  // Loaded asset records
    gs_slot_array(gs_core_asset_t*) assets;                         // Slot array of raw asset data 
    gs_hash_table(uint64_t, uint32_t) uuid2id;                      // Mapping from uuid to record slot id
    gs_hash_table(uint64_t, uint32_t) name2id;                      // Mapping from qualified name to record slot id
    char file_extension[GS_CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH];  // File extension for asset
    uint64_t class_id;                                              // Class ID
    gs_core_asset_handle_t default_asset;                           // Default asset for this importer
    char save_dir[GS_CORE_ASSET_STR_MAX];                           // Default save directory for importer
    char file_name[GS_CORE_ASSET_STR_MAX];                          // Default file name for importer
} gs_core_asset_importer_t; 

//=====[ Texture Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_texture_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL void
gs_core_asset_texture_importer_free_asset(gs_core_asset_t* asset);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_texture_importer_register_default();

//=====[ Material Importer ]=====//

GS_API_DECL void
gs_core_asset_material_importer_free_asset(gs_core_asset_t* asset);

GS_API_DECL gs_core_asset_t*
gs_core_asset_material_importer_register_default();

//=====[ Mesh Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_mesh_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_mesh_importer_register_default(); 

GS_API_DECL void
gs_core_asset_mesh_importer_free_asset(gs_core_asset_t* asset);

//=====[ Pipeline Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_pipeline_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_pipeline_importer_register_default();

GS_API_DECL void
gs_core_asset_pipeline_importer_free_asset(gs_core_asset_t* asset);

//=====[ Font Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_font_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_font_importer_register_default(); 

GS_API_DECL void
gs_core_asset_font_importer_free_asset(gs_core_asset_t* asset);

//=====[ Audio Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_audio_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_audio_importer_register_default();

GS_API_DECL void
gs_core_asset_audio_importer_free_asset(gs_core_asset_t* asset);

//=====[ UIStyleSheet Importer ]=====//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_ui_stylesheet_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_t* 
gs_core_asset_ui_stylesheet_importer_register_default();

GS_API_DECL void
gs_core_asset_ui_stylesheet_importer_free_asset(gs_core_asset_t* asset);

//=====[ Assets ]=====//

typedef struct gs_core_assets_s
{ 
    char root_path[GS_CORE_ASSET_STR_MAX];
    gs_slot_array(gs_core_asset_importer_t) importers;
    gs_hash_table(uint64_t, uint32_t) cid2importer; // Mapping from class id to importer data
    gs_hash_table(uint64_t, uint32_t) fe2importer;  // Mapping file extension to importer data 
} gs_core_assets_t;

#define gs_core_assets_get_default(_T) gs_core_assets_get_default_impl(gs_core_cls_cid(_T))

GS_API_DECL gs_core_assets_t* 
gs_core_assets_new(const char* root_path);

GS_API_DECL void 
gs_core_assets_shutdown();

GS_API_DECL gs_core_assets_t* 
gs_core_assets_instance();

GS_API_DECL gs_core_asset_handle_t 
gs_core_assets_import(gs_core_asset_import_options_t* options);

GS_API_DECL gs_core_asset_handle_t
gs_core_assets_add(gs_core_asset_t* asset, gs_core_asset_import_options_t* options);

GS_API_PRIVATE gs_core_asset_handle_t
gs_core_assets_get_default_impl(uint64_t cls_id);

GS_API_DECL void 
gs_core_assets_serialize_asset(const char* path, const gs_core_asset_t* in);

GS_API_DECL void 
gs_core_assets_deserialize_asset(const char* path, gs_core_asset_t* out);

GS_API_DECL void
gs_core_asset_handle_free(const gs_core_asset_handle_t* hndl);

GS_API_DECL void
gs_core_asset_free(gs_core_asset_t* asset);

#endif // GS_CORE_ASSET_H






















