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
#define GS_GRAPHICS_IMPL_OPENGL_CORE
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
#define GS_CORE_MEMORY_DBG 1

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

// ============================================================================
// Memory Debugger Implementation
// ============================================================================
// Simple hash table for tracking allocations with proper thread safety
// Uses open addressing with linear probing for collision resolution

#define GS_MEM_DBG_HASH_SIZE    (GS_CORE_MEMORY_DBG_MAX_ALLOCATIONS * 2)  // 2x for low collision rate
#define GS_MEM_DBG_HASH_MASK    (GS_MEM_DBG_HASH_SIZE - 1)

typedef struct gs_mem_dbg_entry_s {
    void* ptr;
    size_t sz;
    const char* file;
    uint32_t line;
    uint32_t occupied;  // 1 = occupied, 0 = empty
} gs_mem_dbg_entry_t;

static gs_mem_dbg_entry_t g_mem_table[GS_MEM_DBG_HASH_SIZE] = {0};
static gs_atomic_int_t g_mem_lock = 0;
static volatile size_t g_total_allocated = 0;
static volatile size_t g_total_freed = 0;
static volatile size_t g_allocation_count = 0;

// Simple spinlock implementation
#define GS_MEM_LOCK()    while (gs_atomic_cmp_swp(&g_mem_lock, 1, 0) != 0) { /* spin */ }
#define GS_MEM_UNLOCK()  while (gs_atomic_cmp_swp(&g_mem_lock, 0, 1) != 1) { /* spin */ }

// Hash function - simple but effective
static inline uint32_t gs_mem_hash_ptr(void* ptr) {
    uintptr_t p = (uintptr_t)ptr;
    p ^= p >> 16;
    p *= 0x85ebca6b;
    p ^= p >> 13;
    p *= 0xc2b2ae35;
    p ^= p >> 16;
    return (uint32_t)(p % GS_MEM_DBG_HASH_MASK);
}

// Insert allocation record into hash table
static int32_t gs_mem_insert(void* ptr, size_t sz, const char* file, uint32_t line) {
    if (!ptr) return -1;
    
    GS_MEM_LOCK();
    
    uint32_t idx = gs_mem_hash_ptr(ptr);
    uint32_t start_idx = idx;
    gs_assert(idx < GS_MEM_DBG_HASH_SIZE);
    
    // Linear probing to find empty slot
    do {
        if (!g_mem_table[idx].occupied) {
            g_mem_table[idx].ptr = ptr;
            g_mem_table[idx].sz = sz;
            g_mem_table[idx].file = file;
            g_mem_table[idx].line = line;
            g_mem_table[idx].occupied = 1;
            g_total_allocated += sz;
            g_allocation_count++;
            GS_MEM_UNLOCK();
            return 0;
        }
        idx = (idx + 1) % GS_MEM_DBG_HASH_MASK;
        gs_assert(idx < GS_MEM_DBG_HASH_SIZE);
    } while (idx != start_idx);
    
    // Table full
    GS_MEM_UNLOCK();
    gs_log_error("Memory debug table full! Max allocations: %d", GS_MEM_DBG_HASH_SIZE);
    return -1;
}

// Find allocation record in hash table
static gs_mem_dbg_entry_t* gs_mem_find(void* ptr) {
    if (!ptr) return NULL;
    
    uint32_t idx = gs_mem_hash_ptr(ptr);
    uint32_t start_idx = idx;
    gs_assert(idx < GS_MEM_DBG_HASH_SIZE);
    
    do {
        if (g_mem_table[idx].occupied && g_mem_table[idx].ptr == ptr) {
            return &g_mem_table[idx];
        }
        if (!g_mem_table[idx].occupied && !g_mem_table[idx].ptr) {
            return NULL;  // Never inserted
        }
        idx = (idx + 1) % GS_MEM_DBG_HASH_MASK;
        gs_assert(idx < GS_MEM_DBG_HASH_SIZE);
    } while (idx != start_idx);
    
    return NULL;
}

// Remove allocation record from hash table
static int32_t gs_mem_remove(void* ptr, const char* file, uint32_t line) {
    if (!ptr) return -1;
    
    GS_MEM_LOCK();
    
    gs_mem_dbg_entry_t* entry = gs_mem_find(ptr);
    if (!entry) {
        GS_MEM_UNLOCK();
        gs_log_error("Double free or invalid free at %s:%d (ptr: %p)", file, line, ptr);
        return -1;
    }
    
    g_total_freed += entry->sz;
    g_allocation_count--;
    entry->occupied = 0;
    // Keep ptr set for tombstone (allows linear probing to continue)
    
    GS_MEM_UNLOCK();
    return 0;
}

