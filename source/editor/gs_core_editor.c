/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_editor.c

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

// Core Includes
#include "editor/views/gs_core_editor_view_outliner.h"
#include "editor/views/gs_core_editor_view_properties.h"
#include "editor/views/gs_core_editor_view_pie.h"
#include "editor/views/gs_core_editor_view_assets.h"
#include "editor/views/gs_core_editor_view_scene.h"
#include "editor/gs_core_editor.h"
#include "core/gs_core_app.h" 

GS_API_DECL void gs_core_editor_app_hot_reload();
GS_API_DECL void gs_core_editor_library_load();
GS_API_DECL void gs_core_editor_library_unload();

// Global editor instance
static gs_core_editor_t* g_editor = NULL; 
static char g_editor_path[256] = {0};
static char g_app_path[256] = {0};

#define GS_CORE_EDITOR_DOCKSPACE_NAME "Dockspace##gs_core_editor"

GS_API_DECL gs_core_editor_t* 
gs_core_editor_instance()
{ 
    return g_editor;
} 

GS_API_DECL void 
gs_core_editor_init()
{ 
    // Set global instance
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);
    gs_assert(editor);

    // Init core
    editor->core = gs_core_new();

    // Register editor meta
    _gs_core_editor_meta_register();

    // Init gui
    editor->gui = gs_gui_context_new(gs_platform_main_window()); 

    // Initialize paths
    gs_snprintf(g_app_path, sizeof(g_app_path), "%s.%s", gs_app_dll_path(), GS_CORE_EDITOR_DYNAMIC_LIB_EXT);
    gs_snprintf(g_editor_path, sizeof(g_editor_path), "%s.%s", gs_core_editor_dll_path(), GS_CORE_EDITOR_DYNAMIC_LIB_EXT);

    // Dock editor views (set for now by default)
    gs_gui_context_t* gui = &editor->gui;

    // Register core editor views
    gs_core_editor_view_t* scene = gs_core_editor_view_register(gs_core_editor_view_scene_t);
    gs_core_editor_view_t* outliner = gs_core_editor_view_register(gs_core_editor_view_outliner_t);
    gs_core_editor_view_t* properties = gs_core_editor_view_register(gs_core_editor_view_properties_t);

    // Set up splits
    gs_gui_dock_ex(gui, scene->name, GS_CORE_EDITOR_DOCKSPACE_NAME, GS_GUI_SPLIT_BOTTOM, 1.f);
    gs_gui_dock_ex(gui, outliner->name, scene->name, GS_GUI_SPLIT_RIGHT, 0.3f);
    gs_gui_dock_ex(gui, properties->name, outliner->name, GS_GUI_SPLIT_BOTTOM, 0.7f);

    // Load app .dll if available
    gs_core_editor_app_hot_reload();

    // Initialize file stats
    editor->app.fstats = gs_platform_file_stats(g_app_path);
}

GS_API_DECL void
gs_core_editor_update()
{
    // Grab application and all required gs structures
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);
    gs_command_buffer_t* cb = &editor->core->cb;
    gs_immediate_draw_t* gsi = &editor->core->gsi;
    gs_gui_context_t* gui = &editor->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 

    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) 
    {
        gs_quit();
    }

    // App update
    if (editor->app.app)
    { 
        if (gs_platform_key_pressed(GS_KEYCODE_O))
        { 
            gs_core_app_state state = editor->app.app->state;
            state = state == GS_CORE_APP_STATE_STOPPED ? GS_CORE_APP_STATE_PLAYING : 
                                                         GS_CORE_APP_STATE_STOPPED;
            editor->app.app->state = state;
        }

        _gs_core_app_update();
    }

    // Check if application needs to be reloaded
    gs_platform_file_stats_t cfstats = gs_platform_file_stats(g_app_path);
    if (gs_platform_file_compare_time(editor->app.fstats.modified_time, cfstats.modified_time))
    { 
        if (!editor->app.hot_reload_begin)
        {
            gs_println("Initialize hot reload..."); 
            editor->app.hot_reload_begin = true; 
        }
    }
    else if (editor->app.hot_reload_begin)
    { 
        gs_core_editor_app_hot_reload();
    } 
    
    editor->app.fstats = cfstats;

    // Gui 
    gs_gui_begin(gui, &(gs_gui_hints_t){.framebuffer_size = fbs, .viewport = gs_gui_rect(0.f, 0.f, fbs.x, fbs.y), 
        .flags = GS_GUI_HINT_FLAG_NO_INVERT_Y | GS_GUI_HINT_FLAG_NO_SCALE_BIAS_MOUSE});
    {
        // Dockspace
        int32_t opt = 
            GS_GUI_OPT_NOCLIP | 
            GS_GUI_OPT_NOFRAME | 
            GS_GUI_OPT_FORCESETRECT | 
            GS_GUI_OPT_NOTITLE | 
            GS_GUI_OPT_DOCKSPACE | 
            GS_GUI_OPT_FULLSCREEN | 
            GS_GUI_OPT_NOMOVE | 
            GS_GUI_OPT_NOBRINGTOFRONT | 
            GS_GUI_OPT_NOFOCUS | 
            GS_GUI_OPT_NORESIZE;

        gs_gui_window_begin_ex(gui, "Dockspace##gs_core_editor", gs_gui_rect(350, 40, 600, 500), NULL, NULL, opt);
        {
            // Empty dockspace...
        }
        gs_gui_window_end(gui);

        // Iterate through all views
        for (
            gs_hash_table_iter it = gs_hash_table_iter_new(editor->views); 
            gs_hash_table_iter_valid(editor->views, it); 
            gs_hash_table_iter_advance(editor->views, it)
        )
        { 
            gs_core_editor_view_t* view = gs_hash_table_iter_get(editor->views, it);
            gs_gui_window_begin_ex(gui, view->name, gs_gui_rect(350, 40, 600, 500), NULL, NULL, GS_GUI_OPT_NOSCROLL);
            {
                // Editor callback
                gs_core_cast_vt(view, gs_core_editor_view_t)->callback(view);
            }
            gs_gui_window_end(gui);
        } 
    } 
    gs_gui_end(gui);

    // Submit gsi
    gsi_renderpass_submit(gsi, cb, (uint32_t)fbs.x, (uint32_t)fbs.y, gs_color(10, 10, 10, 255)); 

    // Submit gui
    gs_gui_renderpass_submit_ex(gui, cb, NULL);

    // Submit command buffer for GPU
    gs_core_graphics_instance()->submit(cb);
}

