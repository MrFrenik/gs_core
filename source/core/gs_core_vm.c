/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_vm.c

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

// Core includes
#include "gs_core_vm.h"

// Third Party Includes
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

GS_API_DECL gs_core_vm_state_t
gs_core_vm_state_new()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);       // Create way to open specific libs
    return L;
}

GS_API_DECL void
gs_core_vm_state_free(gs_core_vm_state_t s)
{
    if (!s) return;
    lua_State* L = (lua_State*)s;
    lua_close(L);
}

GS_API_PRIVATE int32_t 
gs_core_vm_writer(lua_State *L, const void *p, size_t sz, void *ud)
{ 
    gs_byte_buffer_t* buffer = (gs_byte_buffer_t*)ud;
    gs_byte_buffer_write_bulk(buffer, p, sz);
    return GS_CORE_VM_OK;
} 

GS_API_DECL bool
gs_core_vm_get_global(gs_core_vm_state_t* s, gs_core_vm_type_t type, const char* name) { 
    if (!s || !type || !name) return false;
    lua_State* L = (lua_State*)s;
    lua_getglobal(L, name);
    switch (type) {
        case GS_CORE_VM_TYPE_TABLE:     return lua_istable(L, -1); break;
        case GS_CORE_VM_TYPE_FUNC:      return lua_isfunction(L, -1); break;
        case GS_CORE_VM_TYPE_STR:       return lua_isstring(L, -1); break;
        case GS_CORE_VM_TYPE_BOOL:      return lua_isboolean(L, -1); break;
        case GS_CORE_VM_TYPE_F32:
        case GS_CORE_VM_TYPE_F64:
        case GS_CORE_VM_TYPE_U8:
        case GS_CORE_VM_TYPE_U16:
        case GS_CORE_VM_TYPE_U32:
        case GS_CORE_VM_TYPE_U64:
        case GS_CORE_VM_TYPE_S8:
        case GS_CORE_VM_TYPE_S16:
        case GS_CORE_VM_TYPE_S32:
        case GS_CORE_VM_TYPE_S64:
            return lua_isnumber(L, -1); 
        break;
    }
    return false;
}

// Push data to top of stack
GS_API_DECL void
gs_core_vm_stack_push(gs_core_vm_state_t s, gs_core_vm_type_t type, void* data) {
    if (!s || !type || !data) return;
    lua_State* L = (lua_State*)s;
    switch (type) { 
        case GS_CORE_VM_TYPE_STR:    lua_pushstring(L, (const char*)data); break;
        case GS_CORE_VM_TYPE_F32:    lua_pushnumber(L, *((float*)data)); break;
        case GS_CORE_VM_TYPE_F64:    lua_pushnumber(L, *((double*)data)); break;
        case GS_CORE_VM_TYPE_U8:     lua_pushnumber(L, *((uint8_t*)data)); break;
        case GS_CORE_VM_TYPE_U16:    lua_pushnumber(L, *((uint16_t*)data)); break;
        case GS_CORE_VM_TYPE_U32:    lua_pushnumber(L, *((uint32_t*)data)); break;
        case GS_CORE_VM_TYPE_U64:    lua_pushnumber(L, *((uint64_t*)data)); break;
        case GS_CORE_VM_TYPE_S8:     lua_pushnumber(L, *((int8_t*)data)); break;
        case GS_CORE_VM_TYPE_S16:    lua_pushnumber(L, *((int16_t*)data)); break;
        case GS_CORE_VM_TYPE_S32:    lua_pushnumber(L, *((int32_t*)data)); break;
        case GS_CORE_VM_TYPE_S64:    lua_pushnumber(L, *((int64_t*)data)); break;
        case GS_CORE_VM_TYPE_BOOL:   lua_pushboolean(L, *((int32_t*)data)); break;
    }
}

GS_API_DECL void 
gs_core_vm_dump_stack(gs_core_vm_state_t s) {
    if (!s) return;
    lua_State* L = (lua_State*)s;
    int32_t t = lua_gettop(L);
    for (int32_t i = 1; i <= t; ++i) {
        gs_printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
            case LUA_TNUMBER: { 
                gs_printf("%g\n", lua_tonumber(L, i));
            } break;
            case LUA_TSTRING: { 
                gs_printf("%s\n", lua_tostring(L, i));
            } break;
            case LUA_TBOOLEAN: { 
                gs_printf("%s\n", lua_toboolean(L, i) ? "true" : "false");
            } break;
            case LUA_TFUNCTION: { 
                gs_printf("%p\n", lua_topointer(L, i));
            } break;
            case LUA_TNIL: { 
                gs_printf("%s\n", "nil");
            } break;
            default: {
                gs_printf("%p\n", lua_topointer(L, i)); 
            } break;
        }
    }
}

