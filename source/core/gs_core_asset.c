/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: sm_asset.c

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

#include "core/gs_core_asset.h"
#include "core/gs_core.h"
#include "core/generated/gs_core_generated.h"

//===============[ Assets ]===============//

typedef struct 
{
    uint32_t start;
    uint32_t count;
} string_split_t;

// Functions 
static void _gs_core_asset_qualified_name(const char* src, char* dst, size_t sz)
{
    // Need to remove extension
    uint32_t idx = 0;
    gs_transient_buffer(TMP, 1024);
    for (uint32_t i = gs_string_length(src) - 1; i >= 0; --i) {
        if (src[i] == '.') {
            idx = i;
            break;
        }
    }
    gs_util_string_substring(src, TMP, 1024, 0, idx);

    // Split string
    gs_dyn_array(string_split_t) splits = NULL;
    gs_dyn_array_reserve(splits, 1);

    // Take in delimiter character, then split based on that
    size_t len = gs_string_length(TMP);
    uint32_t start = 0;
    uint32_t rem = 0;
    for (uint32_t i = 0; i < len; ++i)
    {
        if (TMP[i] == '/') {
            string_split_t split = gs_default_val();
            split.start = start;
            split.count = i - start;
            gs_dyn_array_push(splits, split);
            start = i;
            rem = len - start;
        }
    }
    // Push last bit into split
    string_split_t split = {0};
    split.start = start;
    split.count = rem;
    gs_dyn_array_push(splits, split);

    // If no splits, then push back entire string into split
    if (gs_dyn_array_empty(splits)) {
        string_split_t split = gs_default_val();
        split.start = 0; split.count = len;
        gs_dyn_array_push(splits, split);
    }
    else
    {
        // For each split, print
        uint32_t c = 0;
        gs_for_range_i(gs_dyn_array_size(splits))
        {
            string_split_t* s = &splits[i];
            gs_transient_buffer(TMP2, GS_CORE_ASSET_STR_MAX);
            gs_transient_buffer(TMP3, GS_CORE_ASSET_STR_MAX);
            memcpy(TMP2, (TMP + s->start), s->count);
            gs_util_string_replace_delim(TMP2, TMP3, GS_CORE_ASSET_STR_MAX, '/', '.');
            memcpy((dst + c), TMP3, s->count);
            c += s->count;
        }

        // Need the last remainder of the string as well
        gs_dyn_array_free(splits);
    }
} 

static void _gs_core_assets_register_importer(gs_core_assets_t* assets, gs_core_asset_importer_desc_t* desc)
{ 
    gs_core_asset_importer_t importer = {0};
    importer.load_resource_from_file = desc->load_resource_from_file;
    importer.register_defaults = desc->register_defaults;
    importer.free_asset = desc->free_asset;
    importer.class_id = desc->class_id;
    memcpy(importer.file_extension, desc->asset_file_extension, GS_CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH);
    
    uint32_t hndl = gs_slot_array_insert(assets->importers, importer);
    gs_hash_table_insert(assets->cid2importer, desc->class_id, hndl); 
    for (uint32_t i = 0; desc->resource_file_extensions[i] != NULL; ++i) {
        gs_hash_table_insert(assets->fe2importer, gs_hash_str64(desc->resource_file_extensions[i]), hndl); 
    }
}

