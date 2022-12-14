/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * Project Solomon
    * File: sm_config.cpp
    * Github: https://github.com/MrFrenik/project_solomon

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

// GS Includes 
#define GS_NO_HIJACK_MAIN
#define GS_IMPL
#define GS_IMMEDIATE_DRAW_IMPL
#define GS_GUI_IMPL
#define GS_AI_IMPL
#define GS_GFXT_IMPL
#define GS_META_IMPL 
#define GS_NO_OS_MEMORY_ALLOC_DEFAULT

// Core defines
#define GS_CORE_MEMORY_DBG

// Memory management defines 
#ifdef __cplusplus
    #define GS_CORE_API_EXTERN extern "C"
#else
    #define GS_CORE_API_EXTERN extern
#endif

struct gs_os_api_s;

GS_CORE_API_EXTERN struct gs_os_api_s
gs_core_os_api_new(); 

#define gs_os_api_new gs_core_os_api_new

#ifdef GS_CORE_MEMORY_DBG

    GS_CORE_API_EXTERN void* 
    gs_core_malloc_dbg(void* ptr, size_t sz, const char* file, int line);

    GS_CORE_API_EXTERN void 
    gs_core_free_dbg(void* ptr, const char* file, int line);

    #define gs_malloc(__SZ) gs_core_malloc_dbg(gs_ctx()->os.malloc(__SZ), __SZ, __FILE__, __LINE__) 
    #define gs_free(__PTR)  gs_core_free_dbg(__PTR, __FILE__, __LINE__) 

#endif 

// GS
#include "core/gs_core_gs.h" 
#include "core/gs_core.h" 

#ifdef GS_CORE_MEMORY_DBG

#define GS_CORE_MEMORY_TOTAL_ALLOCS 4096
static gs_core_memory_alloc_t g_allocations[GS_CORE_MEMORY_TOTAL_ALLOCS] = {0}; 
static uint32_t g_allocation_max_idx = 0;
static size_t g_total_allocated = 0;
static size_t g_total_freed = 0;

GS_API_DECL void
gs_core_memory_print()
{ 
    for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    { 
        if (g_allocations[i].ptr)
        { 
            gs_println("%x, %s, %zu", g_allocations[i].ptr, g_allocations[i].file, g_allocations[i].line);
        }
    }
} 

GS_API_DECL void
gs_core_memory_print_to_file(const char* path)
{ 
    FILE* fp = NULL; 
    fp = fopen(path, "w"); 

    gs_fprintln(fp, "ta: %zu, tf: %zu", g_total_allocated, g_total_freed);

    for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    { 
        if (g_allocations[i].ptr)
        { 
            gs_core_memory_alloc_t* mem = &g_allocations[i];
            gs_fprintln(fp, "%x, %s, %d", mem->ptr, mem->file, mem->line);
        }
    }
}

GS_CORE_API_EXTERN void* 
gs_core_malloc_dbg(void* ptr, size_t sz, const char* file, int32_t line)
{
    // Add allocation
    g_total_allocated += sz;
    gs_core_memory_alloc_t alloc = {.ptr = ptr, .file = file, .line = line, .sz = sz};

    for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    {
        if (g_allocations[i].ptr == ptr) {
            g_allocations[i] = alloc;
            goto done;
        }
    }

    for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    {
        if (!g_allocations[i].ptr) {
            g_allocations[i] = alloc;
            g_allocation_max_idx = gs_max(g_allocation_max_idx, i);
            goto done;
        }
    }

done:
    return ptr;
}

GS_CORE_API_EXTERN void 
gs_core_free_dbg(void* ptr, const char* file, int line)
{ 
    // Search for allocation and remove
    for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    {
        if (g_allocations[i].ptr == ptr) { 
            g_total_freed += g_allocations[i].sz;
            g_allocations[i].ptr = NULL;
            g_allocations[i].file = NULL;
            g_allocations[i].line = 0;
            break;
        }
    }

    gs_ctx()->os.free(ptr);
} 
#endif

GS_API_DECL void* 
gs_core_os_malloc(size_t sz)
{
    return malloc(sz);
}

GS_API_DECL void* 
gs_core_os_malloc_init(size_t sz)
{
    void* data = malloc(sz);
    memset(data, 0, sz);
    return data;
}

GS_API_DECL void 
gs_core_os_free(void* ptr)
{ 
    free(ptr);
}

GS_API_DECL void* 
gs_core_os_realloc(void* ptr, size_t sz)
{ 
    return realloc(ptr, sz);
} 

GS_API_DECL void* 
gs_core_os_calloc(size_t num, size_t sz)
{ 
    return calloc(num, sz);
} 

GS_API_DECL void* 
gs_core_os_alloca(size_t sz)
{
    #ifdef GS_PLATFORM_WIN
        return malloc(sz);
    #else
        return malloc(sz);
    #endif
} 

GS_API_DECL char* 
gs_core_os_strdup(const char* str)
{
    return strdup(str);
}

GS_API_DECL struct gs_os_api_s
gs_core_os_api_new()
{
    struct gs_os_api_s os = gs_default_val();
    os.malloc = gs_core_os_malloc;
    os.malloc_init = gs_core_os_malloc_init;
    os.free = gs_core_os_free;
    os.realloc = gs_core_os_realloc;
    os.calloc = gs_core_os_calloc;
    os.alloca = gs_core_os_alloca;
    os.strdup = gs_core_os_strdup;
    return os;
} 

// Flecs
#define ecs_os_malloc   gs_malloc
#define ecs_os_free     gs_free
#define ecs_os_realloc  gs_realloc
#define ecs_os_calloc   gs_calloc
#define ecs_os_alloca   gs_alloca 
#define ecs_os_strdup   gs_strdup

#include "flecs/flecs.h"


