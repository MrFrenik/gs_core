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

#include "core_asset.h"

//===============[ Assets ]===============//

static core_assets_t* g_assets = NULL;   // Global, static asset manager instance 

typedef struct 
{
    uint32_t start;
    uint32_t count;
} string_split_t;

// Functions 
static void _core_asset_qualified_name(const char* src, char* dst, size_t sz)
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
            gs_transient_buffer(TMP2, CORE_ASSET_STR_MAX);
            gs_transient_buffer(TMP3, CORE_ASSET_STR_MAX);
            memcpy(TMP2, (TMP + s->start), s->count);
            gs_util_string_replace(TMP2, TMP3, CORE_ASSET_STR_MAX, '/', '.');
            memcpy((dst + c), TMP3, s->count);
            c += s->count;
        }

        // Need the last remainder of the string as well
        gs_dyn_array_free(splits);
    }
} 

static void _core_assets_register_importer(core_assets_t* assets, core_asset_importer_desc_t* desc)
{ 
    core_asset_importer_t importer = {0};
    importer.load_resource_from_file = desc->load_resource_from_file;
    importer.register_defaults = desc->register_defaults;
    importer.class_id = desc->class_id;
    memcpy(importer.file_extension, desc->asset_file_extension, CORE_ASSETS_FILE_EXTENSION_MAX_LENGTH);
    
    uint32_t hndl = gs_slot_array_insert(assets->importers, importer);
    gs_hash_table_insert(assets->cid2importer, core_cls_cid(core_texture_t), hndl); 
    for (uint32_t i = 0; desc->resource_file_extensions[i] != NULL; ++i) {
        gs_hash_table_insert(assets->fe2importer, gs_hash_str64(desc->resource_file_extensions[i]), hndl); 
    }
}

static void _core_assets_register_importers(core_assets_t* assets) 
{
    uint32_t hndl = UINT32_MAX;

    // Register texture importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_texture_importer_load_resource_from_file,
        .register_defaults = core_texture_importer_register_defaults,
        .resource_file_extensions = {"png", "jpg", "tga"},
        .asset_file_extension = "tex",
        .class_id = core_cls_cid(core_texture_t)
    });

    // Register mesh importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_mesh_importer_load_resource_from_file,
        .register_defaults = core_mesh_importer_register_defaults,
        .resource_file_extensions = {"glb", "gltf"},
        .asset_file_extension = "msh",
        .class_id = core_cls_cid(core_mesh_t)
    });

    // Register pipeline importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_pipeline_importer_load_resource_from_file,
        .register_defaults = core_pipeline_importer_register_defaults,
        .resource_file_extensions = {"gpip"},
        .asset_file_extension = "pip",
        .class_id = core_cls_cid(core_pipeline_t)
    });

    // Register font importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_font_importer_load_resource_from_file,
        .register_defaults = core_font_importer_register_defaults,
        .resource_file_extensions = {"ttf", "otf"},
        .asset_file_extension = "fnt",
        .class_id = core_cls_cid(core_font_t)
    });

    // Register audio importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_audio_importer_load_resource_from_file,
        .register_defaults = core_audio_importer_register_defaults,
        .resource_file_extensions = {"wav", "mp3", "ogg"},
        .asset_file_extension = "aud",
        .class_id = core_cls_cid(core_audio_t)
    });

    // Register style sheet importer
    _core_assets_register_importer(assets, &(core_asset_importer_desc_t){
        .load_resource_from_file = core_ui_stylesheet_importer_load_resource_from_file,
        .register_defaults = core_ui_stylesheet_importer_register_defaults,
        .resource_file_extensions = {"gss"},
        .asset_file_extension = "uss",
        .class_id = core_cls_cid(core_ui_stylesheet_t)
    });
}