static void _gs_core_assets_register_importers(gs_core_assets_t* assets) 
{
    uint32_t hndl = UINT32_MAX;

    // Register texture importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_texture_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_texture_importer_register_defaults,
        .free_asset = gs_core_asset_texture_importer_free_asset,
        .resource_file_extensions = {"png", "jpg", "tga"},
        .asset_file_extension = "tex",
        .class_id = gs_core_cls_cid(gs_core_asset_texture_t)
    });

    // Register material importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = NULL,
        .register_defaults = NULL,
        .free_asset = gs_core_asset_material_importer_free_asset,
        .resource_file_extensions = NULL,
        .asset_file_extension = "mat",
        .class_id = gs_core_cls_cid(gs_core_asset_material_t)
    });

    // Register mesh importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_mesh_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_mesh_importer_register_defaults,
        .free_asset = gs_core_asset_mesh_importer_free_asset,
        .resource_file_extensions = {"glb", "gltf"},
        .asset_file_extension = "msh",
        .class_id = gs_core_cls_cid(gs_core_asset_mesh_t)
    });

    // Register pipeline importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_pipeline_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_pipeline_importer_register_defaults,
        .free_asset = gs_core_asset_pipeline_importer_free_asset,
        .resource_file_extensions = {"sf"},
        .asset_file_extension = "pip",
        .class_id = gs_core_cls_cid(gs_core_asset_pipeline_t)
    });

    // Register font importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_font_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_font_importer_register_defaults,
        .free_asset = gs_core_asset_font_importer_free_asset,
        .resource_file_extensions = {"ttf", "otf"},
        .asset_file_extension = "fnt",
        .class_id = gs_core_cls_cid(gs_core_asset_font_t)
    });

    // Register audio importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_audio_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_audio_importer_register_defaults,
        .free_asset = gs_core_asset_audio_importer_free_asset,
        .resource_file_extensions = {"wav", "mp3", "ogg"},
        .asset_file_extension = "aud",
        .class_id = gs_core_cls_cid(gs_core_asset_audio_t)
    });

    // Register style sheet importer
    _gs_core_assets_register_importer(assets, &(gs_core_asset_importer_desc_t){
        .load_resource_from_file = gs_core_asset_ui_stylesheet_importer_load_resource_from_file,
        .register_defaults = gs_core_asset_ui_stylesheet_importer_register_defaults,
        .free_asset = gs_core_asset_ui_stylesheet_importer_free_asset,
        .resource_file_extensions = {"gss"},
        .asset_file_extension = "uss",
        .class_id = gs_core_cls_cid(gs_core_asset_ui_stylesheet_t)
    });
}

GS_API_DECL gs_core_assets_t* 
gs_core_assets_new(const char* root_path)
{
    // Get core instance
    gs_core_t* core = gs_core_instance();

    // Can only be ONE instance!
    if (core->assets) return core->assets;

    gs_core_assets_t* am = gs_malloc_init(gs_core_assets_t); 

    // Copy root path for assets
    memcpy(am->root_path, root_path, GS_CORE_ASSET_STR_MAX);

    // Register importers 
    _gs_core_assets_register_importers(am);

    // TODO(john): Pull in asset data from disk and create internal records on load

    return am;
}

GS_API_DECL void 
gs_core_assets_shutdown()
{
    // Get core instance
    gs_core_t* core = gs_core_instance();

    // Can only be ONE instance!
    if (core->assets) return core->assets;

    /*
typedef struct gs_core_assets_s
{ 
    char root_path[GS_CORE_ASSET_STR_MAX];
    gs_slot_array(gs_core_asset_importer_t) importers;
    gs_hash_table(uint64_t, uint32_t) cid2importer; // Mapping from class id to importer data
    gs_hash_table(uint64_t, uint32_t) fe2importer;  // Mapping file extension to importer data 
} gs_core_assets_t;
*/ 
    gs_core_assets_t* am = core->assets;
    
    // Free all importers
    for (
        gs_slot_array_iter it = gs_slot_array_iter_new(am->importers);
        gs_slot_array_iter_valid(am->importers, it);
        gs_slot_array_iter_advance(am->importers, it)
    )
    {
        
        /*
    typedef struct gs_core_asset_importer_s
    {
        gs_core_asset_t* (* load_resource_from_file)(const char* path, void* import_options);
        void (* register_defaults)();
        void (* free_asset)(gs_core_asset_handle_t* hndl);
        gs_slot_array(gs_core_asset_record_t) records;                  // Loaded asset records
        gs_slot_array(gs_core_asset_t*) assets;                         // Slot array of raw asset data 
        gs_hash_table(uint64_t, uint32_t) uuid2id;                      // Mapping from uuid to record slot id
        gs_hash_table(uint64_t, uint32_t) name2id;                      // Mapping from qualified name to record slot id
        char file_extension[GS_CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH];  // File extension for asset
        uint64_t class_id;                                              // Class ID
    } gs_core_asset_importer_t; 
    */

        gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, it);
        for (
            gs_slot_array_iter ait = gs_slot_array_iter_new(importer->records);
            gs_slot_array_iter_valid(importer->records, ait);
            gs_slot_array_iter_advance(importer->records, ait)
        )
        {
            gs_core_asset_record_t* record = gs_slot_array_iter_getp(importer->records, ait);
            gs_core_asset_handle_t handle = {
                .hndl = record->hndl,
                .importer = gs_hash_table_get(am->cid2importer, importer->class_id)
            };
            gs_core_asset_handle_free(&handle);
        }
        gs_hash_table_free(importer->uuid2id);
        gs_hash_table_free(importer->name2id);
        gs_slot_array_free(importer->records); 
        gs_slot_array_free(importer->assets);
    } 

    gs_hash_table_free(am->cid2importer);
    gs_hash_table_free(am->fe2importer); 
}

