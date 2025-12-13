/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_code.c
    * 
    * Code Search Tools for AI Coordinator - Implementation

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2025 John Jackson

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

/*================================================================================
// Includes
================================================================================*/

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2/pcre2.h>

#include "gs_core_ai_tools_code.h"
#include <cJSON/cJSON.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>

// For directory operations on Windows
#ifdef _WIN32
    #include <gs/external/dirent/dirent.h>
#else
    #include <dirent.h>
#endif

/*================================================================================
// Constants
================================================================================*/

#define GS_CORE_AI_TOOLS_CODE_MAX_LINE_LENGTH 8192
#define GS_CORE_AI_TOOLS_CODE_MAX_RESULTS_DEFAULT 100
#define GS_CORE_AI_TOOLS_CODE_MAX_CONTEXT_LINES 10
#define GS_CORE_AI_TOOLS_CODE_MAX_FILE_SIZE (10 * 1024 * 1024) // 10MB

/*================================================================================
// Helper Functions
================================================================================*/

static bool _code_is_path_safe(const char* path) {
    // Basic path safety check - prevent path traversal attacks
    if (!path || strlen(path) == 0) return false;
    
    // Check for suspicious patterns
    if (strstr(path, "..") != NULL) return false;
    
    return true;
}

static bool _match_extension(const char* filename, const char* filter) {
    if (!filter || strlen(filter) == 0) return true;
    if (!filename) return false;
    
    // Filter can be comma-separated list: ".c,.h,.cpp"
    const char* ext_start = strrchr(filename, '.');
    if (!ext_start) return false;
    
    // Check each extension in filter
    char filter_copy[256];
    strncpy(filter_copy, filter, sizeof(filter_copy) - 1);
    filter_copy[sizeof(filter_copy) - 1] = '\0';
    
    char* token = strtok(filter_copy, ",");
    while (token) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        
        if (strcmp(ext_start, token) == 0) {
            return true;
        }
        token = strtok(NULL, ",");
    }
    
    return false;
}

static bool _is_source_file(const char* filename) {
    // Check if file is a source file
    return _match_extension(filename, ".c,.h,.cpp,.hpp,.cc,.cxx,.hxx");
}

