/*==============================================================================================================
    * Copyright: 2022 John Jackson
    * File: proj_gen.c

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
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
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

#ifdef _WIN32
#include <windows.h>
#endif

/* Maximum allowed project-name length (arbitrary safety ceiling). */
#define PROJ_NAME_MAX_LEN   256

/** Path-buffer size large enough for any combination of root + sub-path
 *  plus a small margin for separators and filename components. */
#define PATH_BUF_SIZE       (MAX_PATH + 128)

/* ---------------------------------------------------------------------------
 * Global path state – populated once during startup.
 * ---------------------------------------------------------------------------*/

/** Project name supplied by the caller (default "app"). */
char proj_name[PROJ_NAME_MAX_LEN] = {0};

/** Absolute path to the gs_core repository root directory.
 *  Populated during startup by either detecting it from the
 *  executable's location or accepting it via a -gcs <path> argument. */
char gs_core_root[PATH_BUF_SIZE];

/** Absolute path to the proj_gen template source tree inside gs_core.
 *  Computed once gs_core_root is known. */
char proj_gen_src_dir[PATH_BUF_SIZE];

/** Absolute path to the directory where generated projects will reside.
 *  Defaults to the parent of gs_core_root; overridden via --dir. */
char project_dir[PATH_BUF_SIZE] = {0};

/* Derived output paths – all computed from gs_core_root so that the tool
 * operates correctly regardless of the invocation working directory. */
char ROOT_DIR[PATH_BUF_SIZE];
char SRC_DIR[PATH_BUF_SIZE];
char EDITOR_SRC_DIR[PATH_BUF_SIZE];
char ASSET_DIR[PATH_BUF_SIZE];
char ASSET_TEXTURE_DIR[PATH_BUF_SIZE];
char ASSET_PIPELINE_DIR[PATH_BUF_SIZE];
char PROC_DIR[PATH_BUF_SIZE];
char PROC_WIN_DIR[PATH_BUF_SIZE];
char PROJ_SRC[PATH_BUF_SIZE];
char PROJ_HDR[PATH_BUF_SIZE];
char PROJ_UNITY_SRC[PATH_BUF_SIZE];
char EDITOR_HDR[PATH_BUF_SIZE];
char EDITOR_SRC[PATH_BUF_SIZE];
char EDITOR_UNITY_SRC[PATH_BUF_SIZE];

/* ---------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------------*/

static void write_template_to_disk(const char* read_path, const char* write_path);
static void write_assets_to_disk(const char* read_path, const char* write_path);
static bool ensure_trailing_slash(char* path, size_t cap);

/* ===========================================================================
 * Logging helper – uses fprintf(stderr) directly because gs_log_* asserts
 * on failure which can crash when the framework is still initializing.
 * ===========================================================================*/

static void log_err(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fputs("ERROR: ", stderr);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
}

static void log_warn(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fputs("WARNING: ", stderr);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
}

/* ===========================================================================
 * Path helpers – no ".." string literals anywhere below.
 * ===========================================================================*/

/** Copy |src| into |dst| (max |dst_cap|), guaranteeing NUL-termination.
 *  Returns true on success. */
static bool safe_strcpy(char* dst, size_t dst_cap, const char* src)
{
    if (!src || !dst || dst_cap == 0) return false;
    strncpy_s(dst, dst_cap, src, _TRUNCATE);
    return true;
}

/** Concatenate |a| and |b| into |dst| (max |dst_cap|), inserting no separator.
 *  Returns true on success. */
static bool str_concat(char* dst, size_t dst_cap, const char* a, const char* b)
{
    if (!safe_strcpy(dst, dst_cap, a)) return false;
    size_t len = strlen(dst);
    size_t blen = strlen(b);
    if (len + blen >= dst_cap) return false;
    memcpy(dst + len, b, blen + 1);
    return true;
}

/** Strip the last component off |full| so that it refers to the parent dir.
 *  Returns true on success, false if |full| cannot be trimmed further. */
static bool strip_last_component(char* full)
{
    size_t len = strlen(full);
    /* Find last '\' */
    while (len > 0 && full[len - 1] != '\\') --len;
    if (len == 0) return false;
    full[len] = '\0';
    return true;
}

/** Walk upwards from |exe_dir| looking for a directory named "third_party".
 *  When found, the directory containing "third_party" is returned as the
 *  gs_core root in |out|.  Returns true on success. */