GS_API_DECL gs_core_assets_t* 
gs_core_assets_instance()
{
    return gs_core_instance()->assets;
}

GS_API_DECL gs_core_asset_handle_t gs_core_assets_import(const char* path, gs_core_asset_import_options_t* options, bool serialize)
{
    gs_core_assets_t* am = gs_core_assets_instance();

    // Create record for asset, set path to asset using qualified name
    gs_core_asset_record_t record = {0};

    // Asset handle to create and return
    gs_core_asset_handle_t asset_hndl = {0};

    // Get importer from importers
    gs_transient_buffer(FILE_EXT, 10);
    gs_platform_file_extension(FILE_EXT, 10, path);

    if (!gs_hash_table_exists(am->fe2importer, gs_hash_str64(FILE_EXT)))
    { 
        gs_log_warning("Assets::import::importer does not exist for file extension %s", FILE_EXT);
        return asset_hndl;
    }

    // Get asset storage
    uint32_t importer_hndl = gs_hash_table_get(am->fe2importer, gs_hash_str64(FILE_EXT));
    gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, importer_hndl); 

    // Get class id from storage
    uint64_t cid = importer->class_id;

    // Get absolute path to asset
    gs_snprintfc(PATH, GS_CORE_ASSET_STR_MAX, "%s/%s", am->root_path, path);

    // Get qualified name of asset
    gs_transient_buffer(QUAL_NAME, GS_CORE_ASSET_STR_MAX);
    _gs_core_asset_qualified_name(path, QUAL_NAME, GS_CORE_ASSET_STR_MAX);
    memcpy(record.name, QUAL_NAME, GS_CORE_ASSET_STR_MAX);

    // Create final save path for asset
    gs_transient_buffer(FINAL_PATH_TMP, GS_CORE_ASSET_STR_MAX);
    gs_transient_buffer(FINAL_PATH, GS_CORE_ASSET_STR_MAX);
    gs_snprintf(FINAL_PATH_TMP, GS_CORE_ASSET_STR_MAX, "%s/%s", am->root_path, QUAL_NAME);
    gs_util_string_replace_delim((FINAL_PATH_TMP + 1), (FINAL_PATH + 1), GS_CORE_ASSET_STR_MAX, '.', '/');
    FINAL_PATH[0] = '.'; 

    // Get file extension from registered mappings
    const char* file_ext = importer->file_extension;
    gs_snprintf(record.path, GS_CORE_ASSET_STR_MAX, "%s.%s", FINAL_PATH, file_ext);

    // Generate uuid for asset
    record.uuid = gs_platform_uuid_generate(); 

    gs_core_asset_t* asset = importer->load_resource_from_file(PATH, options); 
    if (!asset)
    {
        gs_log_warning("assets::import::failed to load resource from file: %s", path);
        return asset_hndl;
    } 

    gs_println("assets::import::imported %s to %s", path, record.path);

    // Insert into data array
    uint32_t hndl = gs_slot_array_insert(importer->assets, asset);

    // Set up tables
    gs_transient_buffer(UUID_BUF, 34);
    gs_platform_uuid_to_string(UUID_BUF, &record.uuid);
    gs_hash_table_insert(importer->uuid2id, gs_hash_str64(UUID_BUF), hndl);
    gs_hash_table_insert(importer->name2id, gs_hash_str64(record.name), hndl); 

    // Assign asset handle to record
    record.hndl = hndl;

    // Store record in storage
    uint32_t rhndl = gs_slot_array_insert(importer->records, record);

    // Set asset record hndl
    asset->record_hndl = rhndl; 

    // Serialize asset to disk
    if (serialize)
    {
        gs_core_assets_serialize_asset(record.path, asset);
    }

    asset_hndl.hndl = hndl;
    asset_hndl.importer = importer_hndl;
    
    return asset_hndl;
} 

