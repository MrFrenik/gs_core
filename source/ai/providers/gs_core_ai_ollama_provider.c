/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_ollama_provider.c
    * 
    * Ollama Provider Implementation

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

#include "gs_core_ai_ollama_provider.h"
#include <cJSON/cJSON.h>
#include <curl/curl.h>
#include <string.h>
#include <stdio.h>

/*================================================================================
// Provider-Specific Data
================================================================================*/

typedef struct gs_core_ai_ollama_provider_data_t {
    char* base_url;              // Base URL for Ollama API (owned)
    char* api_endpoint;          // Full API endpoint URL (owned)
    uint32_t timeout_seconds;    // Request timeout
} gs_core_ai_ollama_provider_data_t;

typedef struct gs_core_ai_ollama_curl_response_t {
    char* data;                  // Response data (owned)
    size_t size;                 // Response size in bytes
} gs_core_ai_ollama_curl_response_t;

/*================================================================================
// Internal Helpers
================================================================================*/

static size_t 
_ollama_curl_write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total = size * nmemb;
    gs_core_ai_ollama_curl_response_t* resp = (gs_core_ai_ollama_curl_response_t*)userp;
    
    char* new_data = (char*)gs_realloc(resp->data, resp->size + total + 1);
    if (!new_data) {
        gs_println("[Ollama] ERROR: Out of memory in write callback");
        return 0;
    }
    
    resp->data = new_data;
    memcpy(&(resp->data[resp->size]), contents, total);
    resp->size += total;
    resp->data[resp->size] = 0; // Null-terminate
    
    return total;
}

static gs_core_ai_provider_response_t 
_ollama_parse_response(const char* response_json, const char* model_name, float duration_ms)
{
    cJSON* json = cJSON_Parse(response_json);
    if (!json) {
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_PARSE,
            "Failed to parse Ollama JSON response"
        );
    }
    
    // Check for error field
    cJSON* error = cJSON_GetObjectItem(json, "error");
    if (error && cJSON_IsString(error)) {
        gs_core_ai_provider_response_t result = gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_SERVER,
            error->valuestring
        );
        cJSON_Delete(json);
        return result;
    }
    
    // Extract response text
    cJSON* response_text = cJSON_GetObjectItem(json, "response");
    if (!response_text || !cJSON_IsString(response_text)) {
        cJSON_Delete(json);
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_PARSE,
            "Missing or invalid 'response' field in Ollama response"
        );
    }
    
    // Extract metadata
    gs_core_ai_provider_response_metadata_t metadata = {0};
    metadata.model = model_name;
    metadata.duration_ms = duration_ms;
    
    // Extract token counts if available
    cJSON* prompt_eval_count = cJSON_GetObjectItem(json, "prompt_eval_count");
    if (prompt_eval_count && cJSON_IsNumber(prompt_eval_count)) {
        metadata.prompt_tokens = (uint32_t)prompt_eval_count->valueint;
    }
    
    cJSON* eval_count = cJSON_GetObjectItem(json, "eval_count");
    if (eval_count && cJSON_IsNumber(eval_count)) {
        metadata.completion_tokens = (uint32_t)eval_count->valueint;
    }
    
    metadata.total_tokens = metadata.prompt_tokens + metadata.completion_tokens;
    
    // Check if done
    cJSON* done = cJSON_GetObjectItem(json, "done");
    if (done && cJSON_IsBool(done)) {
        metadata.finish_reason_complete = cJSON_IsTrue(done);
    }
    
    // Extract finish reason if available
    cJSON* done_reason = cJSON_GetObjectItem(json, "done_reason");
    if (done_reason && cJSON_IsString(done_reason)) {
        metadata.finish_reason = done_reason->valuestring;
    } else {
        metadata.finish_reason = metadata.finish_reason_complete ? "stop" : "incomplete";
    }
    
    // Create success response
    gs_core_ai_provider_response_t result = gs_core_ai_provider_response_success(
        response_text->valuestring,
        &metadata
    );
    
    cJSON_Delete(json);
    return result;
}