static bool detect_gs_core_root_from_exe(const char* exe_dir, char* out, size_t out_sz)
{
    if (!safe_strcpy(out, out_sz, exe_dir)) return false;

    /* Keep climbing until we find third_party or run out of parents. */
    for (int depth = 0; depth < 32; ++depth)
    {
        char candidate[PATH_BUF_SIZE];
        /* Normalise |out| to have exactly one trailing backslash, then append
         * the target directory name – avoids doubled separators. */
        ensure_trailing_slash(out, out_sz);
        snprintf(candidate, sizeof(candidate), "%sthird_party", out);

        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA(candidate, &fd);
        if (h != INVALID_HANDLE_VALUE)
        {
            FindClose(h);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                return true;  /* found it */
        }
        /* Not here – go up one level */
        if (!strip_last_component(out)) break;
    }
    return false;
}

/** Ensure |path| ends with exactly one trailing backslash.
 *  Modifies in-place; returns true on success. */
static bool ensure_trailing_slash(char* path, size_t cap)
{
    size_t len = strlen(path);
    if (len == 0) return false;
    if (path[len - 1] == '\\' || path[len - 1] == '/') return true;

    if (len + 1 >= cap) return false;
    path[len] = '\\';
    path[len + 1] = '\0';
    return true;
}

/** Compute every derived path variable from project_dir and proj_gen_src_dir.
 *  ROOT_DIR is now built directly from project_dir rather than climbing up
 *  from gs_core_root. */
static void compute_paths(void)
{
    /* Ensure project_dir ends with backslash. */
    ensure_trailing_slash(project_dir, sizeof(project_dir));

    /* Build proj_gen_src_dir relative to gs_core_root. */
    ensure_trailing_slash(gs_core_root, sizeof(gs_core_root));
    str_concat(proj_gen_src_dir, sizeof(proj_gen_src_dir),
               gs_core_root, "source\\proj_gen");

    /* User-specified base dir + project name = actual ROOT_DIR */
    safe_strcpy(ROOT_DIR, sizeof(ROOT_DIR), project_dir);
    size_t rlen = strlen(ROOT_DIR);
    size_t nlen = strlen(proj_name);
    if (rlen + nlen >= sizeof(ROOT_DIR)) { /* overflow guard */ }
    else { memcpy(ROOT_DIR + rlen, proj_name, nlen + 1); }
    ensure_trailing_slash(ROOT_DIR, sizeof(ROOT_DIR));

    snprintf(SRC_DIR,         sizeof(SRC_DIR),         "%ssource\\",     ROOT_DIR);
    snprintf(EDITOR_SRC_DIR,  sizeof(EDITOR_SRC_DIR),  "%seditor\\",    SRC_DIR);
    snprintf(ASSET_DIR,       sizeof(ASSET_DIR),       "%sassets\\",    ROOT_DIR);
    snprintf(ASSET_TEXTURE_DIR, sizeof(ASSET_TEXTURE_DIR), "%stextures\\", ASSET_DIR);
    snprintf(ASSET_PIPELINE_DIR, sizeof(ASSET_PIPELINE_DIR), "%spipelines\\", ASSET_DIR);
    snprintf(PROC_DIR,        sizeof(PROC_DIR),        "%sproc\\",      ROOT_DIR);
    snprintf(PROC_WIN_DIR,    sizeof(PROC_WIN_DIR),    "%swin\\",       PROC_DIR);

    /* Source files live under <project>/source/ */
    snprintf(PROJ_SRC,      sizeof(PROJ_SRC),      "%s%s.c", SRC_DIR, proj_name);
    snprintf(PROJ_HDR,      sizeof(PROJ_HDR),      "%s%s.h", SRC_DIR, proj_name);
    snprintf(PROJ_UNITY_SRC,sizeof(PROJ_UNITY_SRC),"%s_unity.c", PROJ_SRC);
    snprintf(EDITOR_HDR,    sizeof(EDITOR_HDR),    "%s%s_editor.h", EDITOR_SRC_DIR, proj_name);
    snprintf(EDITOR_SRC,    sizeof(EDITOR_SRC),    "%s%s_editor.c", EDITOR_SRC_DIR, proj_name);
    snprintf(EDITOR_UNITY_SRC, sizeof(EDITOR_UNITY_SRC), "%s_unity.c", EDITOR_SRC);
}

/* Resolve the absolute path of the calling executable's directory into |buf|. */
static void get_exe_directory(char* buf, size_t bufsz)
{
#ifdef _WIN32
    char raw[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, raw, sizeof(raw));
    if (n == 0 || n >= sizeof(raw)) { buf[0] = '\0'; return; }

    /* Make sure it's canonical */
    char resolved[MAX_PATH];
    if (GetFullPathNameA(raw, sizeof(resolved), resolved, NULL) == 0) { buf[0] = '\0'; return; }

    /* Copy into buf first, then chop off the filename */
    safe_strcpy(buf, bufsz, resolved);
    strip_last_component(buf);
#else
#error "proj_gen requires Windows (uses GetModuleFileNameA)."
#endif
}

