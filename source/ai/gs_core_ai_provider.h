/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_provider.h
    * 
    * AI Coordinator Provider Interface
    * 
    * Defines the interface for LLM providers (Ollama, OpenAI, Claude, etc.) that can be used
    * by the AI coordinator. This abstraction allows swapping different LLM backends without
    * changing coordinator code.

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

#ifndef GS_CORE_AI_PROVIDER_H
#define GS_CORE_AI_PROVIDER_H

/*================================================================================
// Includes
================================================================================*/

#include <gs/gs.h>

/*================================================================================
// Provider Response Types
================================================================================*/

typedef enum gs_core_ai_provider_status_e {
    GS_CORE_AI_PROVIDER_SUCCESS = 0,           // Request successful
    GS_CORE_AI_PROVIDER_ERROR_CONNECTION,      // Connection failed
    GS_CORE_AI_PROVIDER_ERROR_TIMEOUT,         // Request timed out
    GS_CORE_AI_PROVIDER_ERROR_AUTH,            // Authentication failed
    GS_CORE_AI_PROVIDER_ERROR_INVALID_MODEL,   // Model not found or invalid
    GS_CORE_AI_PROVIDER_ERROR_INVALID_REQUEST, // Malformed request
    GS_CORE_AI_PROVIDER_ERROR_RATE_LIMIT,      // Rate limit exceeded
    GS_CORE_AI_PROVIDER_ERROR_SERVER,          // Server error (5xx)
    GS_CORE_AI_PROVIDER_ERROR_PARSE,           // Failed to parse response
    GS_CORE_AI_PROVIDER_ERROR_OUT_OF_MEMORY,   // Memory allocation failed
    GS_CORE_AI_PROVIDER_ERROR_UNKNOWN          // Unknown error
} gs_core_ai_provider_status_t;

typedef struct gs_core_ai_provider_response_metadata_t {
    const char* model;               // Model name used (read-only, owned by response)
    uint32_t prompt_tokens;          // Number of tokens in prompt (0 if unavailable)
    uint32_t completion_tokens;      // Number of tokens in completion (0 if unavailable)
    uint32_t total_tokens;           // Total tokens used (0 if unavailable)
    float duration_ms;               // Request duration in milliseconds
    bool finish_reason_complete;     // True if response completed normally
    const char* finish_reason;       // Reason for completion (read-only, owned by response)
} gs_core_ai_provider_response_metadata_t;

typedef struct gs_core_ai_provider_response_t {
    gs_core_ai_provider_status_t status;              // Response status
    char* content;                                    // Response content (owned by this struct)
    char* error_message;                              // Error message if status != SUCCESS (owned by this struct)
    gs_core_ai_provider_response_metadata_t metadata; // Response metadata
} gs_core_ai_provider_response_t;

/*================================================================================
// Provider Configuration
================================================================================*/

typedef struct gs_core_ai_provider_config_t {
    const char* model;               // Model name (required)
    float temperature;               // Sampling temperature (0.0-2.0, default 0.7)
    uint32_t max_tokens;             // Max tokens in response (0 = provider default)
    uint32_t num_ctx;                // Context window size in tokens (0 = model default)
    uint32_t timeout_seconds;        // Request timeout in seconds (0 = no timeout)
    bool stream;                     // Enable streaming responses (not yet implemented)
    const char* system_prompt;       // System prompt (optional, can be NULL)
} gs_core_ai_provider_config_t;

#define GS_CORE_AI_PROVIDER_CONFIG_DEFAULT() (gs_core_ai_provider_config_t){ \
    .model = NULL,              \
    .temperature = 0.7f,        \
    .max_tokens = 0,            \
    .num_ctx = 0,               \
    .timeout_seconds = 120,     \
    .stream = false,            \
    .system_prompt = NULL       \
}

/*================================================================================
// Provider Model Information
================================================================================*/

typedef struct gs_core_ai_provider_model_info_t {
    char* name;                      // Model name (owned by this struct)
    char* description;               // Model description (owned by this struct)
    uint32_t context_length;         // Maximum context length in tokens
    bool supports_tools;             // Whether model supports tool calling
    bool supports_streaming;         // Whether model supports streaming
} gs_core_ai_provider_model_info_t;

/*================================================================================
// Provider Interface
================================================================================*/

// Forward declaration
typedef struct gs_core_ai_provider_t gs_core_ai_provider_t;