GS_API_DECL gs_core_asset_handle_t
gs_core_assets_add(gs_core_asset_t* asset, bool serialize)
{ 
    gs_core_assets_t* am = gs_core_assets_instance();

    // Create record for asset, set path to asset using qualified name
    gs_core_asset_record_t record = {0};

    // Asset handle to create and return
    gs_core_asset_handle_t asset_hndl = {0};

    // Get importer from asset type id
    uint32_t importer_hndl = gs_hash_table_get(am->cid2importer, gs_core_obj_cid(asset));

    if (!importer_hndl)
    { 
        return asset_hndl;
    } 
      
    // Grab importer from id
    gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, importer_hndl); 

    // Generate uuid for asset
    record.uuid = gs_platform_uuid_generate(); 

    // Insert into data array
    asset_hndl.hndl = gs_slot_array_insert(importer->assets, asset);
    asset_hndl.importer = importer_hndl;

    // Set up tables
    gs_transient_buffer(UUID_BUF, 34);
    gs_platform_uuid_to_string(UUID_BUF, &record.uuid);
    gs_hash_table_insert(importer->uuid2id, gs_hash_str64(UUID_BUF), asset_hndl.hndl);
    // gs_hash_table_insert(importer->name2id, gs_hash_str64(record.name), hndl); 

    // Assign asset handle to record
    record.hndl = asset_hndl.hndl;

    // Store record in storage
    uint32_t rhndl = gs_slot_array_insert(importer->records, record);

    // Set asset record hndl
    asset->record_hndl = rhndl; 

    // Serialize asset to disk
    if (serialize)
    {
        gs_core_assets_serialize_asset(record.path, asset);
    } 
    
    return asset_hndl;
}

GS_API_DECL void 
gs_core_assets_serialize_asset(const char* path, const gs_core_asset_t* asset)
{
    gs_core_assets_t* am = gs_core_assets_instance();

    // Get class id from asset
    uint64_t cid = gs_core_obj_cid(gs_core_cast(asset, gs_core_obj_t));

    // Get asset importer based on type of asset
    uint32_t sid = gs_hash_table_get(am->cid2importer, cid);
    gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, sid);
    const gs_core_asset_record_t* record = gs_slot_array_getp(importer->records, asset->record_hndl);

    // Construct new buffer for writing asset data
    gs_byte_buffer_t bb = gs_byte_buffer_new();

    // === Object Header === //
    gs_byte_buffer_write(&bb, uint64_t, cid); // Class id (not sure about this) should write out class name instead?

    // === Asset Header === //
    gs_byte_buffer_write(&bb, gs_uuid_t, record->uuid);
    gs_byte_buffer_write_str(&bb, record->name);

    // Serialize asset data 
    gs_result res = gs_core_obj_serialize(&bb, gs_core_cast(asset, gs_core_obj_t));

    // Write to file
    gs_byte_buffer_write_to_file(&bb, path);

    // Free buffer
    gs_byte_buffer_free(&bb);
}