static void _escape_json_string(const char* src, char* dst, size_t dst_size) {
    size_t j = 0;
    for (size_t i = 0; src[i] && j < dst_size - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            if (j < dst_size - 3) {
                dst[j++] = '\\';
                dst[j++] = src[i];
            }
        } else if (src[i] == '\n') {
            if (j < dst_size - 3) {
                dst[j++] = '\\';
                dst[j++] = 'n';
            }
        } else if (src[i] == '\r') {
            if (j < dst_size - 3) {
                dst[j++] = '\\';
                dst[j++] = 'r';
            }
        } else if (src[i] == '\t') {
            if (j < dst_size - 3) {
                dst[j++] = '\\';
                dst[j++] = 't';
            }
        } else {
            dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

/*================================================================================
// Tool Implementation: search_code
================================================================================*/

typedef struct {
    cJSON* results_array;
    pcre2_code* regex;
    size_t* total_matches;
    bool* truncated;
    size_t max_results;
    size_t context_lines;
} search_code_context_t;

static void _search_code_in_file(const char* filepath, search_code_context_t* ctx) {
    if (*ctx->total_matches >= ctx->max_results) {
        *ctx->truncated = true;
        return;
    }
    
    // Read file
    size_t file_size = 0;
    char* content = gs_platform_read_file_contents(filepath, "rb", &file_size);
    if (!content) return;
    
    // Skip files that are too large
    if (file_size > GS_CORE_AI_TOOLS_CODE_MAX_FILE_SIZE) {
        gs_free(content);
        return;
    }
    
    // Split into lines
    char* line_start = content;
    size_t line_num = 1;
    
    // Store lines for context
    #define MAX_STORED_LINES 1000
    char* lines[MAX_STORED_LINES];
    size_t num_lines = 0;
    
    // First pass: split into lines
    char* current = content;
    lines[num_lines++] = line_start;
    
    while (*current && num_lines < MAX_STORED_LINES) {
        if (*current == '\n') {
            *current = '\0';
            if (*(current + 1)) {
                lines[num_lines++] = current + 1;
            }
        }
        current++;
    }
    
    // Create match data
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(ctx->regex, NULL);
    
    // Search each line
    for (size_t i = 0; i < num_lines && *ctx->total_matches < ctx->max_results; i++) {
        const char* line = lines[i];
        size_t line_len = strlen(line);
        
        // Try to match
        int rc = pcre2_match(
            ctx->regex,
            (PCRE2_SPTR)line,
            line_len,
            0,
            0,
            match_data,
            NULL
        );
        
        if (rc >= 0) {
            // Match found
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
            
            cJSON* result_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(result_obj, "file", filepath);
            cJSON_AddNumberToObject(result_obj, "line", (double)(i + 1));
            cJSON_AddNumberToObject(result_obj, "column", (double)ovector[0]);
            
            // Extract matched text
            size_t match_len = ovector[1] - ovector[0];
            char match_text[256];
            size_t copy_len = match_len < sizeof(match_text) - 1 ? match_len : sizeof(match_text) - 1;
            memcpy(match_text, line + ovector[0], copy_len);
            match_text[copy_len] = '\0';
            cJSON_AddStringToObject(result_obj, "match", match_text);
            
            // Add context before
            if (ctx->context_lines > 0) {
                cJSON* context_before = cJSON_CreateArray();
                size_t start = i > ctx->context_lines ? i - ctx->context_lines : 0;
                for (size_t j = start; j < i; j++) {
                    cJSON_AddItemToArray(context_before, cJSON_CreateString(lines[j]));
                }
                cJSON_AddItemToObject(result_obj, "context_before", context_before);
            }
            
            // Add context after
            if (ctx->context_lines > 0) {
                cJSON* context_after = cJSON_CreateArray();
                size_t end = (i + 1 + ctx->context_lines < num_lines) ? i + 1 + ctx->context_lines : num_lines;
                for (size_t j = i + 1; j < end; j++) {
                    cJSON_AddItemToArray(context_after, cJSON_CreateString(lines[j]));
                }
                cJSON_AddItemToObject(result_obj, "context_after", context_after);
            }
            
            cJSON_AddItemToArray(ctx->results_array, result_obj);
            (*ctx->total_matches)++;
        }
    }
    
    pcre2_match_data_free(match_data);
    gs_free(content);
}

static void _search_code_directory(const char* path, bool recursive, search_code_context_t* ctx, const char* filter) {
    if (*ctx->total_matches >= ctx->max_results) {
        *ctx->truncated = true;
        return;
    }
    
    DIR* dir = opendir(path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        
        if (S_ISDIR(st.st_mode)) {
            if (recursive) {
                _search_code_directory(full_path, recursive, ctx, filter);
            }
        } else {
            if (_match_extension(entry->d_name, filter)) {
                _search_code_in_file(full_path, ctx);
            }
        }
    }
    
    closedir(dir);
}

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_search_code_execute(void* user_data, const char* args_json) {
    // Parse arguments
    cJSON* args = cJSON_Parse(args_json);
    if (!args) {
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Failed to parse JSON arguments"
        );
    }
    
    // Get path parameter
    cJSON* path_item = cJSON_GetObjectItem(args, "path");
    if (!path_item || !cJSON_IsString(path_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'path' parameter"
        );
    }
    const char* path = path_item->valuestring;
    
    // Get pattern parameter
    cJSON* pattern_item = cJSON_GetObjectItem(args, "pattern");
    if (!pattern_item || !cJSON_IsString(pattern_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'pattern' parameter"
        );
    }
    const char* pattern = pattern_item->valuestring;
    
    // Validate path
    if (!_code_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get optional parameters
    bool recursive = true;
    cJSON* recursive_item = cJSON_GetObjectItem(args, "recursive");
    if (recursive_item && cJSON_IsBool(recursive_item)) {
        recursive = cJSON_IsTrue(recursive_item);
    }
    
    const char* filter = ".c,.h,.cpp,.hpp";
    cJSON* filter_item = cJSON_GetObjectItem(args, "filter");
    if (filter_item && cJSON_IsString(filter_item)) {
        filter = filter_item->valuestring;
    }
    
    size_t max_results = GS_CORE_AI_TOOLS_CODE_MAX_RESULTS_DEFAULT;
    cJSON* max_results_item = cJSON_GetObjectItem(args, "max_results");
    if (max_results_item && cJSON_IsNumber(max_results_item)) {
        max_results = (size_t)max_results_item->valuedouble;
    }
    
    size_t context_lines = 2;
    cJSON* context_lines_item = cJSON_GetObjectItem(args, "context_lines");
    if (context_lines_item && cJSON_IsNumber(context_lines_item)) {
        context_lines = (size_t)context_lines_item->valuedouble;
        if (context_lines > GS_CORE_AI_TOOLS_CODE_MAX_CONTEXT_LINES) {
            context_lines = GS_CORE_AI_TOOLS_CODE_MAX_CONTEXT_LINES;
        }
    }
    
    // Get optional case_insensitive parameter
    bool case_insensitive = false;
    cJSON* case_insensitive_item = cJSON_GetObjectItem(args, "case_insensitive");
    if (case_insensitive_item && cJSON_IsBool(case_insensitive_item)) {
        case_insensitive = cJSON_IsTrue(case_insensitive_item);
    }
    
    // Compile regex
    int errornumber;
    PCRE2_SIZE erroroffset;
    uint32_t regex_options = case_insensitive ? PCRE2_CASELESS : 0;
    pcre2_code* regex = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        regex_options,
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (!regex) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Invalid regex pattern: %s (at offset %zu)", buffer, erroroffset);
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            error_msg
        );
    }
    
    // Create response
    cJSON* response = cJSON_CreateObject();
    cJSON* results_array = cJSON_CreateArray();
    size_t total_matches = 0;
    bool truncated = false;
    
    search_code_context_t ctx = {
        .results_array = results_array,
        .regex = regex,
        .total_matches = &total_matches,
        .truncated = &truncated,
        .max_results = max_results,
        .context_lines = context_lines
    };
    
    // Check if path is file or directory
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            _search_code_directory(path, recursive, &ctx, filter);
        } else {
            _search_code_in_file(path, &ctx);
        }
    } else {
        pcre2_code_free(regex);
        cJSON_Delete(results_array);
        cJSON_Delete(response);
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_NOT_FOUND,
            "Path not found"
        );
    }
    
    cJSON_AddItemToObject(response, "results", results_array);
    cJSON_AddNumberToObject(response, "total_matches", (double)total_matches);
    cJSON_AddBoolToObject(response, "truncated", truncated);
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    pcre2_code_free(regex);
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Implementation: list_functions
================================================================================*/

typedef struct {
    cJSON* functions_array;
    size_t* total_functions;
    bool* truncated;
    size_t max_results;
} list_functions_context_t;

static bool _is_function_line(const char* line, char* func_name, size_t func_name_size, char* return_type, size_t return_type_size) {
    // Heuristic for C function definitions/declarations
    // Must have proper return type before function name
    
    // Skip leading whitespace
    const char* orig_line = line;
    while (*line && isspace(*line)) line++;
    
    // Skip empty lines
    if (*line == '\0') return false;
    
    // Skip common non-function patterns
    if (strncmp(line, "if", 2) == 0 || strncmp(line, "for", 3) == 0 ||
        strncmp(line, "while", 5) == 0 || strncmp(line, "switch", 6) == 0 ||
        strncmp(line, "return", 6) == 0 || strncmp(line, "//", 2) == 0 ||
        strncmp(line, "/*", 2) == 0 || strncmp(line, "#", 1) == 0) {
        return false;
    }
    
    // Skip lines with assignments (has '=' before '(')
    const char* eq = strchr(line, '=');
    const char* paren = strchr(line, '(');
    if (!paren) return false;
    if (eq && eq < paren) return false;
    
    // Find function name (work backwards from paren)
    const char* name_end = paren - 1;
    while (name_end > line && isspace(*name_end)) name_end--;
    
    const char* name_start = name_end;
    while (name_start > line && (isalnum(*name_start) || *name_start == '_')) {
        name_start--;
    }
    if (name_start != name_end) {
        name_start++;
    }
    
    // Extract function name
    size_t name_len = name_end - name_start + 1;
    if (name_len == 0 || name_len >= func_name_size) return false;
    
    memcpy(func_name, name_start, name_len);
    func_name[name_len] = '\0';
    
    // Must have at least one lowercase letter in function name
    bool has_lower = false;
    for (size_t i = 0; i < name_len; i++) {
        if (islower(func_name[i])) {
            has_lower = true;
            break;
        }
    }
    if (!has_lower) return false;
    
    // Extract return type (everything before function name)
    const char* type_end = name_start - 1;
    while (type_end > line && isspace(*type_end)) type_end--;
    
    // CRITICAL: Function definitions MUST have a return type or storage class
    // A function call won't have this (e.g., "curl_easy_setopt(...)" has no return type before it)
    size_t type_len = type_end - line + 1;
    if (type_len == 0) {
        // No return type - likely a function call
        return false;
    } else if (type_len < return_type_size) {
        memcpy(return_type, line, type_len);
        return_type[type_len] = '\0';
        
        // Validate return type looks reasonable (has word characters, not just punctuation)
        bool has_alnum = false;
        for (size_t i = 0; i < type_len; i++) {
            if (isalnum(return_type[i])) {
                has_alnum = true;
                break;
            }
        }
        if (!has_alnum) return false;
    } else {
        return_type[0] = '\0';
    }
    
    return true;
}

static void _list_functions_in_file(const char* filepath, list_functions_context_t* ctx) {
    if (*ctx->total_functions >= ctx->max_results) {
        *ctx->truncated = true;
        return;
    }
    
    // Read file
    size_t file_size = 0;
    char* content = gs_platform_read_file_contents(filepath, "rb", &file_size);
    if (!content) return;
    
    // Skip files that are too large
    if (file_size > GS_CORE_AI_TOOLS_CODE_MAX_FILE_SIZE) {
        gs_free(content);
        return;
    }
    
    // Parse line by line
    char* line_start = content;
    size_t line_num = 1;
    char* current = content;
    
    // Pattern for function definitions - look for ")" followed by optional whitespace and "{"
    pcre2_code* func_regex = pcre2_compile(
        (PCRE2_SPTR)"^\\s*\\w[\\w\\s\\*]+\\s+\\w+\\s*\\([^)]*\\)\\s*$",
        PCRE2_ZERO_TERMINATED,
        0,
        NULL,
        NULL,
        NULL
    );
    
    while (*current) {
        if (*current == '\n') {
            *current = '\0';
            
            // Skip lines with significant indentation (likely not function definitions)
            const char* check = line_start;
            size_t indent = 0;
            while (*check == ' ' || *check == '\t') {
                indent++;
                check++;
            }
            
            // Function definitions should have 0 indentation (at start of line after any storage class)
            if (indent == 0 && *check != '\0') {
                // Check if this looks like a function definition
                char func_name[128];
                char return_type[256];
                
                if (_is_function_line(line_start, func_name, sizeof(func_name), return_type, sizeof(return_type))) {
                    // Additional validation: must have closing paren and either semicolon or opening brace nearby
                    const char* paren_close = strchr(line_start, ')');
                    if (paren_close) {
                        // Check next few characters for ';' or '{' or just whitespace/newline
                        const char* after = paren_close + 1;
                        while (*after == ' ' || *after == '\t' || *after == '\r') after++;
                        
                        // Valid if followed by semicolon, brace, or end of line
                        if (*after == ';' || *after == '{' || *after == '\0') {
                            if (*ctx->total_functions < ctx->max_results) {
                                cJSON* func_obj = cJSON_CreateObject();
                                cJSON_AddStringToObject(func_obj, "name", func_name);
                                cJSON_AddStringToObject(func_obj, "signature", line_start);
                                cJSON_AddStringToObject(func_obj, "file", filepath);
                                cJSON_AddNumberToObject(func_obj, "line", (double)line_num);
                                if (strlen(return_type) > 0) {
                                    cJSON_AddStringToObject(func_obj, "return_type", return_type);
                                }
                                
                                cJSON_AddItemToArray(ctx->functions_array, func_obj);
                                (*ctx->total_functions)++;
                            }
                        }
                    }
                }
            }
            
            line_start = current + 1;
            line_num++;
        }
        current++;
    }
    
    // Check last line
    if (*line_start) {
        // Skip lines with significant indentation
        const char* check = line_start;
        size_t indent = 0;
        while (*check == ' ' || *check == '\t') {
            indent++;
            check++;
        }
        
        if (indent == 0 && *check != '\0') {
            char func_name[128];
            char return_type[256];
            
            if (_is_function_line(line_start, func_name, sizeof(func_name), return_type, sizeof(return_type))) {
                // Additional validation: must have closing paren and proper ending
                const char* paren_close = strchr(line_start, ')');
                if (paren_close) {
                    const char* after = paren_close + 1;
                    while (*after == ' ' || *after == '\t' || *after == '\r') after++;
                    
                    if (*after == ';' || *after == '{' || *after == '\0') {
                        if (*ctx->total_functions < ctx->max_results) {
                            cJSON* func_obj = cJSON_CreateObject();
                            cJSON_AddStringToObject(func_obj, "name", func_name);
                            cJSON_AddStringToObject(func_obj, "signature", line_start);
                            cJSON_AddStringToObject(func_obj, "file", filepath);
                            cJSON_AddNumberToObject(func_obj, "line", (double)line_num);
                            if (strlen(return_type) > 0) {
                                cJSON_AddStringToObject(func_obj, "return_type", return_type);
                            }
                            
                            cJSON_AddItemToArray(ctx->functions_array, func_obj);
                            (*ctx->total_functions)++;
                        }
                    }
                }
            }
        }
    }
    
    if (func_regex) pcre2_code_free(func_regex);
    gs_free(content);
}

static void _list_functions_directory(const char* path, bool recursive, list_functions_context_t* ctx, const char* filter) {
    if (*ctx->total_functions >= ctx->max_results) {
        *ctx->truncated = true;
        return;
    }
    
    DIR* dir = opendir(path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        
        if (S_ISDIR(st.st_mode)) {
            if (recursive) {
                _list_functions_directory(full_path, recursive, ctx, filter);
            }
        } else {
            if (_match_extension(entry->d_name, filter)) {
                _list_functions_in_file(full_path, ctx);
            }
        }
    }
    
    closedir(dir);
}

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_list_functions_execute(void* user_data, const char* args_json) {
    // Parse arguments
    cJSON* args = cJSON_Parse(args_json);
    if (!args) {
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Failed to parse JSON arguments"
        );
    }
    
    // Get path parameter
    cJSON* path_item = cJSON_GetObjectItem(args, "path");
    if (!path_item || !cJSON_IsString(path_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'path' parameter"
        );
    }
    const char* path = path_item->valuestring;
    
    // Validate path
    if (!_code_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get optional parameters
    bool recursive = true;
    cJSON* recursive_item = cJSON_GetObjectItem(args, "recursive");
    if (recursive_item && cJSON_IsBool(recursive_item)) {
        recursive = cJSON_IsTrue(recursive_item);
    }
    
    const char* filter = ".c,.h,.cpp,.hpp";
    cJSON* filter_item = cJSON_GetObjectItem(args, "filter");
    if (filter_item && cJSON_IsString(filter_item)) {
        filter = filter_item->valuestring;
    }
    
    size_t max_results = 200;
    cJSON* max_results_item = cJSON_GetObjectItem(args, "max_results");
    if (max_results_item && cJSON_IsNumber(max_results_item)) {
        max_results = (size_t)max_results_item->valuedouble;
    }
    
    // Create response
    cJSON* response = cJSON_CreateObject();
    cJSON* functions_array = cJSON_CreateArray();
    size_t total_functions = 0;
    bool truncated = false;
    
    list_functions_context_t ctx = {
        .functions_array = functions_array,
        .total_functions = &total_functions,
        .truncated = &truncated,
        .max_results = max_results
    };
    
    // Check if path is file or directory
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            _list_functions_directory(path, recursive, &ctx, filter);
        } else {
            _list_functions_in_file(path, &ctx);
        }
    } else {
        cJSON_Delete(functions_array);
        cJSON_Delete(response);
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_NOT_FOUND,
            "Path not found"
        );
    }
    
    cJSON_AddItemToObject(response, "functions", functions_array);
    cJSON_AddNumberToObject(response, "total_functions", (double)total_functions);
    cJSON_AddBoolToObject(response, "truncated", truncated);
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Implementation: find_definition
================================================================================*/

