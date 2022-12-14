/*==============================================================================================================
    * Copyright: 2022 John Jackson 
/   * File: reflection.c

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
#include <gs/gs.h> 

#define META_PROPERTY_STR_MAX   128
#define META_FUNCTION_STR_MAX   2048

typedef struct
{ 
    char name[META_PROPERTY_STR_MAX];
    char type[META_PROPERTY_STR_MAX];
} property_t;

typedef struct
{
    char name[META_PROPERTY_STR_MAX];
    char content[META_FUNCTION_STR_MAX];
    char function[META_FUNCTION_STR_MAX];
    bool32 is_overriden;
} method_t;

typedef struct
{
    gs_hash_table(uint64_t, method_t) methods;
} vtable_t;

typedef struct
{
    char name[META_PROPERTY_STR_MAX];  
    char base[META_PROPERTY_STR_MAX];
    gs_dyn_array(property_t) properties;
    gs_hash_table(uint64_t, method_t) methods;
    vtable_t vtable;
    struct {
        bool32 reliable; 
    } rpc;
} class_t;

typedef struct
{
    gs_hash_table(uint64_t, class_t) classes;
    gs_hash_table(uint64_t, const char*) type_map;
} meta_t;

static meta_t g_meta = {0};

GS_API_DECL void write_to_file(meta_t* meta, const char* dir, const char* proj_name, uint32_t id_offset);
GS_API_DECL void parse_file(meta_t* meta, const char* path);

#define TOKEN_EXPECT(LEX, TYPE, MSG)\
    if (!gs_lexer_require_token_type(LEX, TYPE)) {\
        gs_token_t TOKEN = LEX->current_token;\
        gs_log_warning("Unidentified token: %.*s: %s", TOKEN.len, TOKEN.text, MSG);\
        return;\
    } 

#define TOKEN_COPY(TOKEN, DST)\
    memcpy(DST, (TOKEN).text, (TOKEN).len)

GS_API_DECL void
parse_class_rpc(meta_t* meta, class_t* cls, gs_lexer_t* lex)
{ 
    TOKEN_EXPECT(lex, GS_TOKEN_LPAREN, "Expect opening paren.");

    // End at final paren
    uint32_t ct = 1;
    gs_token_t token = lex->next_token(lex); 
    gs_token_t start = token; 

    // Return if closed
    if (token.type == GS_TOKEN_RPAREN) return;

    // Grab all text otherwise
    while (ct)
    {
        token = lex->next_token(lex);
        switch (token.type)
        {
            case GS_TOKEN_LPAREN: ct++; break;
            case GS_TOKEN_RPAREN: ct--; break;
            case GS_TOKEN_IDENTIFIER: 
            {
                if (gs_token_compare_text(&token, "reliable")) {
                    cls->rpc.reliable = true;
                }
            } break;
        }
    }
} 

GS_API_DECL void
parse_class_method(meta_t* meta, class_t* cls, gs_lexer_t* lex)
{
    method_t method = {0};

    // Copy method name
    TOKEN_COPY(lex->current_token, method.name);

    TOKEN_EXPECT(lex, GS_TOKEN_LPAREN, "Expect opening paren.");

    // End at final paren
    uint32_t ct = 1;
    gs_token_t token = lex->next_token(lex); 
    gs_token_t start = token; 

    // Return if closed
    if (token.type == GS_TOKEN_RPAREN) return;

    // Grab all text otherwise
    while (ct)
    {
        token = lex->next_token(lex);
        switch (token.type)
        {
            case GS_TOKEN_LPAREN: ct++; break;
            case GS_TOKEN_RPAREN: ct--; break;
        }
    }

    const uint32_t len = token.text - (start.text);
    memcpy(method.content, start.text, len);

    // Add to class
    gs_hash_table_insert(cls->methods, gs_hash_str64(method.name), method);
}

GS_API_DECL void
parse_class_property(meta_t* meta, class_t* cls, gs_lexer_t* lex)
{ 
    property_t prop = {0};

    // Parse parens
    TOKEN_EXPECT(lex, GS_TOKEN_LPAREN, "Expect lparen for _field");
    TOKEN_EXPECT(lex, GS_TOKEN_RPAREN, "Expect rparen for _field");

    // Get type
    gs_token_t token = lex->next_token(lex);
    gs_token_t start = token;

    // Determine if is pointer, then need to capture this
    while (gs_lexer_peek(lex).type == GS_TOKEN_ASTERISK)
    {
        token = lex->next_token(lex);
    }

    // Parse type
    int32_t len = token.text - start.text > 0 ? token.text - start.text + 1 : token.len; 
    memcpy(prop.type, start.text, len);

    // Parse name
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect property name");
    token = lex->current_token;
    TOKEN_COPY(token, prop.name);
    
    // Add property to class
    gs_dyn_array_push(cls->properties, prop);
}

GS_API_DECL void
parse_class_method_vt(meta_t* meta, class_t* cls, gs_lexer_t* lex, vtable_t* vt)
{
    method_t method = {0};

    /*
    typedef struct
    {
        char name[META_PROPERTY_STR_MAX];
        char content[META_FUNCTION_STR_MAX];
        bool32 is_overriden;
    } method_t;

    typedef struct
    {
        gs_hash_table(uint64_t, method_t) methods;
    } vtable_t;
    */ 

    // Check for "override", if so, then mark as such
    // Move until we find open paren and asterisk for name
    // Move to semicolon
    // Copy all text as contents
    
    // Beginning of method signature (return type)
    gs_token_t start = lex->current_token;
    gs_token_t token = start;

    // gs_token_debug_print(&token);

    // If find 'override' tag, then we're overriden
    if (gs_token_compare_text(&token, "_override"))
    {
        method.is_overriden = true;

        // Move to next colon and get identifier
        gs_lexer_find_next_token_type(lex, GS_TOKEN_COLON); 

        // Get method name
        TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect identifier for function pointer name");
        TOKEN_COPY(lex->current_token, method.name);

        // Look for equal for callback to be registered
        TOKEN_EXPECT(lex, GS_TOKEN_EQUAL, "Expect equal for function pointer");
        TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect function name for function pointer"); 
        TOKEN_COPY(lex->current_token, method.function);

        // Move to semicolon 
        gs_lexer_find_next_token_type(lex, GS_TOKEN_SEMICOLON);
        token = lex->current_token;

        gs_println("%s: %s", method.name, method.function);
    } 
    else
    { 
        // Move to next open paren
        gs_lexer_find_next_token_type(lex, GS_TOKEN_LPAREN);
        TOKEN_EXPECT(lex, GS_TOKEN_ASTERISK, "Expect asterisk for function pointer");

        // Get method name
        TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect identifier for function pointer name"); 
        TOKEN_COPY(lex->current_token, method.name);

        // Move past parens
        gs_lexer_find_next_token_type(lex, GS_TOKEN_RPAREN);
        gs_lexer_find_next_token_type(lex, GS_TOKEN_RPAREN);
        // gs_token_debug_print(&lex->current_token);

        // Look for equal for callback to be registered
        TOKEN_EXPECT(lex, GS_TOKEN_EQUAL, "Expect equal for function pointer name");

        // Copy content
        token = lex->current_token; 
        memcpy(method.content, start.text, (token.text - start.text));

        // gs_println("content: %s", method.content);

        // Get function name
        TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect function name for function pointer"); 
        TOKEN_COPY(lex->current_token, method.function);

        // Move to semicolon 
        gs_lexer_find_next_token_type(lex, GS_TOKEN_SEMICOLON);
        token = lex->current_token;
    }

    // Add method to vt
    uint64_t hash = gs_hash_str64(method.name);
    gs_hash_table_insert(vt->methods, hash, method);
}

