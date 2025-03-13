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
#define GS_DDT_IMPL
#define STB_DS_IMPLEMENTATION

// Core defines
#define GS_CORE_MEMORY_DBG 0

// Memory management defines 
#ifdef __cplusplus
    #define GS_CORE_API_EXTERN extern "C"
#else
    #define GS_CORE_API_EXTERN extern
#endif

// DDT
struct gs_os_api_s;

GS_CORE_API_EXTERN struct gs_os_api_s
gs_core_os_api_new(); 

#define gs_os_api_new gs_core_os_api_new

#if GS_CORE_MEMORY_DBG

    GS_CORE_API_EXTERN void* 
    gs_core_malloc_dbg(size_t sz, const char* file, int line);

    GS_CORE_API_EXTERN void*
    gs_core_realloc_dbg(void* ptr, size_t sz, const char* file, int line);

    GS_CORE_API_EXTERN void*
    gs_core_calloc_dbg(int num, size_t sz, const char* file, int line);

    GS_CORE_API_EXTERN void 
    gs_core_free_dbg(void* ptr, const char* file, int line);

    GS_CORE_API_EXTERN char* 
    gs_core_strdup_dbg(const char* str, const char* file, int line);

    #define gs_malloc(__SZ)         gs_core_malloc_dbg(__SZ, __FILE__, __LINE__) 
    #define gs_realloc(__MEM, __AZ) gs_core_realloc_dbg((__MEM), (__AZ), __FILE__, __LINE__)
    #define gs_calloc(__NUM, __SZ)  gs_core_calloc_dbg((__NUM), (__SZ), __FILE__, __LINE__)
    #define gs_free(__PTR)          gs_core_free_dbg(__PTR, __FILE__, __LINE__) 
	#define gs_alloca(__SZ)         gs_core_malloc_dbg(__SZ, __FILE__, __LINE__)
	#define gs_strdup(__STR)        gs_core_strdup_dbg(__STR, __FILE__, __LINE__)

#endif 

// GS
#include "core/gs_core_gs.h"
#include "core/gs_core.h" 

#ifdef GS_CORE_MEMORY_DBG

static gs_core_memory_alloc_t g_allocations[GS_CORE_MEMORY_DBG_MAX_ALLOCATIONS] = {0}; 
static int32_t g_allocation_max_idx = 0;
static int32_t g_total_allocated = 0;
static int32_t g_total_freed = 0;
static gs_atomic_int_t g_mem_lock = 0;

#define GS_CORE_MEM_AQUIRE_LOCK(LOCK)   while (gs_atomic_cmp_swp(&(LOCK), 1, 0) != 0)
#define GS_CORE_MEM_RELEASE_LOCK(LOCK)  while (gs_atomic_cmp_swp(&(LOCK), 0, 1) != 1)

static void gs_core_memory_spin_lock(gs_atomic_int_t* lock)
{
    do {
        if (gs_atomic_cmp_swp(lock, SCHED_PIPE_CAN_READ, SCHED_PIPE_INVALID) == SCHED_PIPE_CAN_READ) { 
            break;
        } 
        // gs_platform_sleep(1);
    } while (1);
}

static void gs_core_memory_spin_unlock(gs_atomic_int_t* lock)
{
    // do {
    //     gs_println("unlock: %zu", g_mem_lock);
    // } while (!gs_atomic_cmp_swp(&g_mem_lock, 0, 1));

    do { 
        if (gs_atomic_cmp_swp(lock, SCHED_PIPE_INVALID, SCHED_PIPE_CAN_READ) == SCHED_PIPE_INVALID) { 
            break;
        } 
        // gs_platform_sleep(1);
    } while (1);
}

GS_API_DECL void
gs_core_memory_print()
{ 
    // for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    // { 
    //     if (g_allocations[i].ptr)
    //     { 
    //         gs_println("%x, %s, %zu", g_allocations[i].ptr, g_allocations[i].file, g_allocations[i].line);
    //     }
    // }
} 

GS_API_DECL void
gs_core_memory_print_to_file(const char* path)
{ 
    // FILE* fp = NULL; 
    // fp = fopen(path, "w"); 

    // gs_fprintln(fp, "ta: %zu, tf: %zu", g_total_allocated, g_total_freed);

    // for (uint32_t i = 0; i < GS_CORE_MEMORY_TOTAL_ALLOCS; ++i)
    // { 
    //     if (g_allocations[i].ptr)
    //     { 
    //         gs_core_memory_alloc_t* mem = &g_allocations[i];
    //         gs_fprintln(fp, "%x, %s, %d", mem->ptr, mem->file, mem->line);
    //     }
    // } 
}

GS_CORE_API_EXTERN gs_core_memory_alloc_t* 
gs_core_memory_find_by_address(void* ptr)
{
    for (uint32_t i = 0; i < GS_CORE_MEMORY_DBG_MAX_ALLOCATIONS; ++i) {
        if (g_allocations[i].ptr == ptr) {
            return &g_allocations[i];
        }
    }
    return NULL;
}