/* ===========================================================================
 * Main entry point
 * ===========================================================================*/

int32_t main(int32_t argc, char** argv)
{
    /* Initialize the Gunslinger platform so gs_platform_* APIs work. */
    gs_platform_t* plat = gs_platform_create();
    if (!plat) {
        fprintf(stderr, "ERROR: gs_platform_create() failed.\n");
        return -1;
    }
    gs_platform_init(plat);

    /* ---- Parse arguments -------------------------------------------------- */
    if (argc > 1)
    {
        int arg_off = 1;

        /* Optional -gcs / --gs-core <gs_core_root> switch. */
        if (strcmp(argv[arg_off], "-gcs") == 0 || strcmp(argv[arg_off], "--gs-core") == 0)
        {
            if (arg_off + 1 >= argc)
            {
                log_err("%s requires a gs_core root path argument.", argv[arg_off]);
                goto cleanup;
            }
            /* Accept the path without strict file-system validation. */
            safe_strcpy(gs_core_root, sizeof(gs_core_root), argv[arg_off + 1]);
            ensure_trailing_slash(gs_core_root, sizeof(gs_core_root));
            arg_off += 2;
        }
        else
        {
            /* Auto-detect gs_core root from executable location. */
            char exe_dir[PATH_BUF_SIZE];
            get_exe_directory(exe_dir, sizeof(exe_dir));

            if (!detect_gs_core_root_from_exe(exe_dir, gs_core_root, sizeof(gs_core_root)))
            {
                log_err("Could not auto-detect gs_core root. Use -gcs <path>.");
                goto cleanup;
            }
        }

        /* Optional --dir <project_dir> switch. */
        if (arg_off < argc && strcmp(argv[arg_off], "--dir") == 0)
        {
            if (arg_off + 1 >= argc)
            {
                log_err("--dir requires a directory path argument.");
                goto cleanup;
            }
            /* Safely copy the supplied path and ensure a trailing slash. */
            safe_strcpy(project_dir, sizeof(project_dir), argv[arg_off + 1]);
            ensure_trailing_slash(project_dir, sizeof(project_dir));
            arg_off += 2;
        }
        else
        {
            /* Fallback: sibling directory of gs_core_root. */
            char tmp[PATH_BUF_SIZE];
            safe_strcpy(tmp, sizeof(tmp), gs_core_root);
            strip_last_component(tmp);
            strip_last_component(tmp);
            /* Trim any leftover trailing separators. */
            while (strlen(tmp) > 0 && (tmp[strlen(tmp) - 1] == '\\' || tmp[strlen(tmp) - 1] == '/'))
                tmp[strlen(tmp) - 1] = '\0';
            safe_strcpy(project_dir, sizeof(project_dir), tmp);
            ensure_trailing_slash(project_dir, sizeof(project_dir));
        }

        /* Non-flag argument after switches is the project name. */
        if (arg_off < argc)
        {
            const char *src = argv[arg_off];
            size_t slen = strlen(src);
            if (slen >= PROJ_NAME_MAX_LEN) slen = PROJ_NAME_MAX_LEN - 1;
            memcpy(proj_name, src, slen);
            proj_name[slen] = '\0';
        }
        else
        {
            gs_snprintf(proj_name, sizeof(proj_name), "%s", "app");
        }
    }
    else
    {
        /* No args at all – try auto-detection, default project name "app". */
        char exe_dir[PATH_BUF_SIZE];
        get_exe_directory(exe_dir, sizeof(exe_dir));

        if (!detect_gs_core_root_from_exe(exe_dir, gs_core_root, sizeof(gs_core_root)))
        {
            log_err("Could not auto-detect gs_core root. Provide a path or use -gcs <path>.");
            goto cleanup;
        }
        gs_snprintf(proj_name, sizeof(proj_name), "%s", "app");
    }

    gs_println("PROJECT NAME: %s", proj_name);
    gs_println("GS_CORE_ROOT: %s", gs_core_root);
    gs_println("PROJECT_DIR: %s", project_dir);

    /* Derive all output paths from project_dir. */
    compute_paths();

    /* Ensure the parent of project_dir and project_dir itself exist. */
    {
        char parent[PATH_BUF_SIZE];
        safe_strcpy(parent, sizeof(parent), project_dir);
        strip_last_component(parent);  /* Strip trailing '\\' → project dir name */
        strip_last_component(parent);  /* Strip project dir name → actual parent */
        gs_platform_mkdir(parent, 0x00);
    }
    gs_platform_mkdir(project_dir, 0x00);

    if (gs_platform_dir_exists(SRC_DIR))
    {
        log_warn("Project already exists!: %s (directory %s)", proj_name, SRC_DIR);
        goto cleanup;
    }

    /* Create directories */
    gs_platform_mkdir(ROOT_DIR, 0x00);
    gs_platform_mkdir(SRC_DIR, 0x00);
    gs_platform_mkdir(EDITOR_SRC_DIR, 0x00);
    gs_platform_mkdir(PROC_DIR, 0x00);
    gs_platform_mkdir(PROC_WIN_DIR, 0x00);
    gs_platform_mkdir(ASSET_DIR, 0x00);
    gs_platform_mkdir(ASSET_TEXTURE_DIR, 0x00);
    gs_platform_mkdir(ASSET_PIPELINE_DIR, 0x00);

    /* Load up templates */
    struct {const char* read_path; const char* subdir; bool substitute;} templates[] =
    {
        {"templates/proc/win/cl.bat",              "proc/win",     false},
        {"templates/proc/win/cl_dll.bat",          "proc/win",     false},
        {"templates/proc/win/editor_cl.bat",       "proc/win",     false},
        {"templates/app/app.h",                    "source",       true },
        {"templates/app/app.c",                    "source",       true },
        {"templates/app/unity.c",                  "source",       true },
        {"templates/editor/editor.c",              "source/editor",true },
        {"templates/editor/editor.h",              "source/editor",true },
        {"templates/editor/unity.c",               "source/editor",true },
        {NULL}
    };

    /* Write templates out to disk */
    for (uint32_t i = 0; templates[i].read_path != NULL; ++i)
    {
        char wp[PATH_BUF_SIZE];
        char rp[PATH_BUF_SIZE];
        
        /* Extract base filename from read path */
        const char* fname = strrchr(templates[i].read_path, '\\');
        if (!fname) fname = strrchr(templates[i].read_path, '/');
        if (!fname) fname = templates[i].read_path;
        else fname++; /* skip past delimiter */
        
        /* Determine output filename by substituting the template prefix with proj_name */
        char out_fname[256] = {0};
        if (templates[i].substitute) {
            /* Classify by template subdirectory */
            const char* tmpl_dir = strrchr(templates[i].read_path, '\\');
            if (!tmpl_dir) tmpl_dir = strrchr(templates[i].read_path, '/');
            if (!tmpl_dir) tmpl_dir = templates[i].read_path;
            else tmpl_dir++; /* skip past delimiter */
            
            /* Get parent dir from read_path (e.g., "templates/app/" -> "app") */
            char tmpl_parent[128] = {0};
            {
                char tmp[PATH_BUF_SIZE];
                safe_strcpy(tmp, sizeof(tmp), templates[i].read_path);
                char* last_slash = strrchr(tmp, '\\');
                if (!last_slash) last_slash = strrchr(tmp, '/');
                if (last_slash) *last_slash = '\0';
                /* Now tmp is like "templates/app" - extract basename */
                char* base = strrchr(tmp, '\\');
                if (!base) base = strrchr(tmp, '/');
                if (base) base++;
                else base = tmp;
                safe_strcpy(tmpl_parent, sizeof(tmpl_parent), base);
            }
            
            /* Build output name based on template dir + filename */
            if (strncmp(tmpl_parent, "app", 3) == 0) {
                /* app/*.h -> <proj>.h
                   app/*.c -> <proj>.c
                   app/unity.c -> <proj>_unity.c */
                if (strncmp(fname, "unity", 5) == 0) {
                    snprintf(out_fname, sizeof(out_fname), "%s_unity.c", proj_name);
                } else {
                    /* Replace "app" prefix with proj_name, keep extension (including dot) */
                    size_t ext_pos = strcspn(fname, ".");
                    snprintf(out_fname, sizeof(out_fname), "%s%s", proj_name, fname + ext_pos);
                }
            } else if (strncmp(tmpl_parent, "editor", 6) == 0) {
                /* editor/*.c -> <proj>_editor.c
                   editor/*.h -> <proj>_editor.h
                   editor/unity.c -> <proj>_editor_unity.c */
                if (strncmp(fname, "unity", 5) == 0) {
                    snprintf(out_fname, sizeof(out_fname), "%s_editor_unity.c", proj_name);
                } else {
                    /* Replace "editor" prefix with proj_name+_editor, keep extension */
                    size_t ext_pos = strcspn(fname, ".");
                    snprintf(out_fname, sizeof(out_fname), "%s_editor%s", proj_name, fname + ext_pos);
                }
            } else {
                /* Unknown template dir — keep original name */
                safe_strcpy(out_fname, sizeof(out_fname), fname);
            }
        } else {
            /* .bat files keep their original names */
            safe_strcpy(out_fname, sizeof(out_fname), fname);
        }
        
        /* Build full write path: ROOT_DIR/subdir/out_fname */
        snprintf(wp, sizeof(wp), "%s%s\\%s", ROOT_DIR, templates[i].subdir, out_fname);
        snprintf(rp, sizeof(rp), "%s\\%s", proj_gen_src_dir, templates[i].read_path);
        write_template_to_disk(rp, wp);
    }

    /* Load up assets to simply copy over */
    struct {const char* read_path; const char* write_path;} assets[] =
    {
        {.read_path = "templates/assets/textures/gs.png", .write_path = "assets/textures/gs.png"},
        {.read_path = "templates/assets/pipelines/simple.sf", .write_path = "assets/pipelines/simple.sf"},
        {NULL}
    };

    /* Write assets to disk */
    for (uint32_t i = 0; assets[i].read_path != NULL; ++i)
    {
        char wp[PATH_BUF_SIZE];
        char rp[PATH_BUF_SIZE];
        snprintf(wp, sizeof(wp), "%s\\%s", ROOT_DIR, assets[i].write_path);
        snprintf(rp, sizeof(rp), "%s\\%s", proj_gen_src_dir, assets[i].read_path);
        write_assets_to_disk(rp, wp);
    }

    gs_println("Project '%s' generated successfully at %s", proj_name, ROOT_DIR);

cleanup:
    /* Shutdown framework */
    gs_platform_shutdown(plat);
    gs_platform_destroy(plat);

    return 0;
}

