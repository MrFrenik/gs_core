/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_editor_view_assets.h

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

#ifndef GS_CORE_EDITOR_VIEW_ASSETS_H
#define GS_CORE_EDITOR_VIEW_ASSETS_H 

// Editor Includes
#include "editor/gs_core_editor.h"

#define GS_CORE_EDITOR_VIEW_ASSETS_NAME  "Assets##gs_core_editor_view"

_introspect()
typedef struct
{
    gs_core_base(gs_core_editor_view_t);

    _vtable( 
        _override: post_init = _default;
        _override: callback = _default;
    )

} gs_core_editor_view_assets_t; 

#ifdef GS_CORE_EDITOR_IMPL

GS_API_DECL void 
gs_core_editor_view_assets_t_post_init(gs_core_obj_t* _obj)
{ 
    gs_core_editor_view_assets_t* view = gs_core_cast(_obj, gs_core_editor_view_assets_t);
    gs_core_editor_view_set_name(view, GS_CORE_EDITOR_VIEW_ASSETS_NAME);
}

GS_API_DECL void 
gs_core_editor_view_assets_t_callback(struct gs_core_editor_view_s* view)
{
    gs_core_editor_t* editor = gs_user_data(gs_core_editor_t); 
    gs_gui_context_t* gui = &editor->gui; 

    // Iterate through all available assets in virtual directory
}

#endif // GS_CORE_EDITOR_IMPL
#endif // GS_CORE_EDITOR_VIEW_ASSETS_H