GS_API_DECL void 
gs_core_editor_shutdown()
{
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);

    // Unload app 
    gs_core_editor_library_unload();

    // Unegister editor meta
    _gs_core_editor_meta_unregister();

    // Shutdown core
    gs_core_free(editor->core);
}

GS_API_DECL void
gs_core_editor_view_set_name(gs_core_editor_view_t* view, const char* name)
{ 
    if (!view || !name)
    {
        return;
    }

    memcpy(view->name, name, GS_CORE_EDITOR_VIEW_STR_MAX);
}

GS_API_PRIVATE gs_core_editor_view_t*
_gs_core_editor_view_register_internal(gs_core_editor_t* (* obj_new_f)(), const char* name) 
{ 
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);

    // Construct view on heap and cast
    gs_core_obj_t* obj = obj_new_f();
    gs_core_editor_view_t* view = gs_core_cast(obj, gs_core_editor_view_t);

    // Assert if required vtable functions not provided 
    gs_core_vt(gs_core_editor_view_t)* vt = gs_core_cast_vt(view, gs_core_editor_view_t);

    if (!vt || !vt->callback) {
        gs_log_error("View required to override callback vtable function: %s", view->name); 
    }

    // If not provided a valid name, then use type
    if (!gs_string_length(view->name)) {
        memcpy(view->name, name, GS_CORE_EDITOR_VIEW_STR_MAX);
    }

    // If exists, error
    uint64_t hash = gs_hash_str64(view->name);
    if (gs_hash_table_exists(editor->views, hash)) {
        gs_log_error("View already registered with name: %s", view->name);
    }
    gs_hash_table_insert(editor->views, hash, view);

    // If no docking available, then need to dock to dockspace...
    // gs_gui_dock_ex(gui, desc->name, parent, split, amount); 

    return view;
}

GS_API_DECL void 
gs_core_editor_library_load()
{
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);

    editor->app.dll = gs_platform_library_load(g_editor_path); 

    if (editor->app.dll) 
    {
        gs_println("App Loaded!");
        editor->app.new = gs_platform_library_proc_address(editor->app.dll, "_gs_core_app_new");
        editor->app.free = gs_platform_library_proc_address(editor->app.dll, "_gs_core_app_free");
        editor->app.meta_register = gs_platform_library_proc_address(editor->app.dll, "_gs_core_app_meta_register");
        editor->app.meta_unregister = gs_platform_library_proc_address(editor->app.dll, "_gs_core_app_meta_unregister");
        gs_assert(editor->app.new);
        gs_assert(editor->app.free);
        gs_assert(editor->app.meta_register);
        gs_assert(editor->app.meta_unregister);

        // Construct application pointer (this should just register everything, honestly...)
        editor->app.app = editor->app.new(gs_instance(), editor->core);

        // Set app instance
        gs_core_app_instance_set(editor->app.app); 

        // Register meta information
        editor->app.meta_register();

        // Initialize application
        _gs_core_app_init();
    } 
}

GS_API_DECL void 
gs_core_editor_library_unload()
{ 
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t);

    if (editor->app.dll) 
    {
        // Shutdown application and free memory 
        _gs_core_app_shutdown();
        editor->app.meta_unregister();
        editor->app.free(editor->app.app);
        gs_println("App Unloaded!");
        gs_platform_library_unload(editor->app.dll); 
        gs_core_app_instance_set(NULL); 
        editor->app.dll = NULL;
        editor->app.app = NULL;
        editor->app.new = NULL;
        editor->app.meta_register = NULL;
        editor->app.meta_unregister = NULL;
    }
} 

GS_API_DECL void 
gs_core_editor_app_hot_reload()
{
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t); 

    // Determine if can copy first before unloading
    if (!gs_platform_file_exists(g_app_path))
    {
        return;
    }

    // Remove previous .dll
    if (gs_platform_file_exists(g_editor_path))
    {
        gs_core_editor_library_unload();
        gs_println("Deleting old dll...");
        gs_assert(gs_platform_file_delete(g_editor_path)); 
    } 

    // Copy over new
    if (gs_platform_file_copy(g_app_path, g_editor_path))
    {
        gs_println("Copied new dll!");
        gs_core_editor_library_load();
    }
    editor->app.hot_reload_begin = false; 
    gs_println("Hot reload finished..."); 
}