/*================================================================================
// Provider Interface Implementation
================================================================================*/

static gs_core_ai_provider_response_t 
_ollama_provider_request(
    gs_core_ai_provider_t* provider,
    const char* prompt,
    const gs_core_ai_provider_config_t* config)
{
    gs_core_ai_ollama_provider_data_t* data = (gs_core_ai_ollama_provider_data_t*)provider->provider_data;
    
    if (!prompt) {
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_INVALID_REQUEST,
            "Prompt cannot be NULL"
        );
    }
    
    if (!config || !config->model) {
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_INVALID_REQUEST,
            "Config and model must be specified"
        );
    }
    
    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_CONNECTION,
            "Failed to initialize CURL"
        );
    }
    
    // Build JSON request
    cJSON* request = cJSON_CreateObject();
    if (!request) {
        curl_easy_cleanup(curl);
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_OUT_OF_MEMORY,
            "Failed to create JSON request object"
        );
    }
    
    cJSON_AddStringToObject(request, "model", config->model);
    cJSON_AddStringToObject(request, "prompt", prompt);
    cJSON_AddBoolToObject(request, "stream", false);
    
    // Add optional parameters
    if (config->temperature != 0.7f || config->num_ctx > 0) {
        cJSON* options = cJSON_CreateObject();
        if (config->temperature != 0.7f) {
            cJSON_AddNumberToObject(options, "temperature", config->temperature);
        }
        if (config->num_ctx > 0) {
            cJSON_AddNumberToObject(options, "num_ctx", config->num_ctx);
        }
        cJSON_AddItemToObject(request, "options", options);
    }
    
    // Add system prompt if provided
    if (config->system_prompt) {
        cJSON_AddStringToObject(request, "system", config->system_prompt);
    }
    
    char* json_str = cJSON_PrintUnformatted(request);
    if (!json_str) {
        cJSON_Delete(request);
        curl_easy_cleanup(curl);
        return gs_core_ai_provider_response_error(
            GS_CORE_AI_PROVIDER_ERROR_OUT_OF_MEMORY,
            "Failed to serialize JSON request"
        );
    }
    
    // Setup response buffer
    gs_core_ai_ollama_curl_response_t response = {0};
    
    // Setup curl request
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, data->api_endpoint);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _ollama_curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    // Set timeout
    uint32_t timeout = config->timeout_seconds > 0 ? config->timeout_seconds : data->timeout_seconds;
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)timeout);
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    
    // Get timing info from curl (duration in seconds)
    double duration_sec = 0;
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &duration_sec);
    float duration_ms = (float)(duration_sec * 1000.0);
    
    // Check for curl errors
    gs_core_ai_provider_response_t result;
    if (res != CURLE_OK) {
        const char* error_str = curl_easy_strerror(res);
        
        // Determine error type
        gs_core_ai_provider_status_t status;
        if (res == CURLE_OPERATION_TIMEDOUT) {
            status = GS_CORE_AI_PROVIDER_ERROR_TIMEOUT;
        } else if (res == CURLE_COULDNT_CONNECT || res == CURLE_COULDNT_RESOLVE_HOST) {
            status = GS_CORE_AI_PROVIDER_ERROR_CONNECTION;
        } else {
            status = GS_CORE_AI_PROVIDER_ERROR_UNKNOWN;
        }
        
        // Build error message
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "CURL error: %s", error_str);
        result = gs_core_ai_provider_response_error(status, error_msg);
    } else {
        // Check HTTP status code
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (http_code == 200 && response.data) {
            // Parse successful response
            result = _ollama_parse_response(response.data, config->model, duration_ms);
        } else {
            // Handle HTTP error
            gs_core_ai_provider_status_t status;
            if (http_code == 404) {
                status = GS_CORE_AI_PROVIDER_ERROR_INVALID_MODEL;
            } else if (http_code == 401 || http_code == 403) {
                status = GS_CORE_AI_PROVIDER_ERROR_AUTH;
            } else if (http_code == 429) {
                status = GS_CORE_AI_PROVIDER_ERROR_RATE_LIMIT;
            } else if (http_code >= 500) {
                status = GS_CORE_AI_PROVIDER_ERROR_SERVER;
            } else {
                status = GS_CORE_AI_PROVIDER_ERROR_UNKNOWN;
            }
            
            char error_msg[512];
            if (response.data) {
                snprintf(error_msg, sizeof(error_msg), 
                    "HTTP %ld: %s", http_code, response.data);
            } else {
                snprintf(error_msg, sizeof(error_msg), 
                    "HTTP %ld: No response data", http_code);
            }
            result = gs_core_ai_provider_response_error(status, error_msg);
        }
    }
    
    // Cleanup
    if (response.data) gs_free(response.data);
    curl_slist_free_all(headers);
    gs_free(json_str);
    cJSON_Delete(request);
    curl_easy_cleanup(curl);
    
    return result;
}