GS_CORE_API_EXTERN int32_t
gs_core_memory_insert(void* ptr, size_t sz, const char* file, const char* line)
{
    // Aquire lock
    // gs_core_memory_spin_lock(&gs_ctx()->lock);
    // GS_CORE_MEM_AQUIRE_LOCK(gs_ctx()->lock);
    GS_CORE_MEM_AQUIRE_LOCK(g_mem_lock);

    // gs_println("MEMORY LOCK INSERT: %s, %d", file, line);
    // int32_t desired = 1;
    // int32_t expected = g_mem_lock;
    // do {} while (!gs_atomic_cmp_swp(&g_mem_lock, 0, g_mem_lock));
    // do {
	// 	uint32_t c = 0;
	// } while (g_mem_lock);
    // gs_atomic_add(&g_mem_lock, 1);

    // Find empty address
    gs_core_memory_alloc_t* mem = gs_core_memory_find_by_address(0x00);
    if (!mem) {
        gs_log_error("Max allocations reached: %s, %d, %d", file, line, g_allocation_max_idx);
        return -1;
    }

    mem->ptr = ptr;
    mem->sz = sz;
    mem->file = file;
    mem->line = line;
    g_total_allocated += sz;
    g_allocation_max_idx++;

    // Release lock
    // g_mem_lock = 0;
    // gs_atomic_add(&g_mem_lock, -1);
    // gs_core_memory_spin_unlock(&gs_ctx()->lock);
    // GS_CORE_MEM_RELEASE_LOCK(gs_ctx()->lock);
    GS_CORE_MEM_RELEASE_LOCK(g_mem_lock);

    return 0; 
}

GS_CORE_API_EXTERN int32_t
gs_core_memory_erase(void* ptr, const char* file, int32_t line)
{
    // Aquire lock
    // gs_core_memory_spin_lock(&gs_ctx()->lock);
    // GS_CORE_MEM_AQUIRE_LOCK(gs_ctx()->lock);
    GS_CORE_MEM_AQUIRE_LOCK(g_mem_lock);

    // gs_println("MEMORY LOCK ERASE: %s, %d", file, line);
    // int32_t desired = 1;
    // int32_t expected = g_mem_lock;
    // do {} while (!gs_atomic_cmp_swp(&g_mem_lock, 0, g_mem_lock));
    // do {
	// 	uint32_t b = 0;
	// } while (g_mem_lock);
    // gs_atomic_add(&g_mem_lock, 1);

    gs_core_memory_alloc_t* mem = gs_core_memory_find_by_address(ptr);
    if (!mem) { 
        gs_log_error("Trying to erase invalid pointer: %s, %d", file, line);
        return -1;
    }

    g_allocation_max_idx--;
    g_total_freed += mem->sz;
    *mem = (gs_core_memory_alloc_t){0};

    // Release lock
    // gs_core_memory_spin_unlock(&gs_ctx()->lock);
    // GS_CORE_MEM_RELEASE_LOCK(gs_ctx()->lock);
    GS_CORE_MEM_RELEASE_LOCK(g_mem_lock);
    // g_mem_lock = 0;
    // gs_atomic_add(&g_mem_lock, -1);

    return 0;
}

GS_CORE_API_EXTERN void* 
gs_core_malloc_dbg(size_t sz, const char* file, int32_t line)
{
    void* ptr = gs_ctx()->os.malloc(sz);
    gs_core_memory_insert(ptr, sz, file, line);
    return ptr;
}

GS_CORE_API_EXTERN void*
gs_core_realloc_dbg(void* ptr, size_t sz, const char* file, int32_t line)
{ 
    // gs_log_error("%s, %d", file, line);

    if (ptr == 0) { 
		return gs_malloc(sz);
    }

    // Search for allocation and remove
    gs_core_memory_erase(ptr, file, line);

    // Realloc
    void* rptr = gs_ctx()->os.realloc(ptr, sz);
    gs_core_memory_insert(rptr, sz, file, line);
    return rptr;
}

GS_CORE_API_EXTERN void*
gs_core_calloc_dbg(int num, size_t sz, const char* file, int line)
{
    void* ptr = gs_ctx()->os.calloc(num, sz);
    gs_core_memory_insert(ptr, sz, file, line);
    return ptr;
}

GS_CORE_API_EXTERN void 
gs_core_free_dbg(void* ptr, const char* file, int line)
{ 
    // gs_println("Freeing: %zu, %s, %d", ptr, file, line); 
    if (!ptr) {
        gs_log_error("Attempting to free NULL pointer: %s, %d", file, line);
    }

    // Search for allocation and remove 
    gs_core_memory_erase(ptr, file, line);
    gs_ctx()->os.free(ptr);
} 

GS_CORE_API_EXTERN char*
gs_core_strdup_dbg(const char* str, const char* file, int line)
{
    char* dst = gs_ctx()->os.strdup(str);
    size_t sz = gs_string_length(dst);
    gs_core_memory_insert(dst, sz, file, line);
    return dst;
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
    void* data = gs_malloc(sz);
    memset(data, 0, sz);
    return data;
}

GS_API_DECL void 
gs_core_os_free(void* ptr)
{ 
	if (ptr) free(ptr);
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

#define STBDS_REALLOC   gs_realloc
#define STBDS_FREE      gs_free

#include "flecs/flecs.h"