GS_API_DECL int32_t
_gs_core_vm_load(gs_core_vm_state_t s, const char* name, const char* bytecode, size_t sz, int32_t env)
{ 
    // int32_t res = lua_load((lua_State*)s, name, bytecode, sz, env);
    // return res;
    return GS_CORE_VM_OK;
}

GS_API_DECL int32_t 
_gs_core_vm_pcall(gs_core_vm_state_t s, int32_t nargs, int32_t nresults, int32_t errfunc)
{
    return lua_pcall((lua_State*)s, nargs, nresults, errfunc);
}

GS_API_DECL int32_t
_gs_core_vm_interpret(gs_core_vm_state_t s, const char* src, size_t len)
{
    lua_State* L = (lua_State*)s;
    int32_t ret = luaL_loadstring(L, src);
    return _gs_core_vm_pcall(s, 0, LUA_MULTRET, 0);
}

GS_API_DECL bool
gs_core_vm_type_check(gs_core_vm_state_t s, gs_core_vm_type_t type, int32_t idx) {
    bool ret = false;
    lua_State* L = (lua_State*)s;
    switch (type) {
        case GS_CORE_VM_TYPE_F32:       
        case GS_CORE_VM_TYPE_F64:
        case GS_CORE_VM_TYPE_U8:
        case GS_CORE_VM_TYPE_U16:
        case GS_CORE_VM_TYPE_U32:
        case GS_CORE_VM_TYPE_U64:
        case GS_CORE_VM_TYPE_S8:
        case GS_CORE_VM_TYPE_S16:
        case GS_CORE_VM_TYPE_S32:
        case GS_CORE_VM_TYPE_S64:
            if (lua_isnumber(L, idx)) ret = true; 
            break;
        case GS_CORE_VM_TYPE_BOOL:   if (lua_isboolean(L, idx)) ret = true; break;
        case GS_CORE_VM_TYPE_STR:    if (lua_isstring(L, idx)) ret = true; break;
        case GS_CORE_VM_TYPE_FUNC:  if (lua_isfunction(L, idx)) ret = true; break;
        case GS_CORE_VM_TYPE_VEC2: { 
            if (lua_istable(L, idx)) {
                if (lua_getmetatable(L, idx)) { 
                    lua_getfield(L, -1, "__name");
                    if (lua_isstring(L, -1)) {
                        const char* MT = lua_tostring(L, -1); 
                        if (gs_string_compare_equal(MT, gs_to_str(gs_vec2_t))) {
                            ret = true;
                        }
                    }
                    lua_pop(L, 2);
                }
            }
        } break;
        case GS_CORE_VM_TYPE_VEC3: { 
            if (lua_istable(L, idx)) {
                if (lua_getmetatable(L, idx)) { 
                    lua_getfield(L, -1, "__name");
                    if (lua_isstring(L, -1)) {
                        const char* MT = lua_tostring(L, -1); 
                        if (gs_string_compare_equal(MT, gs_to_str(gs_vec3_t))) {
                            ret = true;
                        }
                    }
                    lua_pop(L, 2);
                }
            }
        } break;
        case GS_CORE_VM_TYPE_VEC4: { 
            if (lua_istable(L, idx)) {
                if (lua_getmetatable(L, idx)) { 
                    lua_getfield(L, -1, "__name");
                    if (lua_isstring(L, -1)) {
                        const char* MT = lua_tostring(L, -1); 
                        if (gs_string_compare_equal(MT, gs_to_str(gs_vec4_t))) {
                            ret = true;
                        }
                    }
                    lua_pop(L, 2);
                }
            }
        } break;
        default: break;
    }
    return ret;
}