GS_API_DECL core_assets_t* core_assets_new(const char* root_path)
{
    // Can only be ONE instance!
    if (g_assets) return g_assets;

    core_assets_t* am = gs_malloc_init(core_assets_t);

    // Set global static instance
    g_assets = am;

    // Copy root path for assets
    memcpy(am->root_path, root_path, CORE_ASSET_STR_MAX);

    // Register importers 
    _core_assets_register_importers(am);

    // Pull in asset data from disk and create internal records
}

GS_API_DECL core_assets_t* core_assets_instance()
{
    return g_assets;
}

GS_API_DECL core_asset_handle_t core_assets_import(const char* path, core_asset_import_options_t* options, bool serialize)
{
    core_assets_t* am = core_assets_instance();

    // Create record for asset, set path to asset using qualified name
    core_asset_record_t record = {0};

    // Asset handle to create and return
    core_asset_handle_t asset_hndl = {0};

    // Get importer from importers
    gs_transient_buffer(FILE_EXT, 10);
    gs_platform_file_extension(FILE_EXT, 10, path);

    if (!gs_hash_table_exists(am->fe2importer, gs_hash_str64(FILE_EXT)))
    { 
        gs_log_warning("Assets::import::importer does not exist for file extension %s", FILE_EXT);
        return;
    }

    // Get asset storage
    uint32_t importer_hndl = gs_hash_table_get(am->fe2importer, gs_hash_str64(FILE_EXT));
    core_asset_importer_t* importer = gs_slot_array_getp(am->importers, importer_hndl); 

    // Get class id from storage
    uint64_t cid = importer->class_id;

    // Get absolute path to asset
    gs_snprintfc(PATH, CORE_ASSET_STR_MAX, "%s/%s", am->root_path, path);

    // Get qualified name of asset
    gs_transient_buffer(QUAL_NAME, CORE_ASSET_STR_MAX);
    _core_asset_qualified_name(path, QUAL_NAME, CORE_ASSET_STR_MAX);
    memcpy(record.name, QUAL_NAME, CORE_ASSET_STR_MAX);

    // Create final save path for asset
    gs_transient_buffer(FINAL_PATH_TMP, CORE_ASSET_STR_MAX);
    gs_transient_buffer(FINAL_PATH, CORE_ASSET_STR_MAX);
    gs_snprintf(FINAL_PATH_TMP, CORE_ASSET_STR_MAX, "%s/%s", am->root_path, QUAL_NAME);
    gs_util_string_replace((FINAL_PATH_TMP + 1), (FINAL_PATH + 1), CORE_ASSET_STR_MAX, '.', '/');
    FINAL_PATH[0] = '.'; 

    // Get file extension from registered mappings
    const char* file_ext = importer->file_extension;
    gs_snprintf(record.path, CORE_ASSET_STR_MAX, "%s.%s", FINAL_PATH, file_ext);

    // Generate uuid for asset
    record.uuid = gs_platform_uuid_generate(); 

    core_asset_t* asset = importer->load_resource_from_file(PATH, options); 
    if (!asset)
    {
        gs_log_warning("assets::import::failed to load resource from file: %s", path);
        return;
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
        core_assets_serialize_asset(record.path, asset);
    }

    asset_hndl.hndl = hndl;
    asset_hndl.importer = importer_hndl;
    
    return asset_hndl;
} 

void core_assets_serialize_asset(const char* path, const core_asset_t* asset)
{
    core_assets_t* am = core_assets_instance();

    // Get class id from asset
    uint64_t cid = core_obj_cid(asset);

    // Get asset importer based on type of asset
    uint32_t sid = gs_hash_table_get(am->cid2importer, cid);
    core_asset_importer_t* importer = gs_slot_array_getp(am->importers, sid);
    const core_asset_record_t* record = gs_slot_array_getp(importer->records, asset->record_hndl);

    // Construct new buffer for writing asset data
    gs_byte_buffer_t bb = gs_byte_buffer_new();

    // === Object Header === //
    gs_byte_buffer_write(&bb, uint64_t, cid); // Class id (not sure about this) should write out class name instead?

    // === Asset Header === //
    gs_byte_buffer_write(&bb, gs_uuid_t, record->uuid);
    gs_byte_buffer_write_str(&bb, record->name);

    // Serialize asset data 
    gs_result res = core_obj_serialize(&bb, asset);

    // Default serialization if no serialization provided
    if (res == GS_RESULT_INCOMPLETE)
    {
        res = core_obj_serialize_default(&bb, asset);
    }

    // Write to file
    gs_byte_buffer_write_to_file(&bb, path);

    // Free buffer
    gs_byte_buffer_free(&bb);
}

