// Includes
#include "app.h"

// Reflection Includes
#include "generated/generated.h"

GS_API_DECL void 
%APP%_init()
{
    %APP%_t* app = gs_user_data(%APP%_t);

    // Init core
    app->core = core_new();
}

GS_API_DECL void 
%APP%_update()
{ 
    // Grab application and all required gs structures
    %APP%_t* app = gs_user_data(%APP%_t); 
    gs_command_buffer_t* cb = &app->core->cb;
    gs_immediate_draw_t* gsi = &app->core->gsi;
    gs_gui_context_t* gui = &app->core->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 

    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) 
    {
        gs_quit();
    } 

    // Gui
    gs_gui_begin(gui, fbs);
    {
        // Any game gui can go in between begin/end calls...
        gs_gui_window_begin(gui, "dbg", gs_gui_rect(100, 100, 200, 200));
        {
            gs_gui_layout_row(gui, 1, (int[]){150}, 0);
            gs_gui_label(gui, "Hello, Gunsglinger.");
        } 
        gs_gui_window_end(gui);
    } 
    gs_gui_end(gui);

    // Submit gsi
    gsi_renderpass_submit(gsi, cb, (uint32_t)fbs.x, (uint32_t)fbs.y, gs_color(10, 10, 10, 255)); 

    // Submit gui
    gs_gui_renderpass_submit_ex(gui, cb, NULL);

    // Submit command buffer for GPU
    gs_graphics_command_buffer_submit(cb);
}

GS_API_DECL void 
%APP%_shutdown()
{
    %APP%_t* app = gs_user_data(%APP%_t);

    // Shutdown core
    core_free(app->core);
} 

gs_app_desc_t 
gs_main(int32_t argc, char** argv)
{
    gs_app_desc_t desc = gs_default_val();
    desc.user_data = gs_malloc_init(%APP%_t);
    desc.window_width = 800;
    desc.window_height = 600;
    desc.init = %APP%_init;
    desc.update = %APP%_update;
    desc.shutdown = %APP%_shutdown;
    return desc;
}