/*==============================================================================================================
    * Copyright: 2022 John Jackson 
/   * File: proj_gen.c

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

char proj_name[256] = {0}; 

void write_template_to_disk(const char* read_path, const char* write_path);

int32_t main(int32_t argc, char** argv)
{ 
    if (argc > 1)
    { 
        gs_snprintf(proj_name, sizeof(proj_name), "%s", argv[1]);
    }
    else
    {
        gs_snprintf(proj_name, sizeof(proj_name), "%s", "app");
    }

    gs_println("PROJECT NAME: %s", proj_name);

    // Project directory, right above gs_core
    gs_snprintfc(ROOT_DIR, 256, "../../../%s", proj_name);
    gs_snprintfc(SRC_DIR, 256, "%s/source", ROOT_DIR);
    gs_snprintfc(EDITOR_SRC_DIR, 256, "%s/source/editor", ROOT_DIR);
    gs_snprintfc(ASSET_DIR, 256, "%s/assets", ROOT_DIR);
    gs_snprintfc(PROC_DIR, 256, "%s/proc", ROOT_DIR);
    gs_snprintfc(PROC_WIN_DIR, 256, "%s/win", PROC_DIR);
    gs_snprintfc(PROJ_SRC, 256, "source/%s.c", proj_name);
    gs_snprintfc(PROJ_HDR, 256, "source/%s.h", proj_name);
    gs_snprintfc(EDITOR_HDR, 256, "source/editor/%s_editor.h", proj_name);
    gs_snprintfc(EDITOR_SRC, 256, "source/editor/%s_editor.c", proj_name);

    if (gs_platform_dir_exists(ROOT_DIR))
    {
        gs_log_warning("Proj Gen: Project already exists!: %s", proj_name);
        return -1;
    }

    // Directories
    gs_platform_mkdir(ROOT_DIR, 0x00); 
    gs_platform_mkdir(SRC_DIR, 0x00);
    gs_platform_mkdir(EDITOR_SRC_DIR, 0x00);
    gs_platform_mkdir(PROC_DIR, 0x00);
    gs_platform_mkdir(PROC_WIN_DIR, 0x00);
    gs_platform_mkdir(ASSET_DIR, 0x00); 

    // Load up templates 
    struct {const char* read_path; const char* write_path;} templates[] = 
    {
        {.read_path = "templates/proc/win/cl.bat", .write_path = "proc/win/cl.bat"},
        {.read_path = "templates/proc/win/cl_dll.bat", .write_path = "proc/win/cl_dll.bat"},
        {.read_path = "templates/proc/win/editor_cl.bat", .write_path = "proc/win/editor_cl.bat"},
        {.read_path = "templates/app/app.h", .write_path = PROJ_HDR},
        {.read_path = "templates/app/app.c", .write_path = PROJ_SRC},
        {.read_path = "templates/app/unity.c", .write_path = "source/unity.c"},
        {.read_path = "templates/editor/editor.c", .write_path = EDITOR_SRC},
        {.read_path = "templates/editor/editor.h", .write_path = EDITOR_HDR},
        {.read_path = "templates/editor/unity.c", .write_path = "source/editor/unity.c"},
        {NULL}
    };

    // Write templates out to disk 
    for (uint32_t i = 0; templates[i].read_path != NULL; ++i)
    {
        gs_snprintfc(WP, 256, "%s/%s", ROOT_DIR, templates[i].write_path);
        gs_snprintfc(RP, 256, "../../source/proj_gen/%s", templates[i].read_path);
        write_template_to_disk(RP, WP); 
    }
}

#define REPLACE_TXT(REPLACE, IS_STR)\
    do {\
        /* Eat app identifier token */\
        token = lex.next_token(&lex);\
\
        /* Eat next percent token */\
        token = lex.next_token(&lex);\
\
        if (IS_STR)\
            gs_fprintf(fp, "\"%s\"", REPLACE);\
        else\
            gs_fprintf(fp, "%s", REPLACE);\
    } while (0)

void write_template_to_disk(const char* read_path, const char* write_path)
{
    FILE* fp = NULL; 
    char* contents = gs_platform_read_file_contents(read_path, "rb", NULL);
    gs_println("write path: %s", write_path);
    fp = fopen(write_path, "wb");
    gs_assert(fp); 

    // Lex through contents, look for %APP% identifier to replace
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

                // Look for next token to be identifier
                if (gs_token_compare_text(&peek, "APP")) 
                {
                    REPLACE_TXT(proj_name, false);
                }
                else if (gs_token_compare_text(&peek, "APP_SOURCE_PATH"))
                {
                    gs_snprintfc(TMP, 256, "%s.c", proj_name);
                    REPLACE_TXT(TMP, true);
                }
                else if (gs_token_compare_text(&peek, "APP_HEADER_PATH"))
                {
                    gs_snprintfc(TMP, 256, "%s.h", proj_name);
                    REPLACE_TXT(TMP, true);
                }
                else if (gs_token_compare_text(&peek, "GENERATED_SOURCE_PATH")) 
                { 
                    gs_snprintfc(TMP, 256, "generated/%s_generated.c", proj_name);
                    REPLACE_TXT(TMP, true);
                }
                else if (gs_token_compare_text(&peek, "GENERATED_HEADER_PATH"))
                {
                    gs_snprintfc(TMP, 256, "generated/%s_generated.h", proj_name);
                    REPLACE_TXT(TMP, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_SOURCE_PATH"))
                {
                    gs_snprintfc(TMP, 256, "editor/%s_editor.c", proj_name);
                    REPLACE_TXT(TMP, true);
                }
                else if (gs_token_compare_text(&peek, "EDITOR_HEADER_PATH"))
                {
                    gs_snprintfc(TMP, 256, "editor/%s_editor.h", proj_name);
                    REPLACE_TXT(TMP, true);
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

    fclose(fp);
}

