GS_API_DECL int32_t 
_gs_core_vm_ret_val(gs_core_vm_state_t s, gs_core_vm_type_t type, int32_t idx, void* out) {
    if (!s 
        || !type 
        || (!out && (type != GS_CORE_VM_TYPE_FUNC && type != GS_CORE_VM_TYPE_STR))
    ) {
        return GS_CORE_VM_ERRMEM;
    }
    lua_State* L = (lua_State*)s;
    switch (type) {
        case GS_CORE_VM_TYPE_F32: { 
            if (gs_core_vm_type_check(s, type, idx)) {
                float* v = (float*)out;
                *v = lua_tonumber(L, idx); 
                return GS_CORE_VM_OK;
            }
        } break;
        case GS_CORE_VM_TYPE_VEC2: {
            if (gs_core_vm_type_check(s, type, idx)) {
                gs_vec2_t* v = (gs_vec2_t*)out;
                lua_getfield(L, idx, "x"); v->x = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "y"); v->y = lua_tonumber(L, -1); lua_pop(L, 1);
                return GS_CORE_VM_OK;
            }
        } break;
        case GS_CORE_VM_TYPE_VEC3: {
            if (gs_core_vm_type_check(s, type, idx)) {
                gs_vec3_t* v = (gs_vec3_t*)out;
                lua_getfield(L, idx, "x"); v->x = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "y"); v->y = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "z"); v->z = lua_tonumber(L, -1); lua_pop(L, 1);
                return GS_CORE_VM_OK;
            }
        } break;
        case GS_CORE_VM_TYPE_VEC4: {
            if (gs_core_vm_type_check(s, type, idx)) {
                gs_vec4_t* v = (gs_vec4_t*)out;
                lua_getfield(L, idx, "x"); v->x = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "y"); v->y = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "z"); v->z = lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, idx, "w"); v->w = lua_tonumber(L, -1); lua_pop(L, 1);
                return GS_CORE_VM_OK;
            }
        } break;
        case GS_CORE_VM_TYPE_FUNC: {
            if (gs_core_vm_type_check(s, type, idx)) {
                out = lua_topointer(L, idx);
                return GS_CORE_VM_OK;
            }
        } break;
        case GS_CORE_VM_TYPE_STR: {
            if (gs_core_vm_type_check(s, type, idx)) {
                out = lua_tostring(L, idx);
                return GS_CORE_VM_OK;
            }
        } break;
        default: break;
    }
    return GS_CORE_VM_ERRTYPE;
}

GS_API_DECL int32_t
_gs_core_vm_call(gs_core_vm_state_t s, const gs_core_vm_call_desc_t* desc) {
    return lua_pcall((lua_State*)s, desc->nargs, desc->nresults, desc->errfunc);
    // lua_call((lua_State*)s, desc->nargs, desc->nresults);
    // return GS_CORE_VM_OK;
}

// Loads raw script then transpiles script from file

GS_API_DECL gs_result 
gs_core_vm_script_load_from_file(gs_core_vm_script_t* script, const char* file_path, bool keep_src)
{
    if (!script) return GS_RESULT_FAILURE;
    size_t sz = 0;
    char* src = gs_platform_read_file_contents(file_path, "rb", (size_t*)&sz);
    if (!src) {
        gs_log_warning("Unable to open file: %s", file_path);
        return GS_RESULT_FAILURE;
    }

    // Compile script
    gs_core_vm_state_t S = gs_core_vm_state_new();

    // Release any previous byte code buffer
    if (script->bcode.data) {
        gs_byte_buffer_free(&script->bcode);
    }

    /*
    // Compile script source into raw lua bytecode
    gs_core_vm_script_compile_res_t res = gs_core_vm_compile(src, sz);

    // Copy over src from res
    memcpy(&script->src, &res.src, sizeof(script->src));
    memcpy(&script->bcode, &res.bcode, sizeof(script->bcode));

    // Free src if necessary
    if (!keep_src) { 
        gs_free(script->src.data);
        script->src.sz = 0;
    }

    // If byte-code invalid, return warning (need to parse an error from lua)
    if (!script->bcode.data) { 
        gs_log_warning("Unable to compile bytecode: %s", file_path);
        return GS_RESULT_FAILURE;
    }
    */

    return GS_RESULT_SUCCESS;
}

GS_API_DECL void 
gs_core_vm_script_free(gs_core_vm_script_t* script)
{
    if (!script) return;
    if (script->src.data) gs_free(script->src.data);
    if (script->bcode.data) gs_byte_buffer_free(&script->bcode);
}

GS_API_DECL void* 
gs_core_vm_ret_val_get(gs_core_vm_exec_ret_t* ret, int32_t idx)
{ 
    if (idx >= GS_CORE_VM_EXEC_RET_MAX || !ret->elem[idx].type) return NULL;
    return (ret->data + ret->elem[idx].offset);
} 

//===============================================================================================
// Native-Backend (C as a VM)
//===============================================================================================

// Native state has a stack, the function pointers need to live in the engine
// for "modules" (the scripting engine pro)

/*
    All VM exported functions will have a signature
    Native exported function have a signature
    Stack is command buffer of typed "elements"
    Ret val: Static byte buffer of N size, buffer of M types
*/

typedef struct { 
    gs_core_vm_type_t type;         // Type of element
    size_t size;                    // Byte size
    size_t position;                // Position in byte array
} NATIVE_gs_core_vm_stack_element_t;

// All internal native data, such as function pointer map, etc.
typedef struct {
    gs_hash_table(u64, gs_core_vm_fn_t) exported;    // All exported function pointers from "Scripting" side
    gs_hash_table(u64, gs_core_vm_fn_t) imported;    // All imported function pointers from "Native" side
} NATIVE_gs_core_vm_store_t;

typedef gs_command_buffer_t NATIVE_gs_core_vm_stack_t;

typedef struct {
    NATIVE_gs_core_vm_stack_t stack;
    gs_array(NATIVE_gs_core_vm_stack_element_t, GS_CORE_VM_STACK_SIZE) elements;
} NATIVE_gs_core_vm_state_t;

GS_API_PRIVATE gs_core_vm_state_t 
NATIVE_gs_core_vm_state_new()
{ 
    NATIVE_gs_core_vm_state_t* state = gs_calloc(1, sizeof(NATIVE_gs_core_vm_state_t));
    state->stack = gs_command_buffer_new();
    return state;
}

GS_API_PRIVATE void 
NATIVE_gs_core_vm_state_free(gs_core_vm_state_t state)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)state;
    gs_command_buffer_free(&N->stack);
    gs_free(N);
    N = NULL;
} 

// Imports/Exports
GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_register_export(struct gs_core_vm_script_engine_s* engine, char* name, gs_core_vm_fn_t fn)
{ 
    if (!engine || !name || !fn) { 
        return GS_CORE_VM_ERRMEM;
    } 
    NATIVE_gs_core_vm_store_t* store = (NATIVE_gs_core_vm_store_t*)engine->user_data;
    const u64 hash = gs_hash_str64(name);
    if (!gs_hash_table_exists(store->exported, hash)) { 
        gs_hash_table_insert(store->exported, hash, fn);
    }
    return GS_CORE_VM_OK;
}

GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_register_import(struct gs_core_vm_script_engine_s* engine, char* name, gs_core_vm_fn_t fn)
{ 
    if (!engine || !name || !fn) { 
        return GS_CORE_VM_ERRMEM;
    } 
    NATIVE_gs_core_vm_store_t* store = (NATIVE_gs_core_vm_store_t*)engine->user_data;
    const u64 hash = gs_hash_str64(name);
    if (!gs_hash_table_exists(store->imported, hash)) { 
        gs_hash_table_insert(store->imported, hash, fn);
    }
    return GS_CORE_VM_OK;
}

// Scripts
GS_API_PRIVATE gs_result 
NATIVE_gs_core_vm_script_load_from_file(gs_core_vm_script_t* script, const char* file_path, bool keep_src)
{ 
    // Nothing for this.
    return GS_RESULT_SUCCESS;
}

GS_API_PRIVATE gs_core_vm_script_compile_res_t 
NATIVE_gs_core_vm_script_compile(const char* src, size_t sz)
{ 
    gs_core_vm_script_compile_res_t res = {0};
    return res;
}

GS_API_PRIVATE void 
NATIVE_gs_core_vm_script_free(gs_core_vm_script_t* script)
{ 
}

// Stack
GS_API_PRIVATE bool 
NATIVE_gs_core_vm_get_global(gs_core_vm_state_t* s, gs_core_vm_type_t type, const char* name)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;
    return false;
}

