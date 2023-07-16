// Includes
#include %APP_HEADER_PATH%

// Reflection Includes
#include %GENERATED_HEADER_PATH%

GS_CORE_APP_MAIN(%APP%) 
GS_CORE_APP_DEFINE(%APP%)

GS_API_DECL void 
%APP%_init()
{
    // Get instance
    %APP%_t* app = (%APP%_t*)gs_core_app_instance(); 

    // Get core instance
    gs_core_t* core = gs_core_instance();

    // Default mesh
    gs_core_asset_handle_t msh_hndl = gs_core_assets_get_default(gs_core_asset_mesh_t);
    
    // Import texture from resource
    app->tex_hndl = gs_core_assets_import(&(gs_core_asset_import_options_t){
        .import_path = "textures/gs.png"
    }); 

    // Import pipeline from resource 
    app->pip_hndl = gs_core_assets_import(&(gs_core_asset_import_options_t){
        .import_path = "pipelines/simple.sf"
    }); 

    // Create and store new material 
    gs_core_asset_material_t* mat = gs_core_cls_new(gs_core_asset_material_t);
    mat->resource = gs_gfxt_material_create(&(gs_gfxt_material_desc_t) {
        .pip_func.hndl = &((gs_core_asset_pipeline_t*)gs_core_asset_handle_get(&app->pip_hndl))->resource
    }); 
    gs_gfxt_material_set_uniform(&mat->resource, "u_tex", &((gs_core_asset_texture_t*)gs_core_asset_handle_get(&app->tex_hndl))->resource); 
    app->mat_hndl = gs_core_assets_add(mat, &(gs_core_asset_import_options_t){0});

    // Allocate new entity and attach components
    app->ent = gs_core_entities_allocate("player");

    // Transform component
    gs_core_entities_component_add(gs_core_entities_world(), app->ent, gs_core_component_transform_t, { 
        .transform = (gs_vqs) { 
			.translation = gs_v3(0.f, 0.f, 0.f),
			.scale = gs_vec3_scale(gs_v3(1.f, 0.3f, 1.f), 2.f),
			.rotation = gs_quat_angle_axis(gs_deg2rad(180.f), GS_XAXIS)
        }
    });
    gs_core_component_transform_t* tc = gs_core_entities_component_get(gs_core_entities_world(), app->ent, gs_core_component_transform_t);

    // Renderable component
    gs_core_entities_component_add(gs_core_entities_world(), app->ent, gs_core_component_renderable_t, {
        .hndl = gs_core_graphics_scene_renderable_create(&core->gfx->scene, &(gs_core_graphics_renderable_t){ 
            .materials[0] = app->mat_hndl,
            .mesh = msh_hndl,
            .model = gs_vqs_to_mat4(&tc->transform)
        }) 
    });
}

GS_API_DECL void 
%APP%_update()
{ 
    // Grab application and all required gs structures
    %APP%_t* app = (%APP%_t*)gs_core_app_instance(); 
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
    gs_gui_begin(gui, &(gs_gui_hints_t){
        .framebuffer_size = fbs, 
        .viewport = gs_gui_rect(vp.x, vp.y, vp.z, vp.w), 
        .flags = GS_GUI_HINT_FLAG_NO_SCALE_BIAS_MOUSE | GS_GUI_HINT_FLAG_NO_INVERT_Y}
    );
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
%APP%_render(gs_command_buffer_t* cb)
{ 
    %APP%_t* app = (%APP%_t*)gs_core_app_instance(); 
    gs_core_t* core = gs_core_instance();
    gs_immediate_draw_t* gsi = &core->gsi;
    gs_gui_context_t* gui = &core->gui;
    gs_core_graphics_t* gfx = core->gfx;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window()); 
    const gs_vec4 vp = gs_core_cast(app, gs_core_app_t)->viewport; 

    // Render scene with basic pass
    gs_graphics_clear_desc_t clear = {.actions = &(gs_graphics_clear_action_t){.color = {0.05f, 0.05, 0.05, 1.f}}};
    gs_graphics_renderpass_begin(cb, (gs_handle(gs_graphics_renderpass_t)){0});
    {
        // Camera for scene
        gs_camera_t cam = gs_camera_perspective();
        cam.transform.position = gs_v3(0.f, 0.f, 5.f);
        const float _t = gs_platform_elapsed_time() * 0.0001f;
        gs_mat4 view_proj = gs_camera_get_view_projection(&cam, vp.z, vp.w);

        // Set view port
        gs_graphics_set_viewport(cb, vp.x, vp.y, vp.z, vp.w); 

        // Clear screen
        gs_graphics_clear(cb, &clear);

        // Iterate through all scene renderables in graphics context (no frustum culling yet)
        for (
            gs_slot_array_iter it = gs_slot_array_iter_new(gfx->scene.renderables);
            gs_slot_array_iter_valid(gfx->scene.renderables, it);
            gs_slot_array_iter_advance(gfx->scene.renderables, it)
        )
        {
            // Get renderable from scene
            gs_core_graphics_renderable_t* renderable = gs_slot_array_iter_getp(gfx->scene.renderables, it);

            // Calculate MVP matrix for renderable
            gs_mat4 mvp = gs_mat4_mul(view_proj, renderable->model);

            // Get material from renderable 
            gs_core_asset_material_t* mat = gs_core_asset_handle_get(&renderable->materials[0]);
            gs_core_asset_mesh_t* mesh = gs_core_asset_handle_get(&renderable->mesh);

            // Set default material uniforms for this pass
            gs_gfxt_material_set_uniform(&mat->resource, GS_GFXT_UNIFORM_MODEL_VIEW_PROJECTION_MATRIX, &mvp);

            // Bind material pipeline and uniforms
            gs_gfxt_material_bind(cb, &mat->resource); 

            // Draw mesh
            gs_gfxt_mesh_draw_material(cb, &mesh->resource, &mat->resource);
        } 
    }
    gs_graphics_renderpass_end(cb);

    // Submit gsi
    gsi_renderpass_submit_ex(gsi, cb, gs_v4(vp.x, fbs.y - vp.w - vp.y, vp.z, vp.w), NULL); 

    // Submit gui
    gs_gui_renderpass_submit_ex(gui, cb, NULL);
}

GS_API_DECL void 
%APP%_shutdown()
{
    %APP%_t* app = (%APP%_t*)gs_core_app_instance(); 

    // Free entity
    gs_core_entities_deallocate(gs_core_entities_world(), app->ent); 

    // Free assets 
    gs_core_asset_handle_free(&app->mat_hndl); 
    gs_core_asset_handle_free(&app->tex_hndl);
    gs_core_asset_handle_free(&app->pip_hndl);
} 
