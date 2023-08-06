/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_editor_view_outliner.h

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2022 John Jackson

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=================================================================================================================*/

#ifndef GS_CORE_EDITOR_VIEW_OUTLINER_H
#define GS_CORE_EDITOR_VIEW_OUTLINER_H 

// Editor Includes
#include "../gs_core_editor.h" 

#define GS_CORE_EDITOR_VIEW_OUTLINER_NAME "Outliner##gs_core_editor_view"

_introspect()
typedef struct
{
    gs_core_base(gs_core_editor_view_t);

    _vtable( 
        _override: post_init = _default;
        _override: callback = _default; 
    )
    
    // Query for all tag components in scene
    gs_core_entity_query_t* component_tag_query;

} gs_core_editor_view_outliner_t; 

#ifdef GS_CORE_EDITOR_IMPL

GS_API_DECL void 
gs_core_editor_view_outliner_t_post_init(struct gs_core_obj_t* _obj)
{
    gs_core_editor_view_outliner_t* view = gs_core_cast(_obj, gs_core_editor_view_outliner_t); 
    gs_core_editor_view_set_name(view, GS_CORE_EDITOR_VIEW_OUTLINER_NAME);
}

GS_API_DECL void 
gs_core_editor_view_outliner_t_callback(struct gs_core_editor_view_s* view)
{
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t); 
    gs_gui_context_t* gui = &editor->gui; 
    gs_core_editor_view_outliner_t* outliner = gs_core_cast(view, gs_core_editor_view_outliner_t);
    gs_core_entity_world_t* eworld = gs_core_entities_world();

    // Initialize query if necessary
    if (!outliner->component_tag_query)
    {
        outliner->component_tag_query = gs_core_entities_query_init(eworld, &(gs_core_entity_query_desc_t){.filter.terms = {{.id = 
            gs_core_entities_component_id(gs_core_component_tag_t)}}});
    } 

    // Iterate through all entities within entity manager, show their...names?
    // Show their ids, I suppose. This is where it gets kind of odd in a 'generic' sense. 

    gs_core_entity_query_t* q = outliner->component_tag_query;     
    gs_core_entity_iter_t it = gs_core_entities_query_iter(eworld, q);
    while (gs_core_entities_query_next(&it))
    {
        for (uint32_t i = 0; i < it.count; ++i)
        {
            const char* name = gs_core_entities_get_name(eworld, it.entities[i]);
            gs_snprintfc(ID_LABEL, 32, "%d", (uint32_t)it.entities[i]);
            gs_gui_label(gui, "%s", ID_LABEL);
        }
    }
} 

#endif // GS_CORE_EDITOR_IMPL
#endif // GS_CORE_EDITOR_VIEW_OUTLINER_H
