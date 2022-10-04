
// Includes
#include "editor/gs_editor.h" 
#include "editor/gs_editor_view_scene.h"
#include "editor/gs_editor_view_outliner.h"
#include "editor/gs_editor_view_properties.h"
#include "core/gs_core_app.h"

GS_API_DECL void gs_editor_app_hot_reload();
GS_API_DECL void gs_editor_library_load();
GS_API_DECL void gs_editor_library_unload();

// Global editor instance
static gs_editor_t* g_editor = NULL;

GS_API_DECL gs_editor_t* 
gs_editor_instance()
{ 
    return g_editor;
} 

GS_API_DECL void 
gs_editor_init(void* app)
{ 
    // Set global instance
    g_editor = app; 
    gs_editor_t* editor = (gs_editor_t*)app;
    gs_assert(editor);

    // Init core
    editor->core = gs_core_new();

    // Dock editor views (set for now by default)
    gs_gui_context_t* gui = &editor->core->gui;

    // Scene view
    gs_editor_view_register(&(gs_editor_view_desc_t){
        .name = "Scene##editor",
        .cb = gs_editor_view_scene_cb,
        .split = {
            .parent = "Dockspace##editor",
            .type = GS_GUI_SPLIT_BOTTOM,
            .amount = 1.f
        }
    });
    
    // Outliner view
    gs_editor_view_register(&(gs_editor_view_desc_t){
        .name = "Outliner##editor", 
        .cb = gs_editor_view_outliner_cb,
        .split = {
            .parent = "Scene##editor", 
            .type = GS_GUI_SPLIT_RIGHT, 
            .amount = 0.3f
        }
    });

    // Properties view
    gs_editor_view_register(&(gs_editor_view_desc_t){
        .name = "Properties##editor", 
        .cb = gs_editor_view_properties_cb,
        .split = {
            .parent = "Outliner##editor", 
            .type = GS_GUI_SPLIT_BOTTOM, 
            .amount = 0.7f
        }
    }); 

    // Load app .dll if available
    gs_editor_app_hot_reload();

    // Initialize file stats
    editor->app.fstats = gs_platform_file_stats(gs_app_dll_path());
}

