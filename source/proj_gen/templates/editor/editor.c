
// Core Includes
#include <editor\gs_editor.h>

GS_API_DECL void 
%APP%_editor_init(void* app)
{
    // Super
    gs_editor_init(app); 
}

GS_API_DECL void
%APP%_editor_update(void* app)
{
    // Super
    gs_editor_update(app);

    // App update
    gs_editor_t* editor = (gs_editor_t*)app;
    if (editor->app.app)
    { 
        _gs_core_app_update(editor->app.app);
    }
}

GS_API_DECL void
%APP%_editor_shutdown(void* app)
{
    // Super
    gs_editor_shutdown(app);
}

GS_API_DECL int32_t
main(int32_t argc, char** argv)
{
    // Create gs instance
    gs_t* inst = gs_create((gs_app_desc_t) {
        .user_data = gs_core_os_malloc_init(sizeof(gs_editor_t)),
        .window_width = 800,
        .window_height = 600,
        .window_title = gs_to_str(%APP%_editor),
        .init = %APP%_editor_init,
        .update = %APP%_editor_update,
        .shutdown = %APP%_editor_shutdown
    });

    // Main loop
    while (gs_app()->is_running) {
        gs_frame();
    }

    // Shutdown
    gs_free(inst);

    return 0;
}

GS_API_DECL const char* 
gs_app_dll_path()
{ 
    #if GS_DEBUG
        const char* path = gs_platform_dir_exists(gs_to_str(bin/%APP%)) ? gs_to_str(bin/%APP%/%APP%_d.dll) : gs_to_str(../%APP%/%APP%_d.dll);
    #else
        const char* path = gs_platform_dir_exists(gs_to_str(bin/%APP%)) ? gs_to_str(bin/%APP%/%APP%.dll) : gs_to_str(../%APP%/%APP%.dll);
    #endif
            
    return path;
}

GS_API_DECL const char*
gs_editor_dll_path()
{
    #if GS_DEBUG
        const char* path = gs_platform_dir_exists(gs_to_str(bin/editor)) ? gs_to_str(bin/editor/%APP%_d.dll) : gs_to_str(%APP%_d.dll);
    #else
        const char* path = gs_platform_dir_exists(gs_to_str(bin/editor)) ? gs_to_str(bin/editor/%APP%.dll) : gs_to_str(%APP%.dll);
    #endif
            
    return path;
} 
