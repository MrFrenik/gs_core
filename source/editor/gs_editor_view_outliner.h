
#ifndef GS_EDITOR_OUTLINER_H
#define GS_EDITOR_OUTLINER_H 

// Editor Includes
#include "editor\gs_editor.h" 

GS_API_DECL void 
gs_editor_view_outliner_cb(struct gs_editor_view_s* view);

#ifdef GS_EDITOR_IMPL

GS_API_DECL void 
gs_editor_view_outliner_cb(struct gs_editor_view_s* view)
{
    gs_editor_t* editor = gs_user_data(gs_editor_t); 
    gs_gui_context_t* gui = &editor->gui;
    gs_gui_label(gui, "OUTLINER");
}

#endif

#endif