GS_API_DECL void
gs_editor_update(void* app)
{
    // Grab application and all required gs structures
    gs_editor_t* editor = (gs_editor_t*)app;
    gs_command_buffer_t* cb = &editor->core->cb;
    gs_immediate_draw_t* gsi = &editor->core->gsi;
    gs_gui_context_t* gui = &editor->core->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 

    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) 
    {
        gs_quit();
    } 

    // Check if application needs to be reloaded
    gs_platform_file_stats_t cfstats = gs_platform_file_stats(gs_app_dll_path());
    if (gs_platform_file_compare_time(editor->app.fstats.modified_time, cfstats.modified_time))
    { 
        if (!editor->app.hot_reload_begin)
        {
            gs_println("Initialize hot reload..."); 
            editor->app.hot_reload_begin = true; 
        }
        editor->app.hot_reload_timer = 0;
    }
    else if (editor->app.hot_reload_begin && editor->app.hot_reload_timer >= 30)
    { 
        gs_editor_app_hot_reload();

        gs_println("Hot reload finished..."); 
    } 
    else if (editor->app.hot_reload_begin)
    {
        editor->app.hot_reload_timer++;
    } 
    
    editor->app.fstats = cfstats;

    // Gui
    gs_gui_begin(gui, fbs);
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

        gs_gui_window_begin_ex(gui, "Dockspace##editor", gs_gui_rect(350, 40, 600, 500), NULL, NULL, opt);
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
            gs_editor_view_t* view = gs_hash_table_iter_getp(editor->views, it);
            gs_gui_window_begin_ex(gui, view->name, gs_gui_rect(350, 40, 600, 500), NULL, NULL, GS_GUI_OPT_NOSCROLL);
            {
                view->cb(view);
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
gs_editor_shutdown(void* app)
{
    gs_editor_t* editor = (gs_editor_t*)app;

    // Shutdown core
    gs_core_free(editor->core);
}

GS_API_DECL void
gs_editor_view_register(const gs_editor_view_desc_t* desc)
{ 
    // Assert desc has a valid name
    gs_editor_t* editor = gs_user_data(gs_editor_t);
    gs_assert(desc->name);

    // Assert view doesn't already exist
    uint64_t hash = gs_hash_str64(desc->name);
    gs_assert(!gs_hash_table_exists(editor->views, hash));

    // Construct new editor view
    gs_editor_view_t view = (gs_editor_view_t){
        .cb = desc->cb
    };
    memcpy(view.name, desc->name, GS_EDITOR_VIEW_STR_MAX);

    // Add to table
    gs_hash_table_insert(editor->views, hash, view); 

    // Dock view
    gs_gui_context_t* gui = &editor->core->gui;
    const char* parent = "Dockspace##editor";
    gs_gui_split_type split = GS_GUI_SPLIT_BOTTOM;
    float amount = 0.5f;
    if (desc->split.parent)
    {
        parent = desc->split.parent;
        amount = desc->split.amount;
        split = desc->split.type;
    }
    gs_gui_dock_ex(gui, desc->name, parent, split, amount); 
}

GS_API_DECL void 
gs_editor_library_load()
{
    gs_editor_t* editor = gs_user_data(gs_editor_t);

    editor->app.dll = gs_platform_library_load(gs_editor_dll_path());

    if (editor->app.dll) 
    {
        gs_println("App Loaded!");
        editor->app.new = gs_platform_library_proc_address(editor->app.dll, "_gs_app_new");
        editor->app.init = gs_platform_library_proc_address(editor->app.dll, gs_app_init_func_name());
        editor->app.update = gs_platform_library_proc_address(editor->app.dll, gs_app_update_func_name());
        editor->app.shutdown = gs_platform_library_proc_address(editor->app.dll, gs_app_shutdown_func_name());
        editor->scene_draw_cb = gs_platform_library_proc_address(editor->app.dll, gs_app_scene_render_cb_name());
        gs_assert(editor->app.new);
        gs_assert(editor->app.init);
        gs_assert(editor->app.update);
        gs_assert(editor->app.shutdown);
        gs_assert(editor->scene_draw_cb);

        // Construct application pointer (this should just register everything, honestly...)
        editor->app.app = editor->app.new(gs_instance(), editor->core);

        // Initialize application (which SHOULD register meta information correctly)
        editor->app.init(editor->app.app); 
    }
}

GS_API_DECL void 
gs_editor_library_unload()
{ 
    gs_editor_t* editor = gs_user_data(gs_editor_t);

    if (editor->app.dll) 
    {
        // Shutdown application and free memory
        editor->app.shutdown(editor->app.app);
        gs_println("App Unloaded!");
        gs_platform_library_unload(editor->app.dll);
        editor->app.dll = NULL;
        editor->app.app = NULL;
        editor->app.new = NULL;
        editor->app.init = NULL;
        editor->app.update = NULL;
        editor->app.shutdown = NULL;
        editor->scene_draw_cb = NULL;
    }
} 

GS_API_DECL void 
gs_editor_app_hot_reload()
{
    gs_editor_t* editor = gs_user_data(gs_editor_t); 

    // Remove previous .dll
    if (gs_platform_file_exists(gs_editor_dll_path()))
    {
        gs_editor_library_unload();
        gs_println("Deleting old dll...");
        gs_assert(gs_platform_file_delete(gs_editor_dll_path())); 
    } 

    // Copy over new
    if (gs_platform_file_copy(gs_app_dll_path(), gs_editor_dll_path()))
    {
        gs_println("Copied new dll!");
        gs_editor_library_load();
    }
    editor->app.hot_reload_begin = false; 
    editor->app.hot_reload_timer = 0; 
}