GS_API_PRIVATE void 
NATIVE_gs_core_vm_stack_push(gs_core_vm_state_t s, gs_core_vm_type_t type, void* data)
{ 
    if (!s || !data || !type) return;
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;
    // Create element, store type
    NATIVE_gs_core_vm_stack_element_t elem = {
        .type = type, .position = N->stack.commands.position
    }; 
    // Push element to top of stack 
    gs_command_buffer_t* cb = &N->stack;
    cb->num_commands++;
    #define WRITE(T)\
        do {\
            elem.size = sizeof(T);\
            gs_byte_buffer_write(&cb->commands, T, *(T*)data);\
        } while (0)
    switch (type) {
        default: return; break;
		case GS_CORE_VM_TYPE_F32:       WRITE(f32); break;
        case GS_CORE_VM_TYPE_F64:       WRITE(f64); break;
        case GS_CORE_VM_TYPE_U8:        WRITE(uint8_t); break;
        case GS_CORE_VM_TYPE_U16:       WRITE(uint16_t); break;
        case GS_CORE_VM_TYPE_U32:       WRITE(uint32_t); break;
        case GS_CORE_VM_TYPE_U64:       WRITE(uint64_t); break;
        case GS_CORE_VM_TYPE_S8:        WRITE(int8_t); break;
        case GS_CORE_VM_TYPE_S16:       WRITE(int16_t); break;
        case GS_CORE_VM_TYPE_S32:       WRITE(int32_t); break;
        case GS_CORE_VM_TYPE_S64:       WRITE(int64_t); break;
        case GS_CORE_VM_TYPE_BOOL:      WRITE(bool); break;
        case GS_CORE_VM_TYPE_VEC2:      WRITE(gs_vec2_t); break;
        case GS_CORE_VM_TYPE_VEC3:      WRITE(gs_vec3_t); break;
        case GS_CORE_VM_TYPE_VEC4:      WRITE(gs_vec4_t); break;

        case GS_CORE_VM_TYPE_FUNC:
        case GS_CORE_VM_TYPE_PTR: {
            elem.size = sizeof(void*);
            gs_byte_buffer_write(&cb->commands, void*, data);
        } break;

        case GS_CORE_VM_TYPE_STR: {
            elem.size = strlen(data) + 1;
            gs_byte_buffer_write_bulk(&cb->commands, data, elem.size - 1);
            gs_byte_buffer_write(&cb->commands, char, '\0');
        } break;
    }
    gs_array_push(N->elements, elem);   // Write element header
} 

GS_API_PRIVATE void 
NATIVE_gs_core_vm_stack_pop(gs_core_vm_state_t s, int32_t cnt)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;

    // Go from top of stack all the way too bottom...
    if (!N->elements.size) return;
    N->elements.size = gs_max((int32_t)N->elements.size - cnt, 0);
}

GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_stack_top(gs_core_vm_state_t s)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;
    return N->elements.size;
}

GS_API_PRIVATE void* 
NATIVE_gs_core_vm_stack_get(gs_core_vm_state_t* s, int32_t idx)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s; 
    if (idx > N->elements.size) return NULL;
    return (N->stack.commands.data + N->elements.data[idx].position); 
}

