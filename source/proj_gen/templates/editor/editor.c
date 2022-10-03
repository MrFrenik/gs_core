
// Core Includes
#include <editor\gs_editor.h>

GS_API_DECL void 
%APP%_editor_init()
{
    gs_editor_init();

    // Initialize any custom views here...
}

GS_API_DECL void
%APP%_editor_update()
{
    gs_editor_update();
}

GS_API_DECL void
%APP%_editor_shutdown()
{
    gs_editor_shutdown();
}

GS_API_DECL int32_t
main(int32_t argc, char** argv)
{
    // Create gs instance
    gs_t* inst = gs_create((gs_app_desc_t) {
        .user_data = gs_malloc_init(gs_editor_t),
        .window_width = 800,
        .window_height = 600,
        .window_title = gs_to_str(%APP%_editor_t),
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