/* ===========================================================================
 * Template processing – replaces %APP%, %APP_SOURCE_PATH%, etc. tokens.
 * ===========================================================================*/

#define REPLACE_TXT(REPLACE, IS_STR) \
    do { \
        token = lex.next_token(&lex); \
        token = lex.next_token(&lex); \
        if (IS_STR) \
            gs_fprintf(fp, "\"%s\"", REPLACE); \
        else \
            gs_fprintf(fp, "%s", REPLACE); \
    } while (0)\

void write_template_to_disk(const char* read_path, const char* write_path)
{
    FILE* fp = NULL;
    
    /* Open destination file */
    fp = fopen(write_path, "wb");
    if (!fp) {
        log_err("fopen failed for write_path: %s", write_path);
        return;
    }

    char* contents = gs_platform_read_file_contents(read_path, "rb", NULL);
    if (!contents) {
        log_err("Failed to read template: %s", read_path);
        fclose(fp);
        return;
    }

    /* Lex through contents, look for %% identifier to replace */
    gs_lexer_t lex = gs_lexer_c_ctor(contents);
    lex.skip_white_space = false;
    while (lex.can_lex(&lex))
    {
        gs_token_t token = lex.next_token(&lex);
        switch (token.type)
        {
            case GS_TOKEN_PERCENT:
            {
                gs_token_t peek = gs_lexer_peek(&lex);

                /* Look for next token to be identifier */
                if (gs_token_compare_text(&peek, "APP"))
                {
                    REPLACE_TXT(proj_name, false);
                }
                else if (gs_token_compare_text(&peek, "APP_SOURCE_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "%s.c", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "APP_HEADER_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "%s.h", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "GENERATED_SOURCE_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "generated/%s_generated.c", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "GENERATED_HEADER_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "generated/%s_generated.h", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_SOURCE_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "%s_editor.c", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_HEADER_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "%s_editor.h", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_GENERATED_HEADER_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "generated/%s_editor_generated.h", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_GENERATED_SOURCE_PATH"))
                {
                    char tmp[PATH_BUF_SIZE];
                    snprintf(tmp, sizeof(tmp), "generated/%s_editor_generated.c", proj_name);
                    REPLACE_TXT(tmp, true);
                }
                else if (gs_token_compare_text(&peek, "GS_CORE_DIR"))
                {
                    REPLACE_TXT(gs_core_root, true);
                }
                else if (gs_token_compare_text(&peek, "ROOT_DIR"))
                {
                    REPLACE_TXT(ROOT_DIR, true);
                }
                else
                {
                    gs_fprintf(fp, "%.*s", token.len, token.text);
                }
            } break;

            default:
            {
                gs_fprintf(fp, "%.*s", token.len, token.text);
            } break;
        }
    }

    gs_free(contents);
    fclose(fp);
}

void write_assets_to_disk(const char* read_path, const char* write_path)
{
    gs_platform_file_copy(read_path, write_path);
}