GS_API_PRIVATE void 
NATIVE_gs_core_vm_stack_dump(gs_core_vm_state_t s)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;

    // Go from top of stack all the way too bottom...
    if (!N->elements.size) {
        gs_log_warning("Stack empty.");
        return;
    }

    #define READ(T, F, ...)\
        do {\
            T* v = (T*)NATIVE_gs_core_vm_stack_get(s, i);\
            gs_printf("%i - ", i);\
            gs_println(F, ## __VA_ARGS__);\
        } while (0)

    for (int32_t i = 0; i < N->elements.size; ++i) {
        NATIVE_gs_core_vm_stack_element_t elem = N->elements.data[i];
        switch (elem.type) { 
            case GS_CORE_VM_TYPE_F32:       READ(f32, "f32: %.2f", *v); break;
            case GS_CORE_VM_TYPE_F64:       READ(f64, "f64: %.2f", *v); break;
            case GS_CORE_VM_TYPE_U8:        READ(uint8_t, "u8: %zu", *v); break;
            case GS_CORE_VM_TYPE_U16:       READ(uint16_t, "u16: %zu", *v); break;
            case GS_CORE_VM_TYPE_U32:       READ(uint32_t, "u32: %zu", *v); break;
            case GS_CORE_VM_TYPE_U64:       READ(uint64_t, "u64: %zu", *v); break;
            case GS_CORE_VM_TYPE_S8:        READ(int8_t, "i8: %d", *v); break;
            case GS_CORE_VM_TYPE_S16:       READ(int16_t, "i16: %d", *v); break;
            case GS_CORE_VM_TYPE_S32:       READ(int32_t, "i32: %d", *v); break;
            case GS_CORE_VM_TYPE_S64:       READ(int64_t, "i64: %d", *v); break;
            case GS_CORE_VM_TYPE_BOOL:      READ(bool, "bool: %s", *v ? "true" : "false"); break;
            case GS_CORE_VM_TYPE_VEC2:      READ(gs_vec2_t, "v2: <%.2f %.2f>", v->x, v->y); break;
            case GS_CORE_VM_TYPE_VEC3:      READ(gs_vec3_t, "v3: <%.2f %.2f %.2f>", v->x, v->y, v->z); break;
            case GS_CORE_VM_TYPE_VEC4:      READ(gs_vec4_t, "v4: <%.2f %.2f %.2f %.2f>", v->x, v->y, v->z, v->w); break;
            case GS_CORE_VM_TYPE_STR: {
                const char* v = NATIVE_gs_core_vm_stack_get(s, i);
                gs_printf("%i - ", i);
                gs_println("str: %.*s", elem.size, v);
            } break;
        }
    }
}

GS_API_PRIVATE void 
NATIVE_gs_core_vm_stack_clear(gs_core_vm_state_t s)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s; 
    gs_command_buffer_clear(&N->stack);
    gs_array_clear(N->elements);
}

GS_API_PRIVATE bool 
NATIVE_gs_core_vm_type_check(gs_core_vm_state_t s, gs_core_vm_type_t type)
{ 
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;
    return false;
}

// Internal functions
GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_load(gs_core_vm_state_t s, const char* name, const char* bytecode, size_t sz, int32_t env)
{ 
    return GS_CORE_VM_OK;
} 

GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_interpret(gs_core_vm_state_t s, const char* src, size_t len)
{ 
    return GS_CORE_VM_OK;
}

GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_call(gs_core_vm_state_t s, const gs_core_vm_call_desc_t* desc)
{ 
    return GS_CORE_VM_OK;
}

GS_API_PRIVATE int32_t 
NATIVE_gs_core_vm_ret_val(gs_core_vm_state_t s, gs_core_vm_type_t type, int32_t idx, void* out)
{ 
    return GS_CORE_VM_OK;
}
    
