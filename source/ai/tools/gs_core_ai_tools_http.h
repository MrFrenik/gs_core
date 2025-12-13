/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_http.h
    * 
    * HTTP Tools for AI Coordinator
    * 
    * Provides a set of HTTP tools that can be registered with the AI coordinator.
    * Includes tools for making HTTP GET requests with URL validation, timeout handling,
    * and response size limits.

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

#ifndef GS_CORE_AI_TOOLS_HTTP_H
#define GS_CORE_AI_TOOLS_HTTP_H

/*================================================================================
// Includes
================================================================================*/

#include <gs/gs.h>
#include "../gs_core_ai_tool.h"
#include "../gs_core_ai_coordinator.h"

/*================================================================================
// Tool Registration
================================================================================*/

/**
 * Register all HTTP tools with the coordinator.
 * 
 * This registers the following tools:
 * - http_get: Make an HTTP GET request to a URL
 * 
 * @param coordinator The coordinator to register tools with
 */
GS_API_DECL void 
gs_core_ai_tools_http_register(gs_core_ai_coordinator_t* coordinator);

/*================================================================================
// Individual Tool Execute Functions
================================================================================*/

/**
 * http_get tool: Make an HTTP GET request to a URL
 * 
 * Arguments (JSON):
 * - url (string, required): URL to make GET request to (must be http:// or https://)
 * - timeout (number, optional): Request timeout in seconds (default: 30)
 * - max_size (number, optional): Maximum response size in bytes (default: 1MB)
 * - headers (object, optional): Key-value pairs of HTTP headers to include
 * 
 * Returns (JSON):
 * - status_code (number): HTTP status code (200, 404, etc.)
 * - content (string): Response body content
 * - content_type (string): Content-Type header value
 * - size (number): Response size in bytes
 * - truncated (bool): Whether response was truncated due to size limit
 * - duration_ms (number): Request duration in milliseconds
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_http_get_execute(void* user_data, const char* args_json);

#endif // GS_CORE_AI_TOOLS_HTTP_H