GS_API_DECL void 
parse_class_vtable(meta_t* meta, class_t* cls, gs_lexer_t* lex)
{

    vtable_t vt = {0};

    // Parse left paren
    TOKEN_EXPECT(lex, GS_TOKEN_LPAREN, "Expect lparen for _vtable");

    // Paren token
    gs_token_t token = lex->current_token;

    // Until paren count is closed
    uint32_t pc = 1;
    while (pc)
    {
        token = lex->next_token(lex);
        switch (token.type)
        {
            case GS_TOKEN_RPAREN: pc--; break;
            case GS_TOKEN_LPAREN: pc++; break;
            default:
            {
                parse_class_method_vt(meta, cls, lex, &vt);
            }  break; 
        }
    } 

    cls->vtable = vt; 
}

GS_API_DECL void 
parse_class_properties(meta_t* meta, class_t* cls, gs_lexer_t* lex)
{
    // Continue to parse until closing brace
    gs_token_t token = lex->current_token;

    uint32_t ct = 1;
    while (ct)
    {
        token = lex->next_token(lex);
        switch (token.type)
        { 
            case GS_TOKEN_LBRACE: {ct++;} break;
            case GS_TOKEN_RBRACE: {ct--;} break;

            case GS_TOKEN_IDENTIFIER: 
            {
                token = lex->current_token; 

                // Parse VTable
                if (gs_token_compare_text(&token, "_vtable"))
                {
                    parse_class_vtable(meta, cls, lex);
                } 

                // Parse functions
                else if (
                    gs_token_compare_text(&token, "_ctor") || 
                    gs_token_compare_text(&token, "_dtor") || 
                    gs_token_compare_text(&token, "_callback")
                )
                {
                    parse_class_method(meta, cls, lex);
                }
                else if (gs_token_compare_text(&token, "_rpc"))
                {
                    parse_class_rpc(meta, cls, lex);
                }
                else if (gs_token_compare_text(&token, "_field"))
                { 
                    parse_class_property(meta, cls, lex);
                }
            } break;
        }
    }
} 

GS_API_DECL void
parse_class(meta_t* meta, gs_lexer_t* lex)
{
    // Class to construct
    class_t cls = {0};

    gs_token_t token = lex->current_token;

    // Move to end of right paren
    gs_lexer_find_next_token_type(lex, GS_TOKEN_RPAREN);

    // Move past typedef, move past struct 
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect typedef");
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect struct");

    // Move to next open brace
    gs_lexer_find_next_token_type(lex, GS_TOKEN_LBRACE);

    // Look for core base
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect gs_core_base()"); 

    // Check against token, make sure it IS gs_core_base
    if (!gs_token_compare_text(&lex->current_token, "gs_core_base"))
    {
        gs_log_warning("Expected gs_core_base identifier"); 
        gs_lexer_find_next_token_type(lex, GS_TOKEN_RBRACE);
        return;
    }

    // Open paren
    TOKEN_EXPECT(lex, GS_TOKEN_LPAREN, "Expect open paren for gs_core_base"); 

    // Parse and copy base
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expected name" );
    token = lex->current_token;
    TOKEN_COPY(token, cls.base);

    // Close paren and semicolon
    TOKEN_EXPECT(lex, GS_TOKEN_RPAREN, "Expect close paren for gs_core_base"); 
    TOKEN_EXPECT(lex, GS_TOKEN_SEMICOLON, "Expect semicolon for gs_core_base"); 

    // Parse all properties
    parse_class_properties(meta, &cls, lex);

    // Parse class name
    TOKEN_EXPECT(lex, GS_TOKEN_IDENTIFIER, "Expect class name");
    token = lex->current_token;
    TOKEN_COPY(token, cls.name);

    // Add class
    gs_hash_table_insert(meta->classes, gs_hash_str64(cls.name), cls);
}

// List of files to iterate through for includes

typedef struct {char path[512];} FILE_PATH;
gs_dyn_array(FILE_PATH) g_files;

typedef (* iterate_dir_cb)(const char* path, struct dirent* entry);

void iterate_dir(const char* path, iterate_dir_cb cb)
{ 
    DIR* dir;
    struct dirent* entry;

    if (!(dir = opendir(path)))
    { 
        return;
    } 

    while ((entry = readdir(dir)) != NULL)
    {
        // Absolute path
        gs_snprintfc(PATH, 1024, "%s/%s", path, entry->d_name);

        switch (entry->d_type)
        {
            case DT_DIR:
            {
                if (
                    gs_string_compare_equal(entry->d_name, "..") || 
                    gs_string_compare_equal(entry->d_name, ".") || 
                    gs_string_compare_equal(entry->d_name, "reflection") || 
                    gs_string_compare_equal(entry->d_name, "generated") ||
                    gs_string_compare_equal(entry->d_name, "proj_gen") 
                )
                {
                    continue;
                } 

                // Recursively iterate path
                iterate_dir(PATH, cb);
            } break;

            default:
            {
                // Callback
                cb(PATH, entry);
            } break;
        }
    }
    closedir(dir);
}

