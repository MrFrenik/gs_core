/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_fs.c
    * 
    * File System Tools for AI Coordinator - Implementation

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
#define PCRE2_STATIC
#include <pcre2/pcre2.h>

#include "gs_core_ai_tools_fs.h"
#include <cJSON/cJSON.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

// For directory operations on Windows
#ifdef _WIN32
    #include <gs/external/dirent/dirent.h>
#else
    #include <dirent.h>
#endif

/*================================================================================
// Constants
================================================================================*/

#define GS_CORE_AI_TOOLS_FS_MAX_FILE_SIZE_DEFAULT (1024 * 1024)  // 1MB
#define GS_CORE_AI_TOOLS_FS_MAX_SEARCH_RESULTS_DEFAULT 100
#define GS_CORE_AI_TOOLS_FS_MAX_LINE_LENGTH 4096

/*================================================================================
// Helper Functions
================================================================================*/

static bool _is_path_safe(const char* path) {
    // Basic path safety check - prevent path traversal attacks
    if (!path || strlen(path) == 0) return false;
    
    // Use regex to check for suspicious patterns
    // Pattern checks for: path traversal (..), null bytes, control characters
    const char* pattern = "(\\.\\.|\\x00|[\\x01-\\x1F])";
    
    int errornumber;
    PCRE2_SIZE erroroffset;
    pcre2_code* re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        0,
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (!re) return false; // If regex fails to compile, be safe and reject
    
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    int rc = pcre2_match(
        re,
        (PCRE2_SPTR)path,
        strlen(path),
        0,
        0,
        match_data,
        NULL
    );
    
    bool is_safe = (rc == PCRE2_ERROR_NOMATCH); // Safe if no suspicious patterns found
    
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
    
    return is_safe;
}

static bool _is_binary_content(const char* data, size_t size) {
    // Simple heuristic: if more than 5% of bytes are non-printable, consider binary
    size_t non_printable = 0;
    size_t check_size = size < 1024 ? size : 1024; // Check first 1KB
    
    for (size_t i = 0; i < check_size; i++) {
        unsigned char c = (unsigned char)data[i];
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            non_printable++;
        }
    }
    
    return (check_size > 0 && (non_printable * 100 / check_size) > 5);
}

static bool _has_extension(const char* path, const char* ext) {
    if (!path || !ext) return false;
    
    // Build regex pattern for file extensions
    // Supports wildcards like "*.c" or comma-separated list like ".c,.h"
    char pattern[512];
    
    // Check if ext is a comma-separated list or contains wildcards
    if (strchr(ext, ',') != NULL || strchr(ext, '*') != NULL) {
        // Build pattern: \.(c|h|cpp)$ or similar
        char ext_pattern[400] = {0};
        
        if (strchr(ext, ',') != NULL) {
            // Comma-separated list
            char* ext_copy = gs_malloc(strlen(ext) + 1);
            strcpy(ext_copy, ext);
            
            bool first = true;
            char* token = strtok(ext_copy, ",");
            while (token) {
                // Trim whitespace and remove leading dot if present
                while (*token == ' ') token++;
                if (*token == '.') token++;
                
                if (!first) strcat(ext_pattern, "|");
                strcat(ext_pattern, token);
                first = false;
                
                token = strtok(NULL, ",");
            }
            gs_free(ext_copy);
            
            snprintf(pattern, sizeof(pattern), "\\.((%s))$", ext_pattern);
        } else {
            // Wildcard pattern - convert to regex
            // e.g., "*.c" -> "\.c$"
            const char* asterisk = strchr(ext, '*');
            if (asterisk) {
                const char* ext_part = asterisk + 1;
                if (*ext_part == '.') ext_part++;
                snprintf(pattern, sizeof(pattern), "\\.%s$", ext_part);
            } else {
                // Just use as-is
                const char* ext_part = (*ext == '.') ? ext + 1 : ext;
                snprintf(pattern, sizeof(pattern), "\\.%s$", ext_part);
            }
        }
    } else {
        // Single extension
        const char* ext_part = (*ext == '.') ? ext + 1 : ext;
        snprintf(pattern, sizeof(pattern), "\\.%s$", ext_part);
    }
    
    // Compile and execute regex
    int errornumber;
    PCRE2_SIZE erroroffset;
    pcre2_code* re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        PCRE2_CASELESS, // Case-insensitive for file extensions
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (!re) return false;
    
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    int rc = pcre2_match(
        re,
        (PCRE2_SPTR)path,
        strlen(path),
        0,
        0,
        match_data,
        NULL
    );
    
    bool matches = (rc >= 0);
    
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
    
    return matches;
}

static void _list_directory_recursive(const char* path, cJSON* entries_array, 
                                       bool recursive, const char* filter) {
    DIR* dir = opendir(path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Build full path
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        // Check if directory
        struct stat st;
        bool is_directory = false;
        size_t file_size = 0;
        
        if (stat(full_path, &st) == 0) {
            is_directory = S_ISDIR(st.st_mode);
            if (!is_directory) {
                file_size = st.st_size;
            }
        }
        
        // Check filter
        if (filter && strlen(filter) > 0 && !is_directory) {
            if (!_has_extension(entry->d_name, filter)) {
                continue;
            }
        }
        
        // Add entry to JSON array
        cJSON* entry_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(entry_obj, "name", entry->d_name);
        cJSON_AddStringToObject(entry_obj, "path", full_path);
        cJSON_AddBoolToObject(entry_obj, "is_directory", is_directory);
        cJSON_AddNumberToObject(entry_obj, "size", (double)file_size);
        
        cJSON_AddItemToArray(entries_array, entry_obj);
        
        // Recurse if needed
        if (recursive && is_directory) {
            _list_directory_recursive(full_path, entries_array, recursive, filter);
        }
    }
    
    closedir(dir);
}

/*================================================================================
// Tool Implementation: read_file
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_read_file_execute(void* user_data, const char* args_json) {
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
    if (!_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get max_size parameter (optional)
    size_t max_size = GS_CORE_AI_TOOLS_FS_MAX_FILE_SIZE_DEFAULT;
    cJSON* max_size_item = cJSON_GetObjectItem(args, "max_size");
    if (max_size_item && cJSON_IsNumber(max_size_item)) {
        max_size = (size_t)max_size_item->valuedouble;
    }
    
    // Read file
    size_t file_size = 0;
    char* content = gs_platform_read_file_contents(path, "rb", &file_size);
    if (!content) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_NOT_FOUND,
            "Failed to read file - file not found or permission denied"
        );
    }
    
    // Check size limit
    if (file_size > max_size) {
        gs_free(content);
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "File size exceeds maximum allowed size"
        );
    }
    
    // Check if binary
    bool is_binary = _is_binary_content(content, file_size);
    
    // Create response JSON
    cJSON* response = cJSON_CreateObject();
    if (is_binary) {
        cJSON_AddStringToObject(response, "content", "[Binary file - content not displayed]");
    } else {
        cJSON_AddStringToObject(response, "content", content);
    }
    cJSON_AddNumberToObject(response, "size", (double)file_size);
    cJSON_AddBoolToObject(response, "is_binary", is_binary);
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    gs_free(content);
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Implementation: list_directory
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_list_directory_execute(void* user_data, const char* args_json) {
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
    if (!_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get recursive parameter (optional)
    bool recursive = false;
    cJSON* recursive_item = cJSON_GetObjectItem(args, "recursive");
    if (recursive_item && cJSON_IsBool(recursive_item)) {
        recursive = cJSON_IsTrue(recursive_item);
    }
    
    // Get filter parameter (optional)
    const char* filter = NULL;
    cJSON* filter_item = cJSON_GetObjectItem(args, "filter");
    if (filter_item && cJSON_IsString(filter_item)) {
        filter = filter_item->valuestring;
    }
    
    // Create response JSON
    cJSON* response = cJSON_CreateObject();
    cJSON* entries_array = cJSON_CreateArray();
    
    // List directory
    _list_directory_recursive(path, entries_array, recursive, filter);
    
    cJSON_AddItemToObject(response, "entries", entries_array);
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Implementation: file_exists
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_file_exists_execute(void* user_data, const char* args_json) {
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
    if (!_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Check if file exists
    struct stat st;
    bool exists = (stat(path, &st) == 0);
    bool is_directory = false;
    size_t size = 0;
    
    if (exists) {
        is_directory = S_ISDIR(st.st_mode);
        if (!is_directory) {
            size = st.st_size;
        }
    }
    
    // Create response JSON
    cJSON* response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "exists", exists);
    if (exists) {
        cJSON_AddBoolToObject(response, "is_directory", is_directory);
        cJSON_AddNumberToObject(response, "size", (double)size);
    }
    
    char* response_str = cJSON_PrintUnformatted(response);
    gs_core_ai_tool_result_t result = gs_core_ai_tool_result_success(response_str);
    
    // Cleanup
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Implementation: search_files
================================================================================*/

typedef struct {
    cJSON* results_array;
    const char* pattern;
    pcre2_code* compiled_pattern;
    const char* filter;
    size_t max_results;
    size_t* total_matches;
    bool* truncated;
} search_files_context_t;

static void _search_file(const char* file_path, search_files_context_t* ctx) {
    if (*ctx->total_matches >= ctx->max_results) {
        *ctx->truncated = true;
        return;
    }
    
    FILE* fp = fopen(file_path, "r");
    if (!fp) return;
    
    char line[GS_CORE_AI_TOOLS_FS_MAX_LINE_LENGTH];
    size_t line_num = 1;
    
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(ctx->compiled_pattern, NULL);
    
    while (fgets(line, sizeof(line), fp) && *ctx->total_matches < ctx->max_results) {
        // Try regex match
        int rc = pcre2_match(
            ctx->compiled_pattern,
            (PCRE2_SPTR)line,
            strlen(line),
            0,
            0,
            match_data,
            NULL
        );
        
        if (rc >= 0) {
            // Match found
            // Remove trailing newline
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }
            if (len > 1 && line[len - 2] == '\r') {
                line[len - 2] = '\0';
            }
            
            cJSON* result_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(result_obj, "file", file_path);
            cJSON_AddNumberToObject(result_obj, "line", (double)line_num);
            cJSON_AddStringToObject(result_obj, "content", line);
            
            // Add match position info
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
            cJSON_AddNumberToObject(result_obj, "match_start", (double)ovector[0]);
            cJSON_AddNumberToObject(result_obj, "match_length", (double)(ovector[1] - ovector[0]));
            
            cJSON_AddItemToArray(ctx->results_array, result_obj);
            
            (*ctx->total_matches)++;
        }
        line_num++;
    }
    
    pcre2_match_data_free(match_data);
    fclose(fp);
}