typedef gs_core_ai_provider_response_t (*gs_core_ai_provider_request_fn)(
    gs_core_ai_provider_t* provider,
    const char* prompt,
    const gs_core_ai_provider_config_t* config
);

typedef gs_core_ai_provider_model_info_t* (*gs_core_ai_provider_get_model_info_fn)(
    gs_core_ai_provider_t* provider,
    const char* model_name,
    uint32_t* out_count
);

typedef void (*gs_core_ai_provider_free_fn)(gs_core_ai_provider_t* provider);

struct gs_core_ai_provider_t {
    void* provider_data;                              // Provider-specific data (owned by provider)
    gs_core_ai_provider_request_fn request;           // Request function (required)
    gs_core_ai_provider_get_model_info_fn get_model_info; // Get model info (optional, can be NULL)
    gs_core_ai_provider_free_fn free;                 // Free function (required)
};

/*================================================================================
// Provider Response Helper Functions
================================================================================*/

GS_API_DECL gs_core_ai_provider_response_t 
gs_core_ai_provider_response_success(
    const char* content,
    const gs_core_ai_provider_response_metadata_t* metadata
);

GS_API_DECL gs_core_ai_provider_response_t 
gs_core_ai_provider_response_error(
    gs_core_ai_provider_status_t status,
    const char* error_message
);

GS_API_DECL void 
gs_core_ai_provider_response_free(gs_core_ai_provider_response_t* response);

GS_API_DECL bool 
gs_core_ai_provider_response_is_success(const gs_core_ai_provider_response_t* response);

GS_API_DECL const char* 
gs_core_ai_provider_status_string(gs_core_ai_provider_status_t status);

GS_API_DECL void 
gs_core_ai_provider_model_info_free(gs_core_ai_provider_model_info_t* models, uint32_t count);

/*================================================================================
// Example Provider Implementation (for documentation purposes)
================================================================================*/

#if 0
/*
 * Example: Minimal provider implementation
 */

// Provider-specific data
typedef struct my_provider_data_t {
    char* api_url;
    char* api_key;
    // ... other provider-specific fields
} my_provider_data_t;

// Request implementation
gs_core_ai_provider_response_t my_provider_request(
    gs_core_ai_provider_t* provider,
    const char* prompt,
    const gs_core_ai_provider_config_t* config)
{
    my_provider_data_t* data = (my_provider_data_t*)provider->provider_data;
    
    // Make HTTP request to provider API
    // Parse response
    // Return result
    
    gs_core_ai_provider_response_metadata_t metadata = {
        .model = config->model,
        .prompt_tokens = 100,
        .completion_tokens = 50,
        .total_tokens = 150,
        .duration_ms = 1234.5f,
        .finish_reason_complete = true,
        .finish_reason = "stop"
    };
    
    return gs_core_ai_provider_response_success("Response text", &metadata);
}

// Free implementation
void my_provider_free(gs_core_ai_provider_t* provider) {
    if (!provider) return;
    
    my_provider_data_t* data = (my_provider_data_t*)provider->provider_data;
    if (data) {
        if (data->api_url) gs_free(data->api_url);
        if (data->api_key) gs_free(data->api_key);
        gs_free(data);
    }
    
    gs_free(provider);
}

// Provider creation
gs_core_ai_provider_t* my_provider_create(const char* api_url, const char* api_key) {
    gs_core_ai_provider_t* provider = gs_malloc(sizeof(gs_core_ai_provider_t));
    
    my_provider_data_t* data = gs_malloc(sizeof(my_provider_data_t));
    data->api_url = gs_malloc(strlen(api_url) + 1);
    strcpy(data->api_url, api_url);
    data->api_key = gs_malloc(strlen(api_key) + 1);
    strcpy(data->api_key, api_key);
    
    provider->provider_data = data;
    provider->request = my_provider_request;
    provider->get_model_info = NULL; // Optional
    provider->free = my_provider_free;
    
    return provider;
}

// Usage
gs_core_ai_provider_t* provider = my_provider_create("https://api.example.com", "key123");
gs_core_ai_provider_config_t config = GS_CORE_AI_PROVIDER_CONFIG_DEFAULT();
config.model = "my-model";

gs_core_ai_provider_response_t response = provider->request(provider, "Hello!", &config);
if (gs_core_ai_provider_response_is_success(&response)) {
    printf("Response: %s\n", response.content);
}
gs_core_ai_provider_response_free(&response);
provider->free(provider);
#endif

#endif // GS_CORE_AI_PROVIDER_H