// Get access to raw asset from handle
GS_API_DECL gs_core_asset_t* 
gs_core_asset_handle_get(const gs_core_asset_handle_t* hndl)
{
    gs_core_assets_t* am = gs_core_assets_instance();
    const gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, hndl->importer); 
    return gs_slot_array_get(importer->assets, hndl->hndl);
} 

GS_API_DECL gs_core_asset_handle_t
gs_core_asset_handle_from_asset(const gs_core_asset_t* asset)
{
    gs_core_assets_t* am = gs_core_assets_instance();
    uint32_t ihndl = gs_hash_table_get(am->cid2importer, gs_core_obj_cid(asset));
    const gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, ihndl);
    const gs_core_asset_record_t* record = gs_slot_array_getp(importer->records, asset->record_hndl); 

    return (gs_core_asset_handle_t){
        .hndl = record->hndl,
        .importer = ihndl
    };
}

GS_API_DECL void 
gs_core_asset_free(gs_core_asset_t* asset)
{
    gs_core_assets_t* am = gs_core_assets_instance();
    uint32_t ihndl = gs_hash_table_get(am->cid2importer, gs_core_obj_cid(asset));
    const gs_core_asset_importer_t* importer = gs_slot_array_getp(am->importers, ihndl);
    uint32_t asset_hndl = (gs_slot_array_getp(importer->records, asset->record_hndl))->hndl;

    // Free asset
    importer->free_asset(asset);

    // Erase record handle
    gs_slot_array_erase(importer->records, asset->record_hndl);

    // Erase asset then free
    gs_slot_array_erase(importer->assets, asset_hndl);
    gs_free(asset);
}

GS_API_DECL void
gs_core_asset_handle_free(const gs_core_asset_handle_t* hndl)
{ 
    gs_core_assets_t* am = gs_core_assets_instance();
    gs_core_asset_free(gs_core_asset_handle_get(hndl)); 
}

//================[ Texture ]===================// 

GS_API_DECL struct gs_core_asset_importer_s*
gs_core_asset_texture_importer()
{
    return NULL;
}

GS_API_DECL gs_result
gs_core_asset_texture_serialize(gs_byte_buffer_t* buffer, const gs_core_obj_t* obj)
{
    return GS_RESULT_INCOMPLETE;
}

GS_API_DECL gs_result
gs_core_asset_texture_deserialize(gs_byte_buffer_t* buffer, gs_core_obj_t* obj)
{
    return GS_RESULT_INCOMPLETE;
}

//================[ Mesh ]===================// 

//================[ Material ]===================// 

//================[ Pipeline ]===================// 

//================[ Font ]===================// 

//================[ Audio ]===================// 

//================[ UIStyleSheet ]===================//

//================[ Texture Importer ]===================//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_texture_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{
    gs_core_asset_texture_t* texture = gs_core_cls_new(gs_core_asset_texture_t); 
    gs_graphics_texture_desc_t* tdesc = options ? &options->texture.desc : NULL;
    texture->resource = gs_gfxt_texture_load_from_file(path, tdesc, false, false); 
    return gs_core_cast(texture, gs_core_asset_t);
}

GS_API_DECL void 
gs_core_asset_texture_importer_register_defaults()
{ 
    gs_core_asset_texture_t* texture = gs_core_cls_new(gs_core_asset_texture_t); 
    texture->resource = gs_gfxt_texture_generate_default();
} 

GS_API_DECL void
gs_core_asset_texture_importer_free_asset(gs_core_asset_t* asset)
{ 
    gs_core_asset_texture_t* texture = (gs_core_asset_texture_t*)asset;
    gs_gfxt_texture_destroy(&texture->resource);
} 

//================[ Material Importer ]===================//

GS_API_DECL void
gs_core_asset_material_importer_free_asset(gs_core_asset_t* asset)
{
    gs_core_asset_material_t* material = (gs_core_asset_material_t*)asset;
    gs_gfxt_material_destroy(&material->resource);
}

