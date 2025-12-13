/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_code.h
    * 
    * Code Search Tools for AI Coordinator
    * 
    * Provides a set of code search and analysis tools that can be registered with the AI coordinator.
    * Includes tools for searching code, listing functions, and finding symbol definitions.

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

#ifndef GS_CORE_AI_TOOLS_CODE_H
#define GS_CORE_AI_TOOLS_CODE_H

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
 * Register all code search tools with the coordinator.
 * 
 * This registers the following tools:
 * - search_code: Search for code patterns using regex
 * - list_functions: List function definitions in source files
 * - find_definition: Find the definition of a symbol
 * 
 * @param coordinator The coordinator to register tools with
 */
GS_API_DECL void 
gs_core_ai_tools_code_register(gs_core_ai_coordinator_t* coordinator);

/*================================================================================
// Individual Tool Execute Functions
================================================================================*/

/**
 * search_code tool: Search for code patterns using regex
 * 
 * Arguments (JSON):
 * - path (string, required): Directory to search in
 * - pattern (string, required): Regex pattern to search for
 * - recursive (bool, optional): Whether to search recursively (default: true)
 * - filter (string, optional): File extension filter (e.g., ".c,.h,.cpp")
 * - max_results (number, optional): Maximum number of results (default: 100)
 * - context_lines (number, optional): Number of context lines before/after match (default: 2)
 * 
 * Returns (JSON):
 * - results (array): Array of search results
 *   - file (string): File path
 *   - line (number): Line number
 *   - column (number): Column number
 *   - match (string): Matched text
 *   - context_before (array): Array of lines before match
 *   - context_after (array): Array of lines after match
 * - total_matches (number): Total number of matches found
 * - truncated (bool): Whether results were truncated
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_search_code_execute(void* user_data, const char* args_json);

/**
 * list_functions tool: List function definitions in source files
 * 
 * Arguments (JSON):
 * - path (string, required): Directory or file to analyze
 * - recursive (bool, optional): Whether to search recursively (default: true)
 * - filter (string, optional): File extension filter (default: ".c,.h,.cpp,.hpp")
 * - max_results (number, optional): Maximum number of results (default: 200)
 * 
 * Returns (JSON):
 * - functions (array): Array of function definitions
 *   - name (string): Function name
 *   - signature (string): Full function signature
 *   - file (string): File path
 *   - line (number): Line number
 *   - return_type (string): Return type (if parseable)
 * - total_functions (number): Total number of functions found
 * - truncated (bool): Whether results were truncated
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_list_functions_execute(void* user_data, const char* args_json);

/**
 * find_definition tool: Find the definition of a symbol (function, struct, typedef, etc.)
 * 
 * Arguments (JSON):
 * - symbol (string, required): Symbol name to find
 * - path (string, optional): Directory to search in (default: current working directory)
 * - recursive (bool, optional): Whether to search recursively (default: true)
 * - filter (string, optional): File extension filter (default: ".c,.h,.cpp,.hpp")
 * 
 * Returns (JSON):
 * - found (bool): Whether the definition was found
 * - definitions (array): Array of possible definitions
 *   - type (string): Definition type (function, struct, typedef, enum, macro, variable)
 *   - name (string): Symbol name
 *   - signature (string): Full signature or declaration
 *   - file (string): File path
 *   - line (number): Line number
 *   - context (array): Array of context lines around definition
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_code_find_definition_execute(void* user_data, const char* args_json);

#endif // GS_CORE_AI_TOOLS_CODE_H