void refl_iterate_dir_cb(const char* path, struct dirent* entry)
{
    // Only operate on header files
    gs_transient_buffer(file_ext, 32);
    gs_platform_file_extension(file_ext, sizeof(file_ext), path); 
    if (!gs_string_compare_equal(file_ext, "h"))
    {
        return;
    }

    // Ignore list
    if (gs_string_compare_equal(entry->d_name, "gs_core_object.h"))
    {
        // return;
    }

    if (!gs_platform_file_exists(path))
    {
        gs_log_error("Reflection::path does not exist %s", path);
    } 

    // Add to files
    FILE_PATH fp = {0};
    gs_snprintf(fp.path, 512, "%s", path); 
    gs_dyn_array_push(g_files, fp);

    char* contents = gs_platform_read_file_contents(path, "rb", NULL); 
    gs_lexer_t lex = gs_lexer_c_ctor(contents);

    while (lex.can_lex(&lex))
    { 
        gs_token_t token = lex.next_token(&lex);
        switch (token.type)
        {
            case GS_TOKEN_IDENTIFIER:
            {
                if (gs_token_compare_text(&token, "_introspect"))
                {
                    parse_class(&g_meta, &lex);
                }
            } break; 
        }
    }
    gs_free(contents);
} 

int32_t main(int32_t argc, char** argv)
{ 
    if (argc < 2)
    {
        gs_log_error("Missing input directory for reflection!");
        return -1;
    }

    if (argc < 3)
    {
        gs_log_error("Missing output directory for generated file!");
        return -1;
    }

    // Need to have a project name as well, so I can generate the files effectively
    if (argc < 4)
    {
        gs_log_error("Missing project name!");
        return -1;
    }

    // Initialize type map 
    struct {const char* key; const char* info;} type_map[] = {

        {.key = "int8_t",       .info = "GS_META_PROPERTY_TYPE_INFO_S8"},
        {.key = "s8",           .info = "GS_META_PROPERTY_TYPE_INFO_S8"},
        {.key = "uint8_t",      .info = "GS_META_PROPERTY_TYPE_INFO_U8"},
        {.key = "u8",           .info = "GS_META_PROPERTY_TYPE_INFO_U8"},
        {.key = "int16_t",      .info = "GS_META_PROPERTY_TYPE_INFO_S16"},
        {.key = "s16",          .info = "GS_META_PROPERTY_TYPE_INFO_S16"},
        {.key = "uint16_t",     .info = "GS_META_PROPERTY_TYPE_INFO_U16"},
        {.key = "u16",          .info = "GS_META_PROPERTY_TYPE_INFO_U16"},
        {.key = "uint32_t",     .info = "GS_META_PROPERTY_TYPE_INFO_U32"},
        {.key = "u32",          .info = "GS_META_PROPERTY_TYPE_INFO_U32"},
        {.key = "int32_t",      .info = "GS_META_PROPERTY_TYPE_INFO_S32"},
        {.key = "s32",          .info = "GS_META_PROPERTY_TYPE_INFO_S32"},
        {.key = "uint64_t",     .info = "GS_META_PROPERTY_TYPE_INFO_U64"},
        {.key = "u64",          .info = "GS_META_PROPERTY_TYPE_INFO_U64"},
        {.key = "int64_t",      .info = "GS_META_PROPERTY_TYPE_INFO_S64"},
        {.key = "s64",          .info = "GS_META_PROPERTY_TYPE_INFO_S64"},
        {.key = "float",        .info = "GS_META_PROPERTY_TYPE_INFO_F32"},
        {.key = "f32",          .info = "GS_META_PROPERTY_TYPE_INFO_F32"},
        {.key = "double",       .info = "GS_META_PROPERTY_TYPE_INFO_F64"},
        {.key = "f64",          .info = "GS_META_PROPERTY_TYPE_INFO_F64"}, 
        {.key = "gs_uuid_t",    .info = "GS_META_PROPERTY_TYPE_INFO_UUID"}, 
        {.key = "size_t",       .info = "GS_META_PROPERTY_TYPE_INFO_SIZE_T"}, 
        {.key = "const char*",  .info = "GS_META_PROPERTY_TYPE_INFO_STR"}, 
        {.key = "char*",        .info = "GS_META_PROPERTY_TYPE_INFO_STR"}, 
        {.key = "gs_vec2",      .info = "GS_META_PROPERTY_TYPE_INFO_VEC2"}, 
        {.key = "gs_vec3",      .info = "GS_META_PROPERTY_TYPE_INFO_VEC3"}, 
        {.key = "gs_vec4",      .info = "GS_META_PROPERTY_TYPE_INFO_VEC4"}, 
        {.key = "gs_quat",      .info = "GS_META_PROPERTY_TYPE_INFO_QUAT"}, 
        {.key = "gs_mat4",      .info = "GS_META_PROPERTY_TYPE_INFO_MAT4"},
        {.key = "gs_vqs",       .info = "GS_META_PROPERTY_TYPE_INFO_VQS"},
        {.key = "gs_color_t",   .info = "GS_META_PROPERTY_TYPE_INFO_COLOR"},

        {NULL}
    };

    for (uint32_t i = 0; type_map[i].key != NULL; ++i) 
    {
        gs_hash_table_insert(g_meta.type_map, gs_hash_str64(type_map[i].key), type_map[i].info);
    }

    const char* in_dir = argv[1];
    const char* out_dir = argv[2];
    const char* proj_name = argv[3];
    uint32_t id_offset = argc > 4 ? atoi(argv[4]) : 0;

    gs_println("in_dir: %s", in_dir);
    gs_println("out_dir: %s", out_dir);
    gs_println("proj_name: %s", proj_name);

    // Directory where written information will live
    // const char* dir = "../source/generated/"; 
    const char* dir = out_dir; 

    // Iterate through core files
    // iterate_dir("../../gs_core/source", refl_iterate_dir_cb); 

    // Iterate through project files (this should be passed in) 
    // for source files to parse, ignore third party directory, generated, reflection
    iterate_dir(in_dir, refl_iterate_dir_cb);

    write_to_file(&g_meta, dir, proj_name, id_offset);

    return 0;
}