// Print memory statistics
GS_API_DECL void gs_core_memory_print() {
    GS_MEM_LOCK();
    
    gs_println("=== Memory Debug Report ===");
    gs_println("Active allocations: %zu", g_allocation_count);
    gs_println("Total allocated:    %zu bytes", g_total_allocated);
    gs_println("Total freed:        %zu bytes", g_total_freed);
    gs_println("Net allocated:      %zu bytes", g_total_allocated - g_total_freed);
    
    if (g_allocation_count > 0) {
        gs_println("\nActive allocations:");
        uint32_t count = 0;
        for (uint32_t i = 0; i < GS_MEM_DBG_HASH_SIZE && count < g_allocation_count; ++i) {
            if (g_mem_table[i].occupied) {
                gs_println("  [%u] %p - %zu bytes @ %s:%u", 
                    count++, 
                    g_mem_table[i].ptr, 
                    g_mem_table[i].sz,
                    g_mem_table[i].file, 
                    g_mem_table[i].line);
            }
        }
    }
    
    GS_MEM_UNLOCK();
}

// Print memory report to file
GS_API_DECL void gs_core_memory_print_to_file(const char* path) {
    FILE* fp = fopen(path, "w");
    if (!fp) {
        gs_log_error("Failed to open memory debug file: %s", path);
        return;
    }
    
    GS_MEM_LOCK();
    
    fprintf(fp, "=== Memory Debug Report ===\n");
    fprintf(fp, "Active allocations: %zu\n", g_allocation_count);
    fprintf(fp, "Total allocated:    %zu bytes\n", g_total_allocated);
    fprintf(fp, "Total freed:        %zu bytes\n", g_total_freed);
    fprintf(fp, "Net allocated:      %zu bytes\n\n", g_total_allocated - g_total_freed);
    
    if (g_allocation_count > 0) {
        fprintf(fp, "Active allocations:\n");
        uint32_t count = 0;
        for (uint32_t i = 0; i < GS_MEM_DBG_HASH_SIZE && count < g_allocation_count; ++i) {
            if (g_mem_table[i].occupied) {
                fprintf(fp, "  [%u] %p - %zu bytes @ %s:%u\n",
                    count++,
                    g_mem_table[i].ptr,
                    g_mem_table[i].sz,
                    g_mem_table[i].file,
                    g_mem_table[i].line);
            }
        }
    }
    
    GS_MEM_UNLOCK();
    fclose(fp);
}

// ============================================================================
// Memory Debugger API Functions
// ============================================================================

GS_CORE_API_EXTERN void* 
gs_core_malloc_dbg(size_t sz, const char* file, int32_t line)
{
    void* ptr = gs_ctx()->os.malloc(sz);
    if (ptr) {
        gs_mem_insert(ptr, sz, file, (uint32_t)line);
    }
    return ptr;
}

GS_CORE_API_EXTERN void*
gs_core_realloc_dbg(void* ptr, size_t sz, const char* file, int32_t line)
{
    if (!ptr) {
        return gs_core_malloc_dbg(sz, file, line);
    }
    
    // Remove old allocation record
    gs_mem_remove(ptr, file, (uint32_t)line);
    
    // Perform realloc
    void* new_ptr = gs_ctx()->os.realloc(ptr, sz);
    
    // Add new allocation record
    if (new_ptr) {
        gs_mem_insert(new_ptr, sz, file, (uint32_t)line);
    }
    
    return new_ptr;
}

GS_CORE_API_EXTERN void*
gs_core_calloc_dbg(int num, size_t sz, const char* file, int line)
{
    void* ptr = gs_ctx()->os.calloc(num, sz);
    if (ptr) {
        gs_mem_insert(ptr, num * sz, file, (uint32_t)line);
    }
    return ptr;
}

GS_CORE_API_EXTERN void 
gs_core_free_dbg(void* ptr, const char* file, int line)
{
    if (!ptr) {
        return;  // Freeing NULL is valid in C
    }
    
    gs_mem_remove(ptr, file, (uint32_t)line);
    gs_ctx()->os.free(ptr);
}

GS_CORE_API_EXTERN char*
gs_core_strdup_dbg(const char* str, const char* file, int line)
{
    if (!str) return NULL;
    
    char* dst = gs_ctx()->os.strdup(str);
    if (dst) {
        size_t sz = gs_string_length(dst) + 1;  // +1 for null terminator
        gs_mem_insert(dst, sz, file, (uint32_t)line);
    }
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