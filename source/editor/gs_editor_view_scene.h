
#ifndef GS_EDITOR_SCENE_H
#define GS_EDITOR_SCENE_H 

// Editor Includes
#include "editor\gs_editor.h" 

GS_API_DECL void 
gs_editor_scene_cb(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd);

GS_API_DECL void 
gs_editor_view_scene_cb(struct gs_editor_view_s* view);

#ifdef GS_EDITOR_IMPL

GS_API_DECL void 
gs_editor_view_scene_cb(struct gs_editor_view_s* view)
{
    gs_editor_t* editor = gs_user_data(gs_editor_t); 
    gs_gui_context_t* gui = &editor->core->gui;

    gs_gui_layout_t* layout = gs_gui_get_layout(gui);
    gs_gui_layout_row(gui, 1, (int[]){-1}, -1); 
    gs_gui_rect_t rect = gs_gui_layout_next(gui);
    gs_gui_layout_set_next(gui, rect, 0);
    gs_gui_container_t* cnt = gs_gui_get_current_container(gui); 
    gs_gui_draw_custom(gui, rect, gs_editor_scene_cb, NULL, 0);

}

GS_API_DECL void 
gs_editor_scene_cb(gs_gui_context_t* ctx, gs_gui_customcommand_t* cmd)
{
    gs_editor_t* editor = gs_user_data(gs_editor_t); 
    gs_immediate_draw_t* gsi = &ctx->gsi;
    const float t = gs_platform_elapsed_time(); 
	const gs_gui_rect_t vp = cmd->viewport;
	const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());

    // Draw room
	gsi_defaults(gsi);
    gsi_camera2D(gsi, (uint32_t)vp.w, (uint32_t)vp.h); 
    gsi_rectvd(gsi, gs_v2s(100.f), gs_v2s(100.f), gs_v2s(0.f), gs_v2s(1.f), 
        GS_COLOR_RED, GS_GRAPHICS_PRIMITIVE_TRIANGLES);
}

#endif

#endif