typedef struct {
    cJSON* definitions_array;
    const char* symbol;
    size_t* total_found;
    bool* found;
} find_definition_context_t;

static const char* _detect_definition_type(const char* line, const char* symbol) {
    // Detect the type of definition
    if (strstr(line, "struct") && strstr(line, symbol)) return "struct";
    if (strstr(line, "typedef") && strstr(line, symbol)) return "typedef";
    if (strstr(line, "enum") && strstr(line, symbol)) return "enum";
    if (strstr(line, "#define") && strstr(line, symbol)) return "macro";
    
    // Check for function definition (has parentheses)
    const char* sym_pos = strstr(line, symbol);
    if (sym_pos) {
        // Look for '(' after symbol
        const char* paren = strchr(sym_pos, '(');
        if (paren && (paren - sym_pos) < 50) {
            return "function";
        }
    }
    
    return "variable";
}

static void _find_definition_in_file(const char* filepath, find_definition_context_t* ctx) {
    // Read file
    size_t file_size = 0;
    char* content = gs_platform_read_file_contents(filepath, "rb", &file_size);
    if (!content) return;
    
    // Skip files that are too large
    if (file_size > GS_CORE_AI_TOOLS_CODE_MAX_FILE_SIZE) {
        gs_free(content);
        return;
    }
    
    // Create regex for finding symbol definitions
    // Match: <symbol> followed by ( or ; or = or {
    char pattern[512];
    snprintf(pattern, sizeof(pattern), "\\b%s\\b", ctx->symbol);
    
    int errornumber;
    PCRE2_SIZE erroroffset;
    pcre2_code* regex = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        PCRE2_CASELESS,  // Make search case-insensitive for better symbol matching
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (!regex) {
        gs_free(content);
        return;
    }
    
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
    
    // Parse line by line
    char* line_start = content;
    size_t line_num = 1;
    char* current = content;
    
    // Store lines for context
    #define MAX_DEF_LINES 10000
    char* lines[MAX_DEF_LINES];
    size_t num_lines = 0;
    
    lines[num_lines++] = line_start;
    while (*current && num_lines < MAX_DEF_LINES) {
        if (*current == '\n') {
            *current = '\0';
            if (*(current + 1)) {
                lines[num_lines++] = current + 1;
            }
        }
        current++;
    }
    
    // Search for symbol in each line
    for (size_t i = 0; i < num_lines; i++) {
        const char* line = lines[i];
        size_t line_len = strlen(line);
        
        // Skip comment lines
        const char* trimmed = line;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        if (strncmp(trimmed, "//", 2) == 0 || strncmp(trimmed, "/*", 2) == 0) continue;
        
        int rc = pcre2_match(
            regex,
            (PCRE2_SPTR)line,
            line_len,
            0,
            0,
            match_data,
            NULL
        );
        
        if (rc >= 0) {
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
            size_t match_pos = ovector[0];
            
            // Check if match is inside a string literal
            bool in_string = false;
            for (size_t j = 0; j < match_pos; j++) {
                if (line[j] == '"' && (j == 0 || line[j-1] != '\\')) {
                    in_string = !in_string;
                }
            }
            if (in_string) continue;
            
            // Found potential definition
            const char* type = _detect_definition_type(line, ctx->symbol);
            
            // For functions, verify it looks like a real definition
            if (strcmp(type, "function") == 0) {
                // Should have return type or be at start of line
                // Skip if it's clearly a call (e.g., indented with spaces suggesting it's in a function body)
                size_t leading_spaces = 0;
                for (const char* p = line; *p == ' ' || *p == '\t'; p++) {
                    leading_spaces++;
                }
                // Function definitions typically have 0-4 spaces of indentation
                // Function calls typically have 4+ spaces
                if (leading_spaces > 4 && strstr(line, "static") == NULL && 
                    strstr(line, "extern") == NULL && strstr(line, "inline") == NULL) {
                    continue;
                }
            }
            
            cJSON* def_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(def_obj, "type", type);
            cJSON_AddStringToObject(def_obj, "name", ctx->symbol);
            cJSON_AddStringToObject(def_obj, "signature", line);
            cJSON_AddStringToObject(def_obj, "file", filepath);
            cJSON_AddNumberToObject(def_obj, "line", (double)(i + 1));
            
            // Add context
            cJSON* context_array = cJSON_CreateArray();
            size_t context_start = i > 2 ? i - 2 : 0;
            size_t context_end = (i + 3 < num_lines) ? i + 3 : num_lines;
            for (size_t j = context_start; j < context_end; j++) {
                cJSON_AddItemToArray(context_array, cJSON_CreateString(lines[j]));
            }
            cJSON_AddItemToObject(def_obj, "context", context_array);
            
            cJSON_AddItemToArray(ctx->definitions_array, def_obj);
            (*ctx->total_found)++;
            *ctx->found = true;
        }
    }
    
    pcre2_match_data_free(match_data);
    pcre2_code_free(regex);
    gs_free(content);
}