// Get access to raw asset from handle
core_asset_t* core_asset_handle_get(const core_asset_handle_t* hndl)
{
    core_assets_t* am = core_assets_instance();
    const core_asset_importer_t* importer = gs_slot_array_getp(am->importers, hndl->importer); 
    return gs_slot_array_get(importer->assets, hndl->hndl);
}

//================[ Core_Texture ]===================// 

//================[ Core_Mesh ]===================// 

//================[ Core_Material ]===================// 

//================[ Pipeline ]===================// 

//================[ Font ]===================// 

//================[ Core_Audio ]===================// 

//================[ Core UIStyleSheet ]===================//

//================[ TextureImporter ]===================//

core_asset_t* core_texture_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{
    core_texture_t* texture = core_cls_new(core_texture_t); 
    gs_graphics_texture_desc_t* tdesc = options ? &options->texture.desc : NULL;
    texture->texture = gs_gfxt_texture_load_from_file(path, tdesc, false, false); 
    return texture;
}

void core_texture_importer_register_defaults()
{ 
    core_texture_t* texture = core_cls_new(core_texture_t); 
    texture->texture = gs_gfxt_texture_generate_default();
} 

//================[ MeshImporter ]===================//

core_asset_t* core_mesh_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{
    core_mesh_t* msh = core_cls_new(core_mesh_t); 
    gs_gfxt_mesh_import_options_t* desc = options ? &options->mesh.desc : NULL;
    msh->mesh = gs_gfxt_mesh_load_from_file(path, desc);
    return msh;
}

void core_mesh_importer_register_defaults()
{ 
    core_mesh_t* msh = core_cls_new(core_mesh_t); 
    msh->mesh = gs_gfxt_mesh_unit_quad_generate(NULL);
} 

//================[ PipelineImporter ]===================//

GS_API_DECL core_asset_t* core_pipeline_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{ 
    core_pipeline_t* pip = core_cls_new(core_pipeline_t);
    pip->pipeline = gs_gfxt_pipeline_load_from_file(path); 
    return pip;
}

GS_API_DECL void core_pipeline_importer_register_defaults()
{
} 

//================[ FontImporter ]===================//

GS_API_DECL core_asset_t* core_font_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{
    core_font_t* font = core_cls_new(core_font_t); 
    uint32_t point_size = options ? options->font.point_size : 16;
    gs_asset_font_load_from_file(path, &font->font, point_size);
    return font;
}

GS_API_DECL void core_font_importer_register_defaults()
{
}

//================[ AudioImporter ]===================//

GS_API_DECL core_asset_t* core_audio_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{
    core_audio_t* aud = core_cls_new(core_audio_t);
    gs_asset_audio_load_from_file(path, &aud->audio);
    return aud;
}

GS_API_DECL void core_audio_importer_register_defaults()
{
}

//================[ Core_UIStyleSheetImporter ]===================//

GS_API_DECL core_asset_t* core_ui_stylesheet_importer_load_resource_from_file(const char* path, core_asset_import_options_t* options)
{
    gs_assert(options && options->gui.ctx);
    core_ui_stylesheet_t* ss = core_cls_new(core_ui_stylesheet_t);
    gs_gui_context_t* ctx = options->gui.ctx;
    ss->style_sheet = gs_gui_style_sheet_load_from_file(ctx, path);
    return ss;
}

GS_API_DECL void core_ui_stylesheet_importer_register_defaults() 
{
} 






