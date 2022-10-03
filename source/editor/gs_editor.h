#ifndef GS_EDITOR_H
#define GS_EDITOR_H

#include "core/gs_core.h"

#define GS_EDITOR_VIEW_STR_MAX    64

struct gs_editor_view_s;

typedef void (* gs_editor_view_cb)(struct gs_editor_view_s* view);

// Editor view base
typedef struct gs_editor_view_s
{
    char name[GS_EDITOR_VIEW_STR_MAX];  // Name for editor view
    gs_editor_view_cb cb;               // Callback for view
} gs_editor_view_t;

typedef struct gs_editor_view_desc_s
{
    const char* name;
    gs_editor_view_cb cb;
    struct {
        const char* parent;
        gs_gui_split_type type;
        float amount;
    } split;

} gs_editor_view_desc_t;

typedef struct gs_editor_s
{
    gs_core_t* core;                                       // Core 
    gs_hash_table(uint64_t, gs_editor_view_t) views;    // All registered editor views 
    struct {
        gs_platform_file_stats_t fstats;    // To track dll hot reloading
        bool32 hot_reload_begin;            // Change detected for hot-reload             
        void* dll;                          // Pointer to library
        void (* app_meta_register)(void* core);
        void (* app_meta_unregister)(void* core);
        uint32_t hot_reload_timer;
    } app;                                  // Struct for loaded application
} gs_editor_t;

GS_API_DECL void
gs_editor_view_register(const gs_editor_view_desc_t* view);

GS_API_DECL void 
gs_editor_init();

GS_API_DECL void
gs_editor_update();

GS_API_DECL void 
gs_editor_shutdown();

GS_API_DECL const char* 
gs_app_dll_path();

GS_API_DECL const char*
gs_editor_dll_path();

#endif // GS_EDITOR_H
