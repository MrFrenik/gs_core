
// Core Includes
#include <editor/gs_core_editor.h>

GS_CORE_EDITOR_DEFINE(%APP%)

GS_API_DECL void 
%APP%_editor_init(void* app)
{
    // Super
    gs_core_editor_init(app); 
}

GS_API_DECL void
%APP%_editor_update(void* app)
{
    // Super
    gs_core_editor_update(app);
}

GS_API_DECL void
%APP%_editor_shutdown(void* app)
{
    // Super
    gs_core_editor_shutdown(app);
}

GS_API_DECL int32_t
main(int32_t argc, char** argv)
{
    // Create gs instance
    gs_t* inst = gs_create((gs_app_desc_t) {
        .user_data = gs_core_os_malloc_init(sizeof(gs_core_editor_t)),
        .window.width = 800,
        .window.height = 600,
        .window.title = gs_to_str(%APP%_editor),
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

GS_API_DECL const char* 
gs_app_dll_path()
{ 
    static char buf[512] = {0};

#ifdef _WIN32
    char raw[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, raw, sizeof(raw));
    if (n == 0 || n >= sizeof(raw)) { buf[0] = '\0'; return buf; }

    char resolved[MAX_PATH];
    if (GetFullPathNameA(raw, sizeof(resolved), resolved, NULL) == 0) { buf[0] = '\0'; return buf; }

    size_t len = strlen(resolved);
    while (len > 0 && resolved[len - 1] != '\\') --len;
    if (len == 0) { buf[0] = '\0'; return buf; }
    resolved[len] = '\0';

#if GS_DEBUG
    snprintf(buf, sizeof(buf), "%sbin/%s/%s_d.dll", resolved, "%APP%", "%APP%");
#else
    snprintf(buf, sizeof(buf), "%sbin/%s/%s.dll", resolved, "%APP%", "%APP%");
#endif

    /* If the primary path does not exist, fall back to just the exe dir */
    if (!gs_platform_dir_exists(gs_to_str(buf)))
    {
#if GS_DEBUG
        snprintf(buf, sizeof(buf), "%s/%s_d.dll", resolved, "%APP%");
#else
        snprintf(buf, sizeof(buf), "%s/%s.dll", resolved, "%APP%");
#endif
    }
#endif

    return buf;
}

GS_API_DECL const char*
gs_core_editor_dll_path()
{
    static char buf[512] = {0};

#ifdef _WIN32
    char raw[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, raw, sizeof(raw));
    if (n == 0 || n >= sizeof(raw)) { buf[0] = '\0'; return buf; }

    char resolved[MAX_PATH];
    if (GetFullPathNameA(raw, sizeof(resolved), resolved, NULL) == 0) { buf[0] = '\0'; return buf; }

    size_t len = strlen(resolved);
    while (len > 0 && resolved[len - 1] != '\\') --len;
    if (len == 0) { buf[0] = '\0'; return buf; }
    resolved[len] = '\0';

#if GS_DEBUG
    snprintf(buf, sizeof(buf), "%s/bin/editor/%s_d.dll", resolved, "%APP%");
#else
    snprintf(buf, sizeof(buf), "%s/bin/editor/%s.dll", resolved, "%APP%");
#endif

    /* If the primary path does not exist, fall back to just the exe dir */
    if (!gs_platform_dir_exists(gs_to_str(buf)))
    {
#if GS_DEBUG
        snprintf(buf, sizeof(buf), "%s/%s_d.dll", resolved, "%APP%");
#else
        snprintf(buf, sizeof(buf), "%s/%s.dll", resolved, "%APP%");
#endif
    }
#endif

    return buf;
} 
