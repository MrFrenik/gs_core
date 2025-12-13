/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tool.c
    * 
    * AI Coordinator Tool Interface Implementation

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

#include "gs_core_ai_tool.h"

/*================================================================================
// Tool Result Helper Functions Implementation
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_result_success(const char* data)
{
    gs_core_ai_tool_result_t result = {0};
    result.code = GS_CORE_AI_TOOL_SUCCESS;
    
    if (data) {
        size_t len = strlen(data);
        result.data = (char*)gs_malloc(len + 1);
        if (result.data) {
            memcpy(result.data, data, len);
            result.data[len] = '\0';
        }
    }
    
    result.error_message = NULL;
    return result;
}

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_result_error(gs_core_ai_tool_result_code_t code, const char* error_message)
{
    gs_core_ai_tool_result_t result = {0};
    result.code = code;
    result.data = NULL;
    
    if (error_message) {
        size_t len = strlen(error_message);
        result.error_message = (char*)gs_malloc(len + 1);
        if (result.error_message) {
            memcpy(result.error_message, error_message, len);
            result.error_message[len] = '\0';
        }
    }
    
    return result;
}

GS_API_DECL void 
gs_core_ai_tool_result_free(gs_core_ai_tool_result_t* result)
{
    if (!result) return;
    
    if (result->data) {
        gs_free(result->data);
        result->data = NULL;
    }
    
    if (result->error_message) {
        gs_free(result->error_message);
        result->error_message = NULL;
    }
    
    result->code = GS_CORE_AI_TOOL_SUCCESS;
}

GS_API_DECL bool 
gs_core_ai_tool_result_is_success(const gs_core_ai_tool_result_t* result)
{
    return result && result->code == GS_CORE_AI_TOOL_SUCCESS;
}

GS_API_DECL const char* 
gs_core_ai_tool_result_code_string(gs_core_ai_tool_result_code_t code)
{
    switch (code) {
        case GS_CORE_AI_TOOL_SUCCESS:               return "Success";
        case GS_CORE_AI_TOOL_ERROR_INVALID_ARGS:    return "Invalid arguments";
        case GS_CORE_AI_TOOL_ERROR_EXECUTION:       return "Execution error";
        case GS_CORE_AI_TOOL_ERROR_TIMEOUT:         return "Timeout";
        case GS_CORE_AI_TOOL_ERROR_NOT_FOUND:       return "Not found";
        case GS_CORE_AI_TOOL_ERROR_PERMISSION:      return "Permission denied";
        case GS_CORE_AI_TOOL_ERROR_OUT_OF_MEMORY:   return "Out of memory";
        default:                                    return "Unknown error";
    }
}