static void _find_definition_directory(const char* path, bool recursive, find_definition_context_t* ctx, const char* filter) {
    DIR* dir = opendir(path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        
        if (S_ISDIR(st.st_mode)) {
            if (recursive) {
                _find_definition_directory(full_path, recursive, ctx, filter);
            }
        } else {
            if (_match_extension(entry->d_name, filter)) {
                _find_definition_in_file(full_path, ctx);
            }
        }
    }
    
    closedir(dir);
}

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_find_definition_execute(void* user_data, const char* args_json) {
    // Parse arguments
    cJSON* args = cJSON_Parse(args_json);
    if (!args) {
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Failed to parse JSON arguments"
        );
    }
    
    // Get symbol parameter
    cJSON* symbol_item = cJSON_GetObjectItem(args, "symbol");
    if (!symbol_item || !cJSON_IsString(symbol_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'symbol' parameter"
        );
    }
    const char* symbol = symbol_item->valuestring;
    
    // Get optional path parameter
    const char* path = ".";
    cJSON* path_item = cJSON_GetObjectItem(args, "path");
    if (path_item && cJSON_IsString(path_item)) {
        path = path_item->valuestring;
    }
    
    // Validate path
    if (!_code_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get optional parameters
    bool recursive = true;
    cJSON* recursive_item = cJSON_GetObjectItem(args, "recursive");
    if (recursive_item && cJSON_IsBool(recursive_item)) {
        recursive = cJSON_IsTrue(recursive_item);
    }
    
    const char* filter = ".c,.h,.cpp,.hpp";
    cJSON* filter_item = cJSON_GetObjectItem(args, "filter");
    if (filter_item && cJSON_IsString(filter_item)) {
        filter = filter_item->valuestring;
    }
    
    // Create response
    cJSON* response = cJSON_CreateObject();
    cJSON* definitions_array = cJSON_CreateArray();
    size_t total_found = 0;
    bool found = false;
    
    find_definition_context_t ctx = {
        .definitions_array = definitions_array,
        .symbol = symbol,
        .total_found = &total_found,
        .found = &found
    };
    
    // Check if path is file or directory
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            _find_definition_directory(path, recursive, &ctx, filter);
        } else {
            _find_definition_in_file(path, &ctx);
        }
    } else {
        cJSON_Delete(definitions_array);
        cJSON_Delete(response);
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_NOT_FOUND,
            "Path not found"
        );
    }
    
    cJSON_AddBoolToObject(response, "found", found);
    cJSON_AddItemToObject(response, "definitions", definitions_array);
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Registration
================================================================================*/

