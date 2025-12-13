/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_http.c
    * 
    * HTTP Tools for AI Coordinator - Implementation

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

#include "gs_core_ai_tools_http.h"
#include <cJSON/cJSON.h>
#include <curl/curl.h>
#include <string.h>
#include <stdio.h>

/*================================================================================
// Constants
================================================================================*/

#define GS_CORE_AI_TOOLS_HTTP_TIMEOUT_DEFAULT 30
#define GS_CORE_AI_TOOLS_HTTP_MAX_SIZE_DEFAULT (1024 * 1024)  // 1MB

/*================================================================================
// Helper Structures
================================================================================*/

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
    size_t max_size;
    bool truncated;
} http_response_buffer_t;

/*================================================================================
// Helper Functions
================================================================================*/

static bool _is_url_valid(const char* url) {
    if (!url || strlen(url) == 0) return false;
    
    // Must start with http:// or https://
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        return false;
    }
    
    return true;
}

static size_t _write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    http_response_buffer_t* buffer = (http_response_buffer_t*)userp;
    
    // Check if we've hit the size limit
    if (buffer->size + real_size > buffer->max_size) {
        size_t available = buffer->max_size - buffer->size;
        if (available > 0) {
            // Copy what we can
            if (buffer->size + available > buffer->capacity) {
                buffer->capacity = buffer->size + available + 1;
                buffer->data = gs_realloc(buffer->data, buffer->capacity);
            }
            memcpy(buffer->data + buffer->size, contents, available);
            buffer->size += available;
        }
        buffer->truncated = true;
        return real_size; // Return full size to continue the transfer
    }
    
    // Expand buffer if needed
    if (buffer->size + real_size + 1 > buffer->capacity) {
        size_t new_capacity = buffer->capacity * 2;
        if (new_capacity < buffer->size + real_size + 1) {
            new_capacity = buffer->size + real_size + 1;
        }
        buffer->data = gs_realloc(buffer->data, new_capacity);
        buffer->capacity = new_capacity;
    }
    
    // Copy data
    memcpy(buffer->data + buffer->size, contents, real_size);
    buffer->size += real_size;
    buffer->data[buffer->size] = '\0';
    
    return real_size;
}

static size_t _header_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
    // We're using this to detect Content-Type header
    // The header callback can be used to parse specific headers
    return size * nitems;
}

/*================================================================================
// Tool Execute Functions
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_http_get_execute(void* user_data, const char* args_json) {
    // Parse arguments
    cJSON* args = cJSON_Parse(args_json);
    if (!args) {
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Failed to parse arguments JSON"
        );
    }
    
    // Get required url parameter
    cJSON* url_item = cJSON_GetObjectItem(args, "url");
    if (!url_item || !cJSON_IsString(url_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'url' parameter"
        );
    }
    const char* url = url_item->valuestring;
    
    // Validate URL
    if (!_is_url_valid(url)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Invalid URL - must start with http:// or https://"
        );
    }
    
    // Get optional parameters
    long timeout = GS_CORE_AI_TOOLS_HTTP_TIMEOUT_DEFAULT;
    cJSON* timeout_item = cJSON_GetObjectItem(args, "timeout");
    if (timeout_item && cJSON_IsNumber(timeout_item)) {
        timeout = (long)timeout_item->valuedouble;
    }
    
    size_t max_size = GS_CORE_AI_TOOLS_HTTP_MAX_SIZE_DEFAULT;
    cJSON* max_size_item = cJSON_GetObjectItem(args, "max_size");
    if (max_size_item && cJSON_IsNumber(max_size_item)) {
        max_size = (size_t)max_size_item->valuedouble;
    }
    
    // Get optional headers
    cJSON* headers_item = cJSON_GetObjectItem(args, "headers");
    
    // Initialize CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_EXECUTION,
            "Failed to initialize CURL"
        );
    }
    
    // Initialize response buffer
    http_response_buffer_t response_buffer = {0};
    response_buffer.capacity = 4096;
    response_buffer.data = gs_malloc(response_buffer.capacity);
    response_buffer.max_size = max_size;
    response_buffer.truncated = false;
    
    // Configure CURL
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "gs_core_ai_http_tool/1.0");
    
    // Add custom headers if provided
    struct curl_slist* header_list = NULL;
    if (headers_item && cJSON_IsObject(headers_item)) {
        cJSON* header = NULL;
        cJSON_ArrayForEach(header, headers_item) {
            if (header->string && cJSON_IsString(header)) {
                char header_str[1024];
                snprintf(header_str, sizeof(header_str), "%s: %s", 
                         header->string, header->valuestring);
                header_list = curl_slist_append(header_list, header_str);
            }
        }
        if (header_list) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }
    }
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    
    // Get response info
    long status_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    
    char* content_type = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    
    double total_time = 0;
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
    
    // Cleanup CURL
    if (header_list) {
        curl_slist_free_all(header_list);
    }
    curl_easy_cleanup(curl);
    cJSON_Delete(args);
    
    // Check for CURL errors
    if (res != CURLE_OK) {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), 
                 "HTTP request failed: %s", curl_easy_strerror(res));
        gs_free(response_buffer.data);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_EXECUTION,
            error_msg
        );
    }
    
    // Build result JSON
    cJSON* result = cJSON_CreateObject();
    cJSON_AddNumberToObject(result, "status_code", (double)status_code);
    cJSON_AddStringToObject(result, "content", 
                            response_buffer.data ? response_buffer.data : "");
    cJSON_AddStringToObject(result, "content_type", 
                            content_type ? content_type : "unknown");
    cJSON_AddNumberToObject(result, "size", (double)response_buffer.size);
    cJSON_AddBoolToObject(result, "truncated", response_buffer.truncated);
    cJSON_AddNumberToObject(result, "duration_ms", total_time * 1000.0);
    
    char* result_str = cJSON_PrintUnformatted(result);
    cJSON_Delete(result);
    gs_free(response_buffer.data);
    
    return gs_core_ai_tool_result_success(result_str);
}

/*================================================================================
// Tool Registration
================================================================================*/

GS_API_DECL void 
gs_core_ai_tools_http_register(gs_core_ai_coordinator_t* coordinator) {
    if (!coordinator) return;
    
    // Register http_get tool
    gs_core_ai_tool_t http_get_tool = {0};
    http_get_tool.name = "http_get";
    http_get_tool.description = "Make an HTTP GET request to a URL and return the response. "
        "Useful for fetching web pages, API data, or any HTTP resource.";
    http_get_tool.parameters_schema = 
        "{"
        "  \"type\": \"object\","
        "  \"properties\": {"
        "    \"url\": {"
        "      \"type\": \"string\","
        "      \"description\": \"URL to make GET request to (must start with http:// or https://)\""
        "    },"
        "    \"timeout\": {"
        "      \"type\": \"number\","
        "      \"description\": \"Request timeout in seconds (default: 30)\""
        "    },"
        "    \"max_size\": {"
        "      \"type\": \"number\","
        "      \"description\": \"Maximum response size in bytes (default: 1048576 = 1MB)\""
        "    },"
        "    \"headers\": {"
        "      \"type\": \"object\","
        "      \"description\": \"Optional HTTP headers as key-value pairs\""
        "    }"
        "  },"
        "  \"required\": [\"url\"]"
        "}";
    http_get_tool.execute = gs_core_ai_tool_http_get_execute;
    http_get_tool.user_data = NULL;
    
    gs_core_ai_coordinator_register_tool(coordinator, http_get_tool);
}