//================[ Mesh Importer ]===================//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_mesh_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{
    gs_core_asset_mesh_t* msh = gs_core_cls_new(gs_core_asset_mesh_t); 
    gs_gfxt_mesh_import_options_t* desc = options ? &options->mesh.desc : NULL;
    msh->resource = gs_gfxt_mesh_load_from_file(path, desc);
    return gs_core_cast(msh, gs_core_asset_t);
}

GS_API_DECL void 
gs_core_asset_mesh_importer_register_defaults()
{ 
    gs_core_asset_mesh_t* msh = gs_core_cls_new(gs_core_asset_mesh_t); 
    msh->resource = gs_gfxt_mesh_unit_quad_generate(NULL);
} 

GS_API_DECL void
gs_core_asset_mesh_importer_free_asset(gs_core_asset_t* asset)
{ 
    gs_core_asset_mesh_t* mesh = (gs_core_asset_mesh_t*)asset;
    gs_gfxt_mesh_destroy(&mesh->resource);
} 

//================[ Pipeline Importer ]===================//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_pipeline_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{ 
    gs_core_asset_pipeline_t* pip = gs_core_cls_new(gs_core_asset_pipeline_t);
    pip->resource = gs_gfxt_pipeline_load_from_file(path); 
    return gs_core_cast(pip, gs_core_asset_t);
}

GS_API_DECL void 
gs_core_asset_pipeline_importer_register_defaults()
{
} 

GS_API_DECL void
gs_core_asset_pipeline_importer_free_asset(gs_core_asset_t* asset)
{ 
    gs_core_asset_pipeline_t* pip = (gs_core_asset_pipeline_t*)asset;
    gs_gfxt_pipeline_destroy(&pip->resource);
} 

//================[ Font Importer ]===================//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_font_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{
    gs_core_asset_font_t* font = gs_core_cls_new(gs_core_asset_font_t); 
    uint32_t point_size = options ? options->font.point_size : 16;
    gs_asset_font_load_from_file(path, &font->resource, point_size);
    return gs_core_cast(font, gs_core_asset_t);
}

GS_API_DECL void 
gs_core_asset_font_importer_register_defaults()
{
}

GS_API_DECL void
gs_core_asset_font_importer_free_asset(gs_core_asset_t* asset)
{ 
    // Not sure about this one yet...
} 

//================[ Audio Importer ]===================//

GS_API_DECL gs_core_asset_t* 
gs_core_asset_audio_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{
    gs_core_asset_audio_t* aud = gs_core_cls_new(gs_core_asset_audio_t);
    gs_asset_audio_load_from_file(path, &aud->resource);
    return gs_core_cast(aud, gs_core_asset_t);
}

GS_API_DECL void 
gs_core_asset_audio_importer_register_defaults()
{
}

GS_API_DECL void
gs_core_asset_audio_importer_free_asset(gs_core_asset_t* asset)
{ 
    // Not sure about this one either...
} 

//================[ UI StyleSheet Importer ]===================//

GS_API_DECL gs_core_asset_t* gs_core_asset_ui_stylesheet_importer_load_resource_from_file(const char* path, gs_core_asset_import_options_t* options)
{
    gs_assert(options && options->gui.ctx);
    gs_core_asset_ui_stylesheet_t* ss = gs_core_cls_new(gs_core_asset_ui_stylesheet_t);
    gs_gui_context_t* ctx = options->gui.ctx;
    ss->resource = gs_gui_style_sheet_load_from_file(ctx, path);
    return gs_core_cast(ss, gs_core_asset_t);
}

GS_API_DECL void gs_core_asset_ui_stylesheet_importer_register_defaults() 
{
} 

GS_API_DECL void
gs_core_asset_ui_stylesheet_importer_free_asset(gs_core_asset_t* asset)
{ 
    gs_core_asset_ui_stylesheet_t* ss = (gs_core_asset_ui_stylesheet_t*)asset;
    gs_gui_style_sheet_destroy(&ss->resource);
} 






