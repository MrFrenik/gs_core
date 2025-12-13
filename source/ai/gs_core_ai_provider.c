/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_provider.c
    * 
    * AI Coordinator Provider Interface Implementation

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

#include "gs_core_ai_provider.h"

/*================================================================================
// Provider Response Helper Functions Implementation
================================================================================*/

GS_API_DECL gs_core_ai_provider_response_t 
gs_core_ai_provider_response_success( const char* content, const 
    gs_core_ai_provider_response_metadata_t* metadata)
{
    gs_core_ai_provider_response_t response = {0};
    response.status = GS_CORE_AI_PROVIDER_SUCCESS;
    
    // Copy content
    if (content) {
        size_t len = strlen(content);
        response.content = (char*)gs_malloc(len + 1);
        if (response.content) {
            memcpy(response.content, content, len);
            response.content[len] = '\0';
        }
    }
    
    response.error_message = NULL;
    
    // Copy metadata if provided
    if (metadata) {
        response.metadata = *metadata;
    }
    
    return response;
}

GS_API_DECL gs_core_ai_provider_response_t 
gs_core_ai_provider_response_error( gs_core_ai_provider_status_t status, 
    const char* error_message)
{
    gs_core_ai_provider_response_t response = {0};
    response.status = status;
    response.content = NULL;
    
    // Copy error message
    if (error_message) {
        size_t len = strlen(error_message);
        response.error_message = (char*)gs_malloc(len + 1);
        if (response.error_message) {
            memcpy(response.error_message, error_message, len);
            response.error_message[len] = '\0';
        }
    }
    
    // Initialize metadata to zero
    memset(&response.metadata, 0, sizeof(response.metadata));
    
    return response;
}

GS_API_DECL void 
gs_core_ai_provider_response_free(gs_core_ai_provider_response_t* response)
{
    if (!response) return;
    
    if (response->content) {
        gs_free(response->content);
        response->content = NULL;
    }
    
    if (response->error_message) {
        gs_free(response->error_message);
        response->error_message = NULL;
    }
    
    // Note: metadata fields (model, finish_reason) are not owned by response
    // They are typically static strings or owned by the provider
    
    response->status = GS_CORE_AI_PROVIDER_SUCCESS;
    memset(&response->metadata, 0, sizeof(response->metadata));
}

GS_API_DECL bool 
gs_core_ai_provider_response_is_success(const gs_core_ai_provider_response_t* response)
{
    return response && response->status == GS_CORE_AI_PROVIDER_SUCCESS;
}

GS_API_DECL const char* 
gs_core_ai_provider_status_string(gs_core_ai_provider_status_t status)
{
    switch (status) {
        case GS_CORE_AI_PROVIDER_SUCCESS:               return "Success";
        case GS_CORE_AI_PROVIDER_ERROR_CONNECTION:      return "Connection error";
        case GS_CORE_AI_PROVIDER_ERROR_TIMEOUT:         return "Timeout";
        case GS_CORE_AI_PROVIDER_ERROR_AUTH:            return "Authentication failed";
        case GS_CORE_AI_PROVIDER_ERROR_INVALID_MODEL:   return "Invalid model";
        case GS_CORE_AI_PROVIDER_ERROR_INVALID_REQUEST: return "Invalid request";
        case GS_CORE_AI_PROVIDER_ERROR_RATE_LIMIT:      return "Rate limit exceeded";
        case GS_CORE_AI_PROVIDER_ERROR_SERVER:          return "Server error";
        case GS_CORE_AI_PROVIDER_ERROR_PARSE:           return "Parse error";
        case GS_CORE_AI_PROVIDER_ERROR_OUT_OF_MEMORY:   return "Out of memory";
        case GS_CORE_AI_PROVIDER_ERROR_UNKNOWN:         return "Unknown error";
        default:                                        return "Unrecognized error";
    }
}

GS_API_DECL void 
gs_core_ai_provider_model_info_free(gs_core_ai_provider_model_info_t* models, uint32_t count)
{
    if (!models) return;
    
    for (uint32_t i = 0; i < count; i++) {
        if (models[i].name) {
            gs_free(models[i].name);
            models[i].name = NULL;
        }
        if (models[i].description) {
            gs_free(models[i].description);
            models[i].description = NULL;
        }
    }
    
    gs_free(models);
}