GS_API_DECL gs_core_vm_exec_ret_t 
NATIVE_gs_core_vm_script_exec(gs_core_vm_script_engine_t* engine, gs_core_vm_state_t s, gs_core_vm_exec_desc_t* desc)
{ 
    gs_core_vm_exec_ret_t ret = {0};
    if (!engine || !s || !desc || (!desc->func.name && !desc->func.ptr)) goto error; 

    NATIVE_gs_core_vm_store_t* store = (NATIVE_gs_core_vm_store_t*)engine->user_data;
    NATIVE_gs_core_vm_state_t* N = (NATIVE_gs_core_vm_state_t*)s;
    gs_core_vm_fn_t fn = desc->func.ptr;

    // Look up function from desc if not available in ptr form
    if (!fn) {
        const uint64_t fnhash = gs_hash_str64(desc->func.name);
        if (!gs_hash_table_exists(store->exported, fnhash)) { 
            gs_log_warning("Exported function does not exist: %s", desc->func.name);
            goto error;
        } 
        fn = gs_hash_table_get(store->exported, fnhash); 
    }
    gs_assert(fn);

	int32_t stack_top = engine->stack_top(s);

    #define PUSH_VAL(T)\
        do {\
            T v = arg->var.val;\
            engine->stack_push(s, arg->type, &v);\
        } while (0) 

    // Push args to state stack
    for (uint32_t i = 0; i < GS_CORE_VM_EXEC_ARGS_MAX; ++i) { 
        gs_core_vm_exec_arg_t* arg = &desc->args[i];
        if (!arg->type) break;
        switch (arg->type) { 
            case GS_CORE_VM_TYPE_F32:       {PUSH_VAL(f32);} break;
            case GS_CORE_VM_TYPE_F64:       {PUSH_VAL(f64);} break;
            case GS_CORE_VM_TYPE_U8:        {PUSH_VAL(u8);} break;
            case GS_CORE_VM_TYPE_U16:       {PUSH_VAL(u16);} break;
            case GS_CORE_VM_TYPE_U32:       {PUSH_VAL(u32);} break;
            case GS_CORE_VM_TYPE_U64:       {PUSH_VAL(u64);} break;
            case GS_CORE_VM_TYPE_S8:        {PUSH_VAL(s8);} break;
            case GS_CORE_VM_TYPE_S16:       {PUSH_VAL(s16);} break;
            case GS_CORE_VM_TYPE_S32:       {PUSH_VAL(s32);} break;
            case GS_CORE_VM_TYPE_S64:       {PUSH_VAL(s64);} break;
            case GS_CORE_VM_TYPE_BOOL:      {PUSH_VAL(bool);} break;
            case GS_CORE_VM_TYPE_FUNC:  
            case GS_CORE_VM_TYPE_STR:    
            case GS_CORE_VM_TYPE_VEC2:      
            case GS_CORE_VM_TYPE_VEC3:
            case GS_CORE_VM_TYPE_VEC4:
            case GS_CORE_VM_TYPE_PTR:
                engine->stack_push(s, arg->type, arg->var.ptr);
				break;
        } 
    }
    // Get stack top
    int32_t top = engine->stack_top(s); 

    // Call function
    fn(engine, s);

    // Get stack top
    int32_t newtop = engine->stack_top(s);

    // Collect returns 
    size_t pos = 0;
    for (int32_t i = 0; i < GS_CORE_VM_EXEC_RET_MAX; ++i) {
        const int32_t idx = top + i;
        if (idx >= newtop) break;
        NATIVE_gs_core_vm_stack_element_t elem = N->elements.data[idx];
        gs_assert(pos + elem.size < GS_CORE_VM_EXEC_RET_BUFFER_SIZE);
        // Store return type
        ret.elem[i].type = elem.type;
        ret.elem[i].size = elem.size;
        ret.elem[i].offset = pos;
        // Push return data 
        memcpy(ret.data + pos, engine->stack_get(s, idx), elem.size);
    }

    // Just clear stack for now
    engine->stack_clear(s);

    error:

    // Return
    return ret;
}

/*==================================================================
// Scripting Engine
==================================================================*/

GS_API_DECL gs_core_vm_script_engine_t
gs_core_vm_native_script_engine_new(gs_core_vm_script_engine_init_fn_t init)
{ 
    gs_core_vm_script_engine_t engine = {0};

    engine.vm_state_new = NATIVE_gs_core_vm_state_new;
    engine.vm_state_free = NATIVE_gs_core_vm_state_free;

    // Imports/Exports
    engine.register_export = NATIVE_gs_core_vm_register_export;
    engine.register_import = NATIVE_gs_core_vm_register_import;

    // Scripts
    engine.script_load_from_file = NATIVE_gs_core_vm_script_load_from_file;
    engine.script_compile = NATIVE_gs_core_vm_script_compile;
    engine.script_free = NATIVE_gs_core_vm_script_free;
    engine.script_exec = NATIVE_gs_core_vm_script_exec;

    // Stack
    engine.stack_get_global = NATIVE_gs_core_vm_get_global;
    engine.stack_get = NATIVE_gs_core_vm_stack_get;
    engine.stack_push = NATIVE_gs_core_vm_stack_push;
    engine.stack_pop = NATIVE_gs_core_vm_stack_pop;
    engine.stack_top = NATIVE_gs_core_vm_stack_top;
    engine.stack_dump = NATIVE_gs_core_vm_stack_dump;
    engine.stack_clear = NATIVE_gs_core_vm_stack_clear;
    engine.stack_type_check = NATIVE_gs_core_vm_type_check;

    // Internal functions
    engine.load = NATIVE_gs_core_vm_load;
    engine.interpret = NATIVE_gs_core_vm_interpret;
    engine.call = NATIVE_gs_core_vm_call;
    engine.ret_val = NATIVE_gs_core_vm_ret_val;

    // Set up store
    engine.user_data = gs_calloc(1, sizeof(NATIVE_gs_core_vm_store_t));

    // Initialize from user-end (module registration and the like?)
    if (init) init(&engine);

    return engine;
}





