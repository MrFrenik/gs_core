// Includes
#include %APP_HEADER_PATH%

// Reflection Includes
#include %GENERATED_HEADER_PATH%

GS_API_DECL void 
%APP%_init()
{
    %APP%_t* app = gs_user_data(%APP%_t);

    // Init core
    app->core = core_new();

    // Register application meta information
    GS_CORE_APP_META_REGISTER(%APP%);
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

#ifdef GS_APP_STANDALONE

GS_API_DECL int32_t 
main(int32_t argc, char** argv)
{
    // Create gunslinger instance with application
    gs_t* inst = gs_create((gs_app_desc_t){
        .user_data = gs_malloc_init(%APP%_t),
        .window_width = 800,
        .window_height = 600,
        .window_title = gs_to_str(%APP%),
        .init = %APP%_init,
        .update = %APP%_update,
        .shutdown = %APP%_shutdown
    });

    // Main loop
    while (gs_app()->is_running) {
        gs_frame();
    }

    // Shutdown
    gs_free(inst);

    return 0;
}

#endif
