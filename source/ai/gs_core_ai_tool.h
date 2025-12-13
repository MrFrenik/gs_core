/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tool.h
    * 
    * AI Coordinator Tool Interface
    * 
    * Defines the interface for creating and registering tools that can be invoked by the AI coordinator.
    * Tools are functions that the AI can call to perform actions or retrieve information.

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

#ifndef GS_CORE_AI_TOOL_H
#define GS_CORE_AI_TOOL_H

/*================================================================================
// Includes
================================================================================*/

#include <gs/gs.h>

/*================================================================================
// Tool Result Types
================================================================================*/

typedef enum gs_core_ai_tool_result_code_e {
    GS_CORE_AI_TOOL_SUCCESS = 0,        // Tool executed successfully
    GS_CORE_AI_TOOL_ERROR_INVALID_ARGS, // Invalid arguments provided
    GS_CORE_AI_TOOL_ERROR_EXECUTION,    // Error during execution
    GS_CORE_AI_TOOL_ERROR_TIMEOUT,      // Tool execution timed out
    GS_CORE_AI_TOOL_ERROR_NOT_FOUND,    // Requested resource not found
    GS_CORE_AI_TOOL_ERROR_PERMISSION,   // Permission denied
    GS_CORE_AI_TOOL_ERROR_OUT_OF_MEMORY // Memory allocation failed
} gs_core_ai_tool_result_code_t;

typedef struct gs_core_ai_tool_result_t {
    gs_core_ai_tool_result_code_t code; // Result status code
    char* data;                          // Result data (JSON or plain text), owned by this struct
    char* error_message;                 // Error message if code != SUCCESS, owned by this struct
} gs_core_ai_tool_result_t;

/*================================================================================
// Tool Definition
================================================================================*/

typedef gs_core_ai_tool_result_t (*gs_core_ai_tool_execute_fn)(void* user_data, const char* args_json);

typedef struct gs_core_ai_tool_t {
    const char* name;              // Tool name (must be unique, string literal or stable pointer)
    const char* description;       // Human-readable description (string literal or stable pointer)
    const char* parameters_schema; // JSON schema describing parameters (string literal or stable pointer)
    gs_core_ai_tool_execute_fn execute; // Function to execute the tool
    void* user_data;               // Optional context data passed to execute function (externally owned)
} gs_core_ai_tool_t;

/*================================================================================
// Tool Result Helper Functions
================================================================================*/

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_result_success(const char* data);

GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_result_error(gs_core_ai_tool_result_code_t code, const char* error_message);

GS_API_DECL void 
gs_core_ai_tool_result_free(gs_core_ai_tool_result_t* result);

GS_API_DECL bool 
gs_core_ai_tool_result_is_success(const gs_core_ai_tool_result_t* result);

GS_API_DECL const char* 
gs_core_ai_tool_result_code_string(gs_core_ai_tool_result_code_t code);

/*================================================================================
// Example Tool Stub (for documentation purposes)
================================================================================*/

#if 0
/*
 * Example: Simple "echo" tool that returns its input
 */

// Tool execution function
gs_core_ai_tool_result_t example_echo_tool_execute(void* user_data, const char* args_json) {
    gs_core_ai_tool_result_t result = {0};
    
    // Parse JSON arguments
    cJSON* args = cJSON_Parse(args_json);
    if (!args) {
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Failed to parse JSON arguments"
        );
    }
    
    // Get "message" parameter
    cJSON* message_item = cJSON_GetObjectItem(args, "message");
    if (!message_item || !cJSON_IsString(message_item)) {
        cJSON_Delete(args);
        return gs_core_ai_tool_result_error(
            GS_CORE_AI_TOOL_ERROR_INVALID_ARGS,
            "Missing or invalid 'message' parameter"
        );
    }
    
    const char* message = message_item->valuestring;
    
    // Create response
    char* response = gs_malloc(strlen(message) + 100);
    snprintf(response, strlen(message) + 100, "Echo: %s", message);
    
    // Create success result
    result = gs_core_ai_tool_result_success(response);
    
    // Cleanup
    gs_free(response);
    cJSON_Delete(args);
    
    return result;
}

// Tool registration
void register_echo_tool(gs_core_ai_coordinator_t* coordinator) {
    gs_core_ai_tool_t echo_tool = {
        .name = "echo",
        .description = "Echoes back the provided message",
        .parameters_schema = "{\"message\": \"string\"}",
        .execute = example_echo_tool_execute,
        .user_data = NULL
    };
    
    gs_core_ai_coordinator_register_tool(coordinator, echo_tool);
}
#endif

#endif // GS_CORE_AI_TOOL_H