static void _search_directory(const char* dir_path, bool recursive, search_files_context_t* ctx) {
    if (*ctx->truncated) return;
    
    DIR* dir = opendir(dir_path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && !*ctx->truncated) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Build full path
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        // Check if directory
        struct stat st;
        bool is_directory = false;
        
        if (stat(full_path, &st) == 0) {
            is_directory = S_ISDIR(st.st_mode);
        }
        
        if (is_directory) {
            if (recursive) {
                _search_directory(full_path, recursive, ctx);
            }
        } else {
            // Check filter
            if (ctx->filter && strlen(ctx->filter) > 0) {
                if (!_has_extension(entry->d_name, ctx->filter)) {
                    continue;
                }
            }
            
            _search_file(full_path, ctx);
        }
    }
    
    closedir(dir);
}

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_search_files_execute(void* user_data, const char* args_json) {
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
    if (!_is_path_safe(path)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid or unsafe path"
        );
    }
    
    // Get recursive parameter (optional, default true)
    bool recursive = true;
    cJSON* recursive_item = cJSON_GetObjectItem(args, "recursive");
    if (recursive_item && cJSON_IsBool(recursive_item)) {
        recursive = cJSON_IsTrue(recursive_item);
    }
    
    // Get filter parameter (optional)
    const char* filter = NULL;
    cJSON* filter_item = cJSON_GetObjectItem(args, "filter");
    if (filter_item && cJSON_IsString(filter_item)) {
        filter = filter_item->valuestring;
    }
    
    // Get max_results parameter (optional)
    size_t max_results = GS_CORE_AI_TOOLS_FS_MAX_SEARCH_RESULTS_DEFAULT;
    cJSON* max_results_item = cJSON_GetObjectItem(args, "max_results");
    if (max_results_item && cJSON_IsNumber(max_results_item)) {
        max_results = (size_t)max_results_item->valuedouble;
    }
    
    // Compile the regex pattern
    int errornumber;
    PCRE2_SIZE erroroffset;
    pcre2_code* compiled_pattern = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        0,
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (!compiled_pattern) {
        cJSON_Delete(args);
        PCRE2_UCHAR error_buffer[256];
        pcre2_get_error_message(errornumber, error_buffer, sizeof(error_buffer));
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Invalid regex pattern: %s at offset %zu", error_buffer, erroroffset);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            error_msg
        );
    }
    
    // Create response JSON
    cJSON* response = cJSON_CreateObject();
    cJSON* results_array = cJSON_CreateArray();
    size_t total_matches = 0;
    bool truncated = false;
    
    // Setup search context
    search_files_context_t ctx = {
        .results_array = results_array,
        .pattern = pattern,
        .compiled_pattern = compiled_pattern,
        .filter = filter,
        .max_results = max_results,
        .total_matches = &total_matches,
        .truncated = &truncated
    };
    
    // Start search
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            _search_directory(path, recursive, &ctx);
        } else {
            // Single file
            _search_file(path, &ctx);
        }
    } else {
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
    pcre2_code_free(compiled_pattern);
    cJSON_free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(args);
    
    return result;
}

/*================================================================================
// Tool Registration
================================================================================*/

GS_API_DECL void 
gs_core_ai_tools_fs_register(gs_core_ai_coordinator_t* coordinator) {
    if (!coordinator) return;
    
    // Register read_file tool
    {
        gs_core_ai_tool_t tool = {
            .name = "read_file",
            .description = "Read and return the contents of a file. Use this when you need to examine the contents of a specific file.",
            .parameters_schema = "{\"path\": \"string (required) - Path to the file\", \"max_size\": \"number (optional) - Maximum file size in bytes (default: 1MB)\"}",
            .execute = gs_core_ai_tool_fs_read_file_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
    
    // Register list_directory tool
    {
        gs_core_ai_tool_t tool = {
            .name = "list_directory",
            .description = "List files and directories in a directory. Use this to explore directory contents.",
            .parameters_schema = "{\"path\": \"string (required) - Directory path\", \"recursive\": \"bool (optional) - List recursively\", \"filter\": \"string (optional) - File extension filter (e.g., '.c')\"}",
            .execute = gs_core_ai_tool_fs_list_directory_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
    
    // Register file_exists tool
    {
        gs_core_ai_tool_t tool = {
            .name = "file_exists",
            .description = "Check if a file or directory exists. Use this to verify paths before operations.",
            .parameters_schema = "{\"path\": \"string (required) - Path to check\"}",
            .execute = gs_core_ai_tool_fs_file_exists_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
    
    // Register search_files tool
    {
        gs_core_ai_tool_t tool = {
            .name = "search_files",
            .description = "Search for files containing a regex pattern (grep-like with full PCRE2 support). Use this to find code or text across files. Pattern supports regular expressions.",
            .parameters_schema = "{\"path\": \"string (required) - Directory to search\", \"pattern\": \"string (required) - PCRE2 regex pattern to search for\", \"recursive\": \"bool (optional) - Search recursively (default: true)\", \"filter\": \"string (optional) - File extension filter (supports wildcards like '*.c' or comma-separated '.c,.h')\", \"max_results\": \"number (optional) - Max results (default: 100)\"}",
            .execute = gs_core_ai_tool_fs_search_files_execute,
            .user_data = NULL
        };
        gs_core_ai_coordinator_register_tool(coordinator, tool);
    }
}
