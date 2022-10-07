// Includes
#include %APP_HEADER_PATH%

// Reflection Includes
#include %GENERATED_HEADER_PATH%

GS_CORE_APP_MAIN(%APP%) 
GS_CORE_APP_DEFINE(%APP%)

GS_API_DECL void 
%APP%_init(void* app)
{
    // Get instance
    %APP%_t* %APP% = (%APP%_t*)app;

    // Get core instance
    gs_core_t* core = gs_core_instance();
}

GS_API_DECL void 
%APP%_update(void* app)
{ 
    // Grab application and all required gs structures
    %APP%_t* %APP% = (%APP%_t*)app; 
    gs_core_t* core = gs_core_instance();
    gs_command_buffer_t* cb = &core->cb;
    gs_immediate_draw_t* gsi = &core->gsi;
    gs_gui_context_t* gui = &core->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 
    const gs_vec4 vp = gs_core_cast(app, gs_core_app_t)->viewport;

    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) 
    {
        gs_quit();
    } 

    // Gui
    gs_gui_begin(gui, &(gs_gui_hints_t){.framebuffer_size = fbs, .viewport = gs_gui_rect(vp.x, vp.y, vp.z, vp.w)});
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

}

GS_API_DECL void
%APP%_render(void* app, gs_command_buffer_t* cb)
{ 
    gs_core_t* core = gs_core_instance();
    gs_immediate_draw_t* gsi = &core->gsi;
    gs_gui_context_t* gui = &core->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 
    const gs_vec4 vp = gs_core_cast(app, gs_core_app_t)->viewport; 

    // Submit gsi
    gsi_renderpass_submit(gsi, cb, (uint32_t)fbs.x, (uint32_t)fbs.y, gs_color(10, 10, 10, 255)); 

    // Submit gui
    gs_gui_renderpass_submit_ex(gui, cb, NULL);
}

GS_API_DECL void 
%APP%_shutdown(void* app)
{
    %APP%_t* %APP% = (%APP%_t*)app; 
} 