GS_API_DECL void 
gs_core_ai_tools_code_register(gs_core_ai_coordinator_t* coordinator) {
    if (!coordinator) return;
    
    // Register search_code tool
    {
        gs_core_ai_tool_t tool = {
            .name = "search_code",
            .description = "Search for code patterns using regex. Use this to find specific code patterns, function calls, or text in source files.",
            .parameters_schema = "{\"path\": \"string (required) - Directory or file to search\", \"pattern\": \"string (required) - Regex pattern to search for\", \"recursive\": \"bool (optional) - Search recursively (default: true)\", \"filter\": \"string (optional) - File extension filter (e.g., '.c,.h,.cpp')\", \"max_results\": \"number (optional) - Max results (default: 100)\", \"context_lines\": \"number (optional) - Context lines before/after match (default: 2)\", \"case_insensitive\": \"bool (optional) - Case-insensitive search (default: false)\"}",
            .execute = gs_core_ai_tool_code_search_code_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
    
    // Register list_functions tool
    {
        gs_core_ai_tool_t tool = {
            .name = "list_functions",
            .description = "List function definitions in source files. Use this to get an overview of functions in a file or directory.",
            .parameters_schema = "{\"path\": \"string (required) - Directory or file to analyze\", \"recursive\": \"bool (optional) - Search recursively (default: true)\", \"filter\": \"string (optional) - File extension filter (default: '.c,.h,.cpp,.hpp')\", \"max_results\": \"number (optional) - Max results (default: 200)\"}",
            .execute = gs_core_ai_tool_code_list_functions_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
    
    // Register find_definition tool
    {
        gs_core_ai_tool_t tool = {
            .name = "find_definition",
            .description = "Find the definition of a symbol (function, struct, typedef, enum, macro, variable). Use this to locate where a symbol is defined. Case-insensitive by default.",
            .parameters_schema = "{\"symbol\": \"string (required) - Symbol name to find\", \"path\": \"string (optional) - File or directory to search. If not provided, searches current directory. Use this to narrow search when you know the file.\", \"recursive\": \"bool (optional) - Search recursively in directories (default: true)\", \"filter\": \"string (optional) - File extension filter (default: '.c,.h,.cpp,.hpp')\"}",
            .execute = gs_core_ai_tool_code_find_definition_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
}


