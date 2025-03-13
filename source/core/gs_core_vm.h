/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_vm.h

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

#ifndef GS_CORE_VM_H
#define GS_CORE_VM_H 

#include <gs/gs.h>

#ifdef __cplusplus
extern "C" {
#endif 

// Error codes
#define GS_CORE_VM_OK		    0
#define GS_CORE_VM_YIELD	    1
#define GS_CORE_VM_ERRRUN	    2
#define GS_CORE_VM_ERRSYNTAX	3
#define GS_CORE_VM_ERRMEM	    4
#define GS_CORE_VM_ERRERR	    5
#define GS_CORE_VM_ERRTYPE	    6

// Stack size
// #define GS_CORE_VM_STACK_SIZE_BYTES        1024
#define GS_CORE_VM_STACK_SIZE              256
#define GS_CORE_VM_RET_VAL_MAX             4         // Total max number of ret-vals

#define GS_CORE_VM_EXEC_ARGS_MAX           8
#define GS_CORE_VM_EXEC_RET_MAX            8
#define GS_CORE_VM_EXEC_RET_BUFFER_SIZE    256       // Total buffer size of ret-val data

typedef void* gs_core_vm_state_t;

typedef enum 
{
    GS_CORE_VM_TYPE_INVALID = 0x00,
    GS_CORE_VM_TYPE_F32,
    GS_CORE_VM_TYPE_F64,
    GS_CORE_VM_TYPE_U8,
    GS_CORE_VM_TYPE_U16,
    GS_CORE_VM_TYPE_U32,
    GS_CORE_VM_TYPE_U64,
    GS_CORE_VM_TYPE_S8,
    GS_CORE_VM_TYPE_S16,
    GS_CORE_VM_TYPE_S32,
    GS_CORE_VM_TYPE_S64,
    GS_CORE_VM_TYPE_BOOL,
    GS_CORE_VM_TYPE_STR,
    GS_CORE_VM_TYPE_FUNC,
    GS_CORE_VM_TYPE_VEC2,
    GS_CORE_VM_TYPE_VEC3,
    GS_CORE_VM_TYPE_VEC4,
    GS_CORE_VM_TYPE_PTR,
    GS_CORE_VM_TYPE_TABLE
} gs_core_vm_type_t;

#define gs_core_vm_data(T)\
    struct {\
        T data;\
        size_t sz;\
    } 

// Script resource? Not sure about this...
typedef struct {
    const char* file_path;          // Path to raw source
    gs_core_vm_data(char*) src;     // Raw source code
    gs_byte_buffer_t bcode;         // Compiled bytecode
    uint32_t flags;                 // Flag to say whether or not its been interprted?
} gs_core_vm_script_t;

typedef struct {
    gs_byte_buffer_t bcode;        // Compiled bytecode
    gs_core_vm_data(char*) src;    // Compiled lua source
} gs_core_vm_script_compile_res_t;

typedef struct { 
    int32_t nargs;
    int32_t nresults;
    int32_t errfunc;
} gs_core_vm_call_desc_t; 

typedef union {
    void* ptr;      // Pointer to structure
    f64 val;        // Max 64-bit value
} gs_core_vm_variant_t;

typedef struct {
    gs_core_vm_type_t type;
    gs_core_vm_variant_t var;
} gs_core_vm_exec_arg_t;

typedef struct { 
    gs_core_vm_script_t* script;                            // Script to execute
    struct { 
        void* ptr;      
        const char* name;
    } func;                                                 // Function to call, either provide name or function ptr
    gs_core_vm_exec_arg_t args[GS_CORE_VM_EXEC_ARGS_MAX];   // Arguments to pass
    int32_t nresults;                                       // Number of return vals
    int32_t tid;
} gs_core_vm_exec_desc_t;

typedef struct { 
    gs_core_vm_type_t type;         // Type of value
    size_t size;                    // Size of data
    size_t offset;                  // Offset into data
} gs_core_vm_ret_val_t;

typedef struct { 
    int32_t err;
    uint8_t data[GS_CORE_VM_EXEC_RET_BUFFER_SIZE];        // Buffered data returned
    gs_core_vm_ret_val_t elem[GS_CORE_VM_EXEC_RET_MAX];   // Types returned by index
} gs_core_vm_exec_ret_t;

GS_API_DECL gs_core_vm_state_t gs_core_vm_state_new();
GS_API_DECL void gs_core_vm_state_free(gs_core_vm_state_t s);
GS_API_DECL gs_core_vm_script_compile_res_t gs_core_vm_compile(const char* src, size_t len);
GS_API_DECL bool gs_core_vm_get_global(gs_core_vm_state_t* s, gs_core_vm_type_t type, const char* name); 
GS_API_DECL void gs_core_vm_stack_push(gs_core_vm_state_t s, gs_core_vm_type_t type, void* data); 
GS_API_DECL void gs_core_vm_dump_stack(gs_core_vm_state_t s); 
GS_API_DECL bool gs_core_vm_type_check(gs_core_vm_state_t s, gs_core_vm_type_t type); 
GS_API_DECL void* gs_core_vm_ret_val_get(gs_core_vm_exec_ret_t* ret, int32_t idx);

GS_API_DECL int32_t 
gs_core_vm_writer(gs_core_vm_state_t s, const void *p, size_t sz, void *ud);

// Load then compile script from file
GS_API_DECL gs_result gs_core_vm_script_load_from_file(gs_core_vm_script_t* script, const char* file_path, bool keep_src);
GS_API_DECL void gs_core_vm_script_free(gs_core_vm_script_t* script);

// Internal functions
GS_API_DECL int32_t _gs_core_vm_load(gs_core_vm_state_t s, const char* name, const char* bytecode, size_t sz, int32_t env); 
GS_API_DECL int32_t _gs_core_vm_interpret(gs_core_vm_state_t s, const char* src, size_t len); 
GS_API_DECL int32_t _gs_core_vm_call(gs_core_vm_state_t s, const gs_core_vm_call_desc_t* desc); 
GS_API_DECL int32_t _gs_core_vm_ret_val(gs_core_vm_state_t s, gs_core_vm_type_t type, int32_t idx, void* out);

#define gs_core_vm_load(S, NAME, BYTECODE, SZ, ENV) (_gs_core_vm_load(S, NAME, BYTECODE, SZ, ENV) == GS_CORE_VM_OK)
#define gs_core_vm_interpret(S, SRC, LEN)           (_gs_core_vm_interpret(S, SRC, LEN) == GS_CORE_VM_OK) 
#define gs_core_vm_call(S, DESC)                    (_gs_core_vm_call(S, DESC) == GS_CORE_VM_OK) 
#define gs_core_vm_ret_val(S, TYPE, IDX, OUT)       (_gs_core_vm_ret_val(S, TYPE, IDX, OUT) == GS_CORE_VM_OK)

#define gs_core_vm_to_string(S, IDX)     lua_tostring((lua_State*)(S), (IDX))
#define gs_core_vm_to_integer(S, IDX)    lua_tointeger((lua_State*)(S), (IDX))
#define gs_core_vm_to_number(S, IDX)     lua_tonumber((lua_State*)(S), (IDX))
#define gs_core_vm_to_boolean(S, IDX)    lua_toboolean((lua_State*)(S), (IDX))
#define gs_core_vm_to_function(S, IDX)   lua_topointer((lua_State*)(S), (IDX))

/*==================================================================
// Scripting Engine
==================================================================*/

// Foward Decls.
typedef struct gs_core_vm_script_engine_s; 
typedef int32_t (* gs_core_vm_script_engine_init_fn_t)(struct gs_core_vm_script_engine_s* engine); 
typedef int32_t (* gs_core_vm_fn_t)(gs_core_vm_state_t* state);

typedef struct gs_core_vm_script_engine_s {
    gs_core_vm_state_t (* vm_state_new)();
    void (* vm_state_free)(gs_core_vm_state_t);

    // Imports/Exports
    int32_t (* register_export)(struct gs_core_vm_script_engine_s* engine, char* name, gs_core_vm_fn_t fn);
    int32_t (* register_import)(struct gs_core_vm_script_engine_s* engine, char* name, gs_core_vm_fn_t fn);

    // Scripts
    gs_result (* script_load_from_file)(gs_core_vm_script_t* script, const char* file_path, bool keep_src);
    gs_core_vm_script_compile_res_t (* script_compile)(const char*, size_t);
    void (* script_free)(gs_core_vm_script_t* script); 
    gs_core_vm_exec_ret_t (* script_exec)(struct gs_core_vm_script_engine_s*, gs_core_vm_state_t s, gs_core_vm_exec_desc_t* desc);

    // Stack
    bool (* stack_get_global)(gs_core_vm_state_t* s, gs_core_vm_type_t type, const char* name);
    void* (* stack_get)(gs_core_vm_state_t* s, int32_t idx);
    void (* stack_push)(gs_core_vm_state_t s, gs_core_vm_type_t type, void* data); 
    void (* stack_pop)(gs_core_vm_state_t s, int32_t cnt);
    int32_t (* stack_top)(gs_core_vm_state_t s);
    void (* stack_dump)(gs_core_vm_state_t s);
    bool (* stack_type_check)(gs_core_vm_state_t s, gs_core_vm_type_t type);
    void (* stack_clear)(gs_core_vm_state_t s);

    // Internal functions
    int32_t (* load)(gs_core_vm_state_t s, const char* name, const char* bytecode, size_t sz, int32_t env); 
    int32_t (* interpret)(gs_core_vm_state_t s, const char* src, size_t len); 
    int32_t (* call)(gs_core_vm_state_t s, const gs_core_vm_call_desc_t* desc); 
    int32_t (* ret_val)(gs_core_vm_state_t s, gs_core_vm_type_t type, int32_t idx, void* out);

    void* user_data;    // User specific engine data

} gs_core_vm_script_engine_t;

//===============================================================================================
// Native-Backend (C as a VM)
//===============================================================================================

GS_API_DECL gs_core_vm_script_engine_t
gs_core_vm_native_script_engine_new(gs_core_vm_script_engine_init_fn_t init);

GS_API_DECL 

#ifdef __cplusplus
}
#endif 

#endif // GS_CORE_VM_H



