static gs_core_ai_provider_model_info_t* 
_ollama_provider_get_model_info(
    gs_core_ai_provider_t* provider,
    const char* model_name,
    uint32_t* out_count)
{
    // Not implemented yet - would query /api/tags endpoint
    if (out_count) *out_count = 0;
    return NULL;
}

static void 
_ollama_provider_free(gs_core_ai_provider_t* provider)
{
    if (!provider) return;
    
    gs_core_ai_ollama_provider_data_t* data = 
        (gs_core_ai_ollama_provider_data_t*)provider->provider_data;
    
    if (data) {
        if (data->base_url) gs_free(data->base_url);
        if (data->api_endpoint) gs_free(data->api_endpoint);
        gs_free(data);
    }
    
    gs_free(provider);
}

/*================================================================================
// Public API
================================================================================*/

gs_core_ai_provider_t* 
gs_core_ai_ollama_provider_create(const gs_core_ai_ollama_config_t* config)
{
    // Use default config if none provided
    gs_core_ai_ollama_config_t default_config = GS_CORE_AI_OLLAMA_CONFIG_DEFAULT();
    if (!config) {
        config = &default_config;
    }
    
    // Initialize curl global (should be called once per application)
    // Note: In production, this should be handled at application level
    static bool curl_initialized = false;
    if (!curl_initialized) {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
            gs_println("[Ollama] ERROR: Failed to initialize CURL globally");
            return NULL;
        }
        curl_initialized = true;
    }
    
    // Allocate provider
    gs_core_ai_provider_t* provider = gs_malloc(sizeof(gs_core_ai_provider_t));
    if (!provider) {
        return NULL;
    }
    
    // Allocate provider data
    gs_core_ai_ollama_provider_data_t* data = gs_malloc(sizeof(gs_core_ai_ollama_provider_data_t));
    if (!data) {
        gs_free(provider);
        return NULL;
    }
    
    // Copy base URL
    size_t url_len = strlen(config->base_url);
    data->base_url = gs_malloc(url_len + 1);
    if (!data->base_url) {
        gs_free(data);
        gs_free(provider);
        return NULL;
    }
    strcpy(data->base_url, config->base_url);
    
    // Build API endpoint URL
    const char* endpoint = "/api/generate";
    size_t endpoint_len = url_len + strlen(endpoint) + 1;
    data->api_endpoint = gs_malloc(endpoint_len);
    if (!data->api_endpoint) {
        gs_free(data->base_url);
        gs_free(data);
        gs_free(provider);
        return NULL;
    }
    snprintf(data->api_endpoint, endpoint_len, "%s%s", config->base_url, endpoint);
    
    data->timeout_seconds = config->timeout_seconds > 0 ? config->timeout_seconds : 120;
    
    // Setup provider interface
    provider->provider_data = data;
    provider->request = _ollama_provider_request;
    provider->get_model_info = _ollama_provider_get_model_info;
    provider->free = _ollama_provider_free;
    
    return provider;
}