GS_API_DECL void 
write_to_file(meta_t* meta, const char* dir, const char* proj_name, uint32_t id_offset)
{
    FILE* fp = NULL; 
    char PATH[256] = {0}; 
    uint32_t CID = 1;
    uint32_t FID = 1;

    // Create dir if needed
    if (!gs_platform_dir_exists(dir))
    {
        gs_platform_mkdir(dir, 0x00);
    }

#define COPYRIGHT(FP, FILE_NAME)\
    do {\
        const uint32_t ecnt = 110;\
        gs_fprintf(FP, "/*"); for (uint32_t i = 0; i < ecnt; ++i) gs_fprintf(FP, "="); gs_fprintf(FP, "\n");\
        gs_fprintln(FP, "\t* Copyright: 2022 John Jackson");\
        gs_fprintln(FP, "\t* File: %s", FILE_NAME);\
        gs_fprintln(FP, "\t* Note: This file has been generated. Any and ALL local changes will be lost.\n");\
\
        gs_fprintln(FP, "\tAll Rights Reserved\n");\
\
        gs_fprintln(FP, "\tBSD 3-Clause License\n");\
\
        gs_fprintln(FP, "\tCopyright (c) 2022 John Jackson\n");\
\
        gs_fprintln(FP, "\tRedistribution and use in source and binary forms, with or without");\
        gs_fprintln(FP, "\tmodification, are permitted provided that the following conditions are met:\n");\
\
        gs_fprintln(FP, "\t1. Redistributions of source code must retain the above copyright notice, this");\
        gs_fprintln(FP, "\t\tlist of conditions and the following disclaimer.\n");\
\
        gs_fprintln(FP, "\t2. Redistributions in binary form must reproduce the above copyright notice,");\
        gs_fprintln(FP, "\t\tthis list of conditions and the following disclaimer in the documentation");\
        gs_fprintln(FP, "\t\tand/or other materials provided with the distribution.\n");\
\
        gs_fprintln(FP, "\t3. Neither the name of the copyright holder nor the names of its contributors may be used to");\
        gs_fprintln(FP, "\t\tendorse or promote products derived from this software without specific prior written permission.\n");\
\
        gs_fprintln(FP, "\tTHIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND");\
        gs_fprintln(FP, "\tANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED");\
        gs_fprintln(FP, "\tWARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE");\
        gs_fprintln(FP, "\tDISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR");\
        gs_fprintln(FP, "\tANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES");\
        gs_fprintln(FP, "\t(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES");\
        gs_fprintln(FP, "\tLOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND");\
        gs_fprintln(FP, "\tON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT");\
        gs_fprintln(FP, "\t(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS");\
        gs_fprintln(FP, "\tSOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");\
\
        for (uint32_t i = 0; i < ecnt; ++i) gs_fprintf(FP, "="); gs_fprintln(FP, "*/");\
    } while (0) 

    //====[ Header ]====//

    gs_snprintfc(FNAME, 256, "%s_generated.h", proj_name);

    gs_snprintf(PATH, sizeof(PATH), "%s/%s", dir, FNAME);
    fp = fopen(PATH, "w"); 
    gs_assert(fp);

    // Copyright info 
    COPYRIGHT(fp, FNAME);
    
    // Include guard
    gs_fprintln(fp, "\n#ifndef %s_GENERATED_H", proj_name);
    gs_fprintln(fp, "#define %s_GENERATED_H\n", proj_name); 

    // Includes
    gs_fprintln(fp, "// Includes");
    gs_fprintln(fp, "#include \"core/gs_core_object.h\""); 
    gs_fprintln(fp, "#include \"core/gs_core_gs.h\"");
    gs_fprintln(fp, "#include \"core/gs_core_object.h\"");
    for (uint32_t i = 0; i < gs_dyn_array_size(g_files); ++i)
    {
        gs_fprintln(fp, "#include \"%s\"", g_files[i].path);
    }
    gs_fprintln(fp, "");

    // Components
    gs_fprintln(fp, "//======[ Components ]=======//\n");

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_ecs_register();\n", proj_name);

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
        
        if (gs_string_compare_equal(cls->base, "gs_core_entities_component_t"))
        {
            gs_fprintln(fp, "gs_core_entities_component_declare(%s);", cls->name);
        }
    } 

    gs_fprintln(fp, "");

    // Systems
    gs_fprintln(fp, "//======[ Systems ]=======//\n");

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
        if (gs_string_compare_equal(cls->base, "gs_core_entities_system_t"))
        {
            gs_fprintln(fp, "gs_core_entities_system_declare(%s);\n", cls->name);

            gs_fprintln(fp, "GS_API_DECL void"); 
            gs_fprintln(fp, "_%s_cb(gs_core_entity_iter_t* iter);\n", cls->name);

            gs_fprintln(fp, "GS_API_DECL void"); 
            gs_fprintln(fp, "%s_cb(gs_core_entities_system_t* system);\n", cls->name);
        }
    }

    gs_fprintln(fp, "");

    // Classes
    gs_fprintln(fp, "//======[ Classes ]=======//\n");

    // VTable Definitions
    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it);

        // Vtable declaration 
        gs_fprintln(fp, "#define %s_vtable_t_methods\\", cls->name); 

        if (gs_string_compare_equal(cls->name, "gs_core_obj_t"))
        {
            gs_fprintln(fp, "\tgs_core_vtable_t_methods\\");
        }

        int32_t noc = 0;
        for (
            gs_hash_table_iter it = gs_hash_table_iter_new(cls->vtable.methods);
            gs_hash_table_iter_valid(cls->vtable.methods, it); 
            gs_hash_table_iter_advance(cls->vtable.methods, it)
        )
        { 
            method_t* mt = gs_hash_table_iter_getp(cls->vtable.methods, it);
            if (!mt->is_overriden) noc++;
        }

        // Do any base declaration here
        if (cls->base && !gs_string_compare_equal(cls->name, "gs_core_obj_t"))
        { 
            gs_fprintf(fp, "\t%s_vtable_t_methods", cls->base); 

            // Check if empty
            if (noc && !gs_hash_table_empty(cls->vtable.methods))
            {
                gs_fprintf(fp, "\\"); 
            }
            gs_fprintf(fp, "\n");
        }

        // Get count of non-overriden functions

        // All vtable functions
        uint32_t ct = 0;
        for (
            gs_hash_table_iter it = gs_hash_table_iter_new(cls->vtable.methods);
            gs_hash_table_iter_valid(cls->vtable.methods, it); 
            gs_hash_table_iter_advance(cls->vtable.methods, it)
        )
        {
            method_t* mt = gs_hash_table_iter_getp(cls->vtable.methods, it);
            if (!mt->is_overriden)
            {
                gs_fprintf(fp, "\t%s;", mt->content); 
            } 
            if (noc && ct != noc - 1)
            {
                gs_fprintf(fp, "\\");
            }
            if (!mt->is_overriden) 
            {
                ct++;
                gs_fprintf(fp, "\n"); 
            }
        } 

        gs_fprintf(fp, "\n"); 

    }

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it);

        gs_fprintln(fp, "/* %s */\n", cls->name); 

        gs_fprintln(fp, "GS_API_DECL uint32_t");
        gs_fprintln(fp, "%s_class_id();\n", cls->name);

        gs_fprintln(fp, "GS_API_DECL const gs_meta_class_t*");
        gs_fprintln(fp, "%s_class();\n", cls->name);

        gs_fprintln(fp, "GS_API_DECL const gs_core_meta_info_t*");
        gs_fprintln(fp, "%s_info();\n", cls->name);

        gs_fprintln(fp, "GS_API_DECL %s*", cls->name);
        gs_fprintln(fp, "%s_new();\n", cls->name);

        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_init(%s* obj);\n", cls->name, cls->name);

        gs_fprintln(fp, "GS_API_DECL %s", cls->name);
        gs_fprintln(fp, "%s_ctor();\n", cls->name);

        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_dtor(gs_core_obj_t* obj);\n", cls->name);

        // If is base of rpc, then check for cb
        if (gs_string_compare_equal(cls->base, "gs_core_network_rpc_t") && cls->methods)
        {
            method_t* rpc = gs_hash_table_getp(cls->methods, gs_hash_str64("_callback"));
            if (rpc)
            {
                gs_fprintln(fp, "GS_API_DECL void");
                gs_fprintln(fp, "%s(%s* rpc);\n", rpc->content, cls->name);
            }
            else
            {
                gs_fprintln(fp, "GS_API_DECL void");
                gs_fprintln(fp, "%s_cb(%s* rpc);\n", cls->name, cls->name);
            }
        } 

        // VTable Struct
        gs_fprintln(fp, "typedef struct %s_vtable_s", cls->name); 
        gs_fprintln(fp, "{"); 
        gs_fprintln(fp, "\t%s_vtable_t_methods", cls->name); 
        gs_fprintln(fp, "} %s_vtable_t;", cls->name);

        gs_fprintln(fp, ""); 

        // Init 
        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_vtable_t_init(%s_vtable_t* vt);\n", cls->name, cls->name);

        // Ctor
        gs_fprintln(fp, "GS_API_DECL %s_vtable_t", cls->name);
        gs_fprintln(fp, "%s_vtable_t_ctor();\n", cls->name);
    }

    gs_fprintln(fp, "//========================//\n");

    // Registry
    gs_fprintln(fp, "//============[ Registry ]===============//\n");

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_register();\n", proj_name); 

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_ecs_register();\n", proj_name); 

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_unregister();\n", proj_name); 

    // Include guard close
    gs_fprintln(fp, "#endif // GENERATED_H\n");

    fclose(fp);

    //====[ Source ]====//
    
    gs_snprintfc(SNAME, 256, "%s_generated.c", proj_name);

    gs_snprintf(PATH, sizeof(PATH), "%s/%s", dir, SNAME); 
    fp = fopen(PATH, "w"); 
    gs_assert(fp);

    COPYRIGHT(fp, SNAME); 

    gs_fprintln(fp, "\n// Includes");

    // Includes
    gs_fprintln(fp, "#include \"%s_generated.h\"\n", proj_name); 
    gs_fprintln(fp, "#include \"core/generated/gs_core_generated.h\"\n");

    gs_fprintln(fp, "// Class Ids");

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
        gs_fprintln(fp, "#define %s_cls_id %zu", cls->name, CID + id_offset);
    }

    gs_fprintln(fp, "");

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 

        gs_fprintln(fp, "/* %s */\n", cls->name); 

        // gs_println("cls: %s", cls->name);

        // Class ID
        gs_fprintln(fp, "GS_API_DECL uint32_t");
        gs_fprintln(fp, "%s_class_id()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\treturn %s_cls_id;", cls->name);
        gs_fprintln(fp, "}\n");

        // Class
        gs_fprintln(fp, "GS_API_DECL const gs_meta_class_t*");
        gs_fprintln(fp, "%s_class()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\treturn gs_core_meta_obj_info_w_cls_id(%s_cls_id)->cls;", cls->name);
        gs_fprintln(fp, "}\n");

        // Info
        gs_fprintln(fp, "GS_API_DECL const gs_core_meta_info_t*");
        gs_fprintln(fp, "%s_info()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\treturn gs_core_meta_obj_info_w_cls_id(%s_cls_id);", cls->name);
        gs_fprintln(fp, "}\n");

        // Init
        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_init(%s* obj)", cls->name, cls->name);
        gs_fprintln(fp, "{"); 
        gs_fprintln(fp, "\tgs_core_cast(obj, gs_core_base_t)->id = %s_class_id();", cls->name);

        // If is rpc, then need to init rpc info
        if (gs_string_compare_equal(cls->base, "gs_core_network_rpc_t") && cls->methods)
        { 
            method_t* cb = gs_hash_table_getp(cls->methods, gs_hash_str64("_callback"));
            gs_snprintfc(CB_GEN, 128, "%s_cb", cls->name);
            gs_snprintfc(DELIVERY_STR, 128, "%s", cls->rpc.reliable ? "GS_CORE_NETWORK_DELIVERY_RELIABLE" : 
                    "GS_CORE_NETWORK_DELIVERY_UNRELIABLE");
            gs_snprintfc(CB_STR, 128, "%s", cb ? cb->content : CB_GEN);
            gs_fprintln(fp, "\tgs_core_network_rpc_t* rpc = gs_core_cast(obj, gs_core_network_rpc_t);");
            gs_fprintln(fp, "\trpc->delivery = %s;", DELIVERY_STR);
            gs_fprintln(fp, "\trpc->callback = %s;", CB_STR);
        }

        if (gs_hash_table_exists(cls->methods, gs_hash_str64("_ctor")))
        {
            method_t* func = gs_hash_table_getp(cls->methods, gs_hash_str64("_ctor"));
            gs_fprintln(fp, "\t%s* this = (%s*)obj;", cls->name, cls->name);
            gs_fprintln(fp, "\t%s", func->content);
        }

        gs_fprintln(fp, "}\n");

        // New
        gs_fprintln(fp, "GS_API_DECL %s*", cls->name);
        gs_fprintln(fp, "%s_new()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\t%s* obj = gs_malloc_init(%s);", cls->name, cls->name);
        gs_fprintln(fp, "\t%s_init(obj);", cls->name); 
        gs_fprintln(fp, "\treturn obj;");
        gs_fprintln(fp, "}\n");

        // Ctor
        gs_fprintln(fp, "GS_API_DECL %s", cls->name);
        gs_fprintln(fp, "%s_ctor()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\t%s obj = gs_default_val();", cls->name);
        gs_fprintln(fp, "\t%s_init(&obj);", cls->name); 
        gs_fprintln(fp, "\treturn obj;");
        gs_fprintln(fp, "}\n");

        // Dtor
        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_dtor(gs_core_obj_t* obj)", cls->name);
        gs_fprintln(fp, "{");

        if (gs_hash_table_exists(cls->methods, gs_hash_str64("_dtor")))
        {
            method_t* func = gs_hash_table_getp(cls->methods, gs_hash_str64("_dtor"));
            gs_fprintln(fp, "\t%s* this = (%s*)obj;", cls->name, cls->name);
            gs_fprintln(fp, "\t%s", func->content);
        }

        gs_fprintln(fp, "}\n");

        // VTable

        // Init 
        gs_fprintln(fp, "GS_API_DECL void");
        gs_fprintln(fp, "%s_vtable_t_init(%s_vtable_t* vt)", cls->name, cls->name);
        gs_fprintln(fp, "{");

        // Set up as base
        if (cls->base && !gs_string_compare_equal(cls->name, "gs_core_obj_t"))
        {
            gs_fprintln(fp, "\t%s_vtable_t_init((%s_vtable_t*)vt);", cls->base, cls->base);
        }

        // Iterate through all and set up function pointers
        for (
            gs_hash_table_iter mit = gs_hash_table_iter_new(cls->vtable.methods); 
            gs_hash_table_iter_valid(cls->vtable.methods, mit);
            gs_hash_table_iter_advance(cls->vtable.methods, mit)
        )
        {
            method_t* mt = gs_hash_table_iter_getp(cls->vtable.methods, mit); 
            gs_fprintln(fp, "\tvt->%s = %s;", mt->name, mt->function);
        }

        // Init base stuff
        gs_fprintln(fp, "\tvt->cls_id = %s_class_id;", cls->name); 
        gs_fprintln(fp, "\tvt->cls = %s_class;", cls->name); 
        gs_fprintln(fp, "\tvt->obj_init= %s_init;", cls->name); 
        gs_fprintln(fp, "\tvt->obj_dtor= %s_dtor;", cls->name); 

        gs_fprintln(fp, "}\n");

        // Ctor
        gs_fprintln(fp, "GS_API_DECL %s_vtable_t", cls->name);
        gs_fprintln(fp, "%s_vtable_t_ctor()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\t%s_vtable_t vt = gs_default_val();", cls->name);
        gs_fprintln(fp, "\t%s_vtable_t_init(&vt);", cls->name);
        gs_fprintln(fp, "\treturn vt;");
        gs_fprintln(fp, "}\n");

        // New
        gs_fprintln(fp, "GS_API_DECL gs_core_vtable_t*");
        gs_fprintln(fp, "%s_vtable_t_new()", cls->name);
        gs_fprintln(fp, "{");
        gs_fprintln(fp, "\t%s_vtable_t* vt = gs_malloc_init(%s_vtable_t);", 
                cls->name, cls->name); 
        gs_fprintln(fp, "\t%s_vtable_t_init(vt);", cls->name);
        gs_fprintln(fp, "\treturn (gs_core_vtable_t*)vt;");
        gs_fprintln(fp, "}\n");
    }

    // Registry
    gs_fprintln(fp, "//============[ Registry ]===============//\n");

    gs_fprintln(fp, "//Static Objects");

    // Classes
    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
        gs_fprintln(fp, "static %s g_%zu = {0};", cls->name, CID);
    }

    gs_fprintln(fp, ""); 

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_register()", proj_name);
    gs_fprintln(fp, "{");

    gs_fprintln(fp, "\tgs_core_meta_registry_t* meta = gs_core_instance()->meta;");
    gs_fprintln(fp, "\tgs_meta_class_t* cls = NULL;"); 
    gs_fprintln(fp, "\tgs_meta_class_t* base = NULL;");
    gs_fprintln(fp, "\tgs_core_meta_info_t* info = NULL;");
    gs_fprintln(fp, "\tgs_core_network_rpc_t* rpc = NULL;");
    gs_fprintln(fp, "\tuint64_t cid = 0;");
    gs_fprintln(fp, "\tuint32_t cnt = 0;");
    gs_fprintln(fp, "\tuint32_t idx = 0;\n"); 

    // Classes
    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    ) 
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 

        gs_fprintln(fp, "\t/* %s */", cls->name);
        gs_fprintln(fp, "\t{");

        gs_fprintln(fp, "\t\tcid = gs_meta_class_register(&meta->registry, (&(gs_meta_class_decl_t){");
        gs_fprintln(fp, "\t\t\t.name = gs_to_str(%s),", cls->name);
        gs_fprintf(fp, "\t\t\t.properties = (gs_meta_property_t[])");

        // Iterate through all properties
        const uint32_t psize = gs_dyn_array_size(cls->properties);
        if (psize)
        {
            gs_fprintf(fp, " {\n"); 

            // For each property
            for (uint32_t p = 0; p < gs_dyn_array_size(cls->properties); ++p)
            { 
                const char* type_info = "GS_META_PROPERTY_TYPE_INFO_OBJ";
                property_t* prop = &cls->properties[p];
                uint64_t type_hash = gs_hash_str64(prop->type);
                if (gs_hash_table_key_exists(meta->type_map, type_hash))
                {
                    type_info = gs_hash_table_get(meta->type_map, type_hash);
                }

                gs_fprintf(fp, "\t\t\t\tgs_meta_property(%s, %s, %s, %s)",
                    cls->name, 
                    prop->type, 
                    prop->name, 
                    type_info
                );

                if (p < psize - 1) gs_fprintf(fp, ",\n");
                else               gs_fprintln(fp, "");
            }
            gs_fprintln(fp, "\t\t\t},");
        }
        else
        {
            gs_fprintf(fp, "{0},\n"); 
        }

        gs_fprintln(fp, "\t\t\t.size = %zu * sizeof(gs_meta_property_t)", gs_dyn_array_size(cls->properties));
        gs_fprintln(fp, "\t\t}));");
        gs_fprintln(fp, "");

        gs_fprintln(fp, "\t\tgs_core_meta_info_t ci = {0};");
        gs_fprintln(fp, "\t\tuint32_t hndl = %s_class_id();", cls->name); 
        gs_fprintln(fp, "\t\tgs_core_meta_info_t* info = gs_slot_array_getp(meta->info, hndl);"); 
        gs_fprintln(fp, "\t\tgs_core_cast(&g_%zu, gs_core_base_t)->id = %s_class_id();", CID, cls->name);

        // If is rpc, then need to init rpc info
        if (gs_string_compare_equal(cls->base, "gs_core_network_rpc_t") && cls->methods)
        { 
            method_t* cb = gs_hash_table_getp(cls->methods, gs_hash_str64("_callback"));
            gs_snprintfc(CB_GEN, 128, "%s_cb", cls->name);
            gs_snprintfc(DELIVERY_STR, 128, "%s", cls->rpc.reliable ? "GS_CORE_NETWORK_DELIVERY_RELIABLE" : 
                    "GS_CORE_NETWORK_DELIVERY_UNRELIABLE");
            gs_snprintfc(CB_STR, 128, "%s", cb ? cb->content : CB_GEN);
            gs_fprintln(fp, "\t\trpc = core_cast(&g_%zu, gs_core_network_rpc_t);", CID);
            gs_fprintln(fp, "\t\trpc->delivery = %s;", DELIVERY_STR);
            gs_fprintln(fp, "\t\trpc->callback = %s;", CB_STR);
        }
        gs_fprintln(fp, "\t\tinfo->instance = gs_core_cast(&g_%zu, gs_core_obj_t);", CID);
        gs_fprintln(fp, "\t\tinfo->cls = gs_meta_class_get(&meta->registry, %s);", cls->name);
        gs_fprintln(fp, "\t\tinfo->cid = hndl;"); 

        // Set base class info
        if (cls->base && !gs_string_compare_equal(cls->base, "gs_core_obj_t") && 
                !gs_string_compare_equal(cls->name, "gs_core_obj_t"))
        {
            gs_fprintln(fp, "\t\tuint32_t bid = gs_core_cls_cid(%s);", cls->base); 
            gs_fprintln(fp, "\t\tinfo->base = gs_core_cls_info(%s);", cls->base); 
        }
        else
        { 
            gs_fprintln(fp, "\t\tinfo->base = NULL;"); 
        }
        
        // Vtable
        gs_fprintln(fp, "\t\tinfo->vtable = %s_vtable_t_new();", cls->name);
        gs_fprintln(fp, "\t\t%s_vtable_t_init(info->vtable);", cls->name);
        gs_fprintln(fp, "\t}\n");
    } 

    gs_fprintln(fp, "}\n");

    // Unregister

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_unregister()", proj_name); 
    gs_fprintln(fp, "{"); 
    gs_fprintln(fp, "\tgs_core_entities_t* ents = gs_core_instance()->entities;\n");

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    ) 
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 

        gs_fprintln(fp, "\t{");
        gs_fprintln(fp, "\t\tgs_core_meta_registry_t* meta = gs_core_instance()->meta;");
        gs_fprintln(fp, "\t\tgs_assert(meta);");
        gs_fprintln(fp, "\t\tgs_core_meta_info_t* info = gs_slot_array_getp(meta->info, %s_cls_id);", cls->name);
        gs_fprintln(fp, "\t\tgs_assert(info);");
        gs_fprintln(fp, "\t\tif (info->cls) {");
        gs_fprintln(fp, "\t\t\tgs_meta_class_unregister(&meta->registry, info->cls->id);");
        gs_fprintln(fp, "\t\t}");
        gs_fprintln(fp, "\t\tinfo->cls = NULL;");
        gs_fprintln(fp, "\t\tinfo->base = NULL;"); 
        gs_fprintln(fp, "\t\tif (info->vtable)"); 
        gs_fprintln(fp, "\t\t{"); 
        gs_fprintln(fp, "\t\t\tgs_free(info->vtable);"); 
        gs_fprintln(fp, "\t\t\tinfo->vtable = NULL;"); 
        gs_fprintln(fp, "\t\t}"); 
        if (gs_string_compare_equal(cls->base, "gs_core_entities_component_t"))
        {
            gs_fprintln(fp, "\t\tents->component_unregister(gs_core_entity_id(%s));", cls->name);
        } 
        if (gs_string_compare_equal(cls->base, "gs_core_entities_system_t"))
        {
            gs_fprintln(fp, "\t\tents->system_unregister(gs_core_entity_id(%s));", cls->name);
        }
        gs_fprintln(fp, "\t}"); 
    } 

    // Components

    // Systems

    gs_fprintln(fp, "}\n"); 

    gs_fprintln(fp, "//=====[ Component/System Registration ]=====//\n");

    gs_fprintln(fp, "GS_API_DECL void");
    gs_fprintln(fp, "%s_meta_ecs_register()", proj_name);
    gs_fprintln(fp, "{"); 

    gs_fprintln(fp, "\tgs_core_entities_t* ents = gs_core_instance()->entities;\n");

    gs_fprintln(fp, "\t// Components\n");

        for (
            gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
            gs_hash_table_iter_valid(meta->classes, it);
            gs_hash_table_iter_advance(meta->classes, it), CID++
        )
        {
            class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
            if (gs_string_compare_equal(cls->base, "gs_core_entities_component_t"))
            {
                gs_fprintln(fp, "\t{");
                gs_fprintln(fp, "\t\tgs_core_entity_t comp = ents->component_register(&(gs_core_entities_component_desc_t){", cls->name);
                gs_fprintln(fp, "\t\t\t.name = gs_to_str(%s),", cls->name);
                gs_fprintln(fp, "\t\t\t.size = sizeof(%s),", cls->name);
                gs_fprintln(fp, "\t\t\t.alignment = ECS_ALIGNOF(%s)", cls->name);
                gs_fprintln(fp, "\t\t});");
                gs_fprintln(fp, "\t\tgs_hash_table_insert(ents->components, gs_hash_str64(gs_to_str(%s)), comp);", cls->name);
                gs_fprintln(fp, "\t}");
            }
        }

    gs_fprintln(fp, "\n\t// Systems\n");

        for (
            gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
            gs_hash_table_iter_valid(meta->classes, it);
            gs_hash_table_iter_advance(meta->classes, it), CID++
        )
        {
            class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
            if (gs_string_compare_equal(cls->base, "gs_core_entities_system_t"))
            {
                gs_fprintln(fp, "\t/* %s */", cls->name);

                gs_snprintfc(CB, 256, "_%s_cb", cls->name);
                gs_fprintln(fp, "\tgs_core_entity_id(%s) = ents->system_register(&(gs_core_entities_system_desc_t){", cls->name);
                gs_fprintln(fp, "\t\t.name = gs_to_str(%s),", cls->name); 
                gs_fprintln(fp, "\t\t.callback = %s,", CB); 
                gs_fprintln(fp, "\t\t.filter.component_list = {");

                    for (uint32_t i = 0; i < gs_dyn_array_size(cls->properties); ++i)
                    { 
                        property_t* p = &cls->properties[i];
                        uint32_t strlen = gs_string_length(p->type) - 1;
                        gs_fprintf(fp, "\t\t\tgs_hash_table_get(ents->components, gs_hash_str64(gs_to_str(%.*s)))", strlen, p->type);
                        if (i < gs_dyn_array_size(cls->properties) - 1)
                        {
                            gs_fprintln(fp, ",");
                        }
                        else
                        {
                            gs_fprintln(fp, "");
                        }
                    }

                gs_fprintln(fp, "\t\t}");
                gs_fprintln(fp, "\t});");
            }
        }

    gs_fprintln(fp, "}\n");

    // ECS Systems

    gs_fprintln(fp, "//======[ Systems ]======//\n");

    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(meta->classes), CID = 1; 
        gs_hash_table_iter_valid(meta->classes, it);
        gs_hash_table_iter_advance(meta->classes, it), CID++
    )
    {
        class_t* cls = gs_hash_table_iter_getp(meta->classes, it); 
        if (gs_string_compare_equal(cls->base, "gs_core_entities_system_t"))
        { 
            gs_snprintfc(CB, 256, "_%s_cb(gs_core_entity_t* iter)", cls->name);
            gs_fprintln(fp, "GS_API_DECL void");
            gs_fprintln(fp, "%s", CB);
            gs_fprintln(fp, "{");

            gs_fprintln(fp, "\tgs_core_app_t* app = gs_core_app_instance();");
            gs_fprintln(fp, "\t%s sdata = {._base.iter = iter};", cls->name);
            gs_fprintln(fp, "\tgs_core_cls_init(%s, &sdata);", cls->name);
            gs_fprintln(fp, "\tgs_core_cast(&sdata, gs_core_entities_system_t)->tick |= GS_CORE_ENTITIES_TICK_ON_PLAY;");

            for (uint32_t i = 0; i < gs_dyn_array_size(cls->properties); ++i)
            {
                property_t* p = &cls->properties[i];
                uint32_t strlen = gs_string_length(p->type) - 1;
                gs_fprintln(fp, "\tsdata.%s = gs_core_entities_term(iter, %.*s, %zu);", 
                    p->name, strlen, p->type, i);
            } 

            // Get tick rate, compare with application 
            gs_fprintln(fp, "\tgs_core_entities_system_tick_flags tick = gs_core_cast(&sdata, gs_core_entities_system_t)->tick;");
            gs_fprintln(fp, "\tbool can_tick = (tick & GS_CORE_ENTITIES_TICK_ALWAYS) ||");
            gs_fprintln(fp, "\t\t(tick & GS_CORE_ENTITIES_TICK_ON_PLAY && app->state == GS_CORE_APP_STATE_PLAYING) ||");
            gs_fprintln(fp, "\t\t(tick & GS_CORE_ENTITIES_TICK_ON_PAUSE && app->state == GS_CORE_APP_STATE_PAUSED) ||");
            gs_fprintln(fp, "\t\t(tick & GS_CORE_ENTITIES_TICK_ON_STOP && app->state == GS_CORE_APP_STATE_STOPPED);");

            // Get vtable, then do callback, if available
            gs_fprintln(fp, "\tif (can_tick && gs_core_cast_vt(&sdata, %s)->callback)", cls->name);
            gs_fprintln(fp, "\t{");
            gs_fprintln(fp, "\t\tgs_core_cast_vt(&sdata, %s)->callback(&sdata);", cls->name);
            gs_fprintln(fp, "\t}");
            gs_fprintln(fp, "}");

        }
    } 

    fclose(fp);
}

// Generate both core and app structures, functions, meta data































