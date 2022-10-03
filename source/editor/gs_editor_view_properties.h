
#ifndef GS_EDITOR_PROPERTIES_H
#define GS_EDITOR_PROPERTIES_H

// Editor Includes
#include "editor\gs_editor.h" 

GS_API_DECL void 
gs_editor_view_properties_cb(struct gs_editor_view_s* view);

#ifdef GS_EDITOR_IMPL

GS_API_DECL void 
gs_editor_view_properties_cb(struct gs_editor_view_s* view)
{
    gs_editor_t* editor = gs_user_data(gs_editor_t); 
    gs_core_meta_registry_t* meta = gs_core_meta_registry_instance();
    gs_gui_context_t* gui = &editor->core->gui;
    gs_gui_container_t* cnt = gs_gui_get_current_container(gui);
    gs_gui_label(gui, "PROPERTIES");

    gs_gui_layout_row(gui, 1, (int16_t[]){cnt->body.w * 0.8f}, 0);

    // Get all registered components from meta
    for (
        gs_slot_array_iter it = gs_slot_array_iter_new(meta->info);
        gs_slot_array_iter_valid(meta->info, it);
        gs_slot_array_iter_advance(meta->info, it)
    )
    {
        gs_core_meta_info_t* info = gs_slot_array_iter_getp(meta->info, it);
        if (gs_core_info_base_of(info, gs_core_entities_component_t))
        {
            gs_gui_label(gui, "%s", info->cls->name);
        }
    }
}

#endif


#endif
