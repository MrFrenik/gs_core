
// Core Includes
#include <editor/gs_core_editor.h>

GS_CORE_EDITOR_DEFINE(%APP%)

GS_API_DECL void 
%APP%_editor_init(void* app)
{
    // Super
    gs_core_editor_init(app); 
}

GS_API_DECL void
%APP%_editor_update(void* app)
{
    // Super
    gs_core_editor_update(app);
}

GS_API_DECL void
%APP%_editor_shutdown(void* app)
{
    // Super
    gs_core_editor_shutdown(app);
}

GS_API_DECL int32_t
main(int32_t argc, char** argv)
{
    // Create gs instance
    gs_t* inst = gs_create((gs_app_desc_t) {
        .user_data = gs_core_os_malloc_init(sizeof(gs_core_editor_t)),
        .window.width = 800,
        .window.height = 600,
        .window.title = gs_to_str(%APP%_editor),
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
            const char* path = gs_platform_dir_exists(gs_to_str(bin/%APP%)) ? 
                gs_to_str(bin/%APP%/%APP%_d) : 
                gs_to_str(../%APP%/%APP%_d);
    #else
            const char* path = gs_platform_dir_exists(gs_to_str(bin/%APP%)) ? 
                gs_to_str(bin/%APP%/%APP%) : 
                gs_to_str(../%APP%/%APP%);
    #endif
            
    return path;
}

GS_API_DECL const char*
gs_core_editor_dll_path()
{
    #if GS_DEBUG
        const char* path = gs_platform_dir_exists(gs_to_str(bin/editor)) ? 
            gs_to_str(bin/editor/%APP%_d) : 
            gs_to_str(%APP%_d);
    #else
        const char* path = gs_platform_dir_exists(gs_to_str(bin/editor)) ? 
            gs_to_str(bin/editor/%APP%) : 
            gs_to_str(%APP%);
    #endif
            
    return path;
} 
