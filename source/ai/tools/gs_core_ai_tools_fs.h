/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_tools_fs.h
    * 
    * File System Tools for AI Coordinator
    * 
    * Provides a set of file system tools that can be registered with the AI coordinator.
    * Includes tools for reading files, listing directories, checking file existence, and searching files.

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

#ifndef GS_CORE_AI_TOOLS_FS_H
#define GS_CORE_AI_TOOLS_FS_H

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
 * Register all file system tools with the coordinator.
 * 
 * This registers the following tools:
 * - read_file: Read and return the contents of a file
 * - list_directory: List files and directories with regex-based extension filtering
 * - file_exists: Check if a file or directory exists
 * - search_files: Search for files containing a PCRE2 regex pattern
 * 
 * All tools use PCRE2 regex for enhanced pattern matching and validation.
 * 
 * @param coordinator The coordinator to register tools with
 */
GS_API_DECL void 
gs_core_ai_tools_fs_register(gs_core_ai_coordinator_t* coordinator);

/*================================================================================
// Individual Tool Execute Functions
================================================================================*/

/**
 * read_file tool: Read and return the contents of a file
 * 
 * Arguments (JSON):
 * - path (string, required): Path to the file to read
 * - max_size (number, optional): Maximum size in bytes to read (default: 1MB)
 * 
 * Returns (JSON):
 * - content (string): File contents
 * - size (number): File size in bytes
 * - is_binary (bool): Whether the file appears to be binary
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_read_file_execute(void* user_data, const char* args_json);

/**
 * list_directory tool: List files and directories in a directory
 * 
 * Arguments (JSON):
 * - path (string, required): Path to the directory to list
 * - recursive (bool, optional): Whether to list recursively (default: false)
 * - filter (string, optional): File extension filter (supports wildcards like "*.c" or comma-separated ".c,.h")
 * 
 * Returns (JSON):
 * - entries (array): Array of file/directory entries
 *   - name (string): Entry name
 *   - path (string): Full path
 *   - is_directory (bool): Whether it's a directory
 *   - size (number): File size in bytes (0 for directories)
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_list_directory_execute(void* user_data, const char* args_json);

/**
 * file_exists tool: Check if a file or directory exists
 * 
 * Arguments (JSON):
 * - path (string, required): Path to check
 * 
 * Returns (JSON):
 * - exists (bool): Whether the path exists
 * - is_directory (bool): Whether it's a directory (if exists)
 * - size (number): File size in bytes (if exists and is file)
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_file_exists_execute(void* user_data, const char* args_json);

/**
 * search_files tool: Search for files containing a pattern using PCRE2 regex
 * 
 * Arguments (JSON):
 * - path (string, required): Directory to search in
 * - pattern (string, required): PCRE2 regex pattern to search for (full regex support)
 * - recursive (bool, optional): Whether to search recursively (default: true)
 * - filter (string, optional): File extension filter (supports wildcards like "*.c" or comma-separated ".c,.h")
 * - max_results (number, optional): Maximum number of results (default: 100)
 * 
 * Returns (JSON):
 * - results (array): Array of search results
 *   - file (string): File path
 *   - line (number): Line number
 *   - content (string): Line content
 *   - match_start (number): Start position of match in line
 *   - match_length (number): Length of match
 * - total_matches (number): Total number of matches found
 * - truncated (bool): Whether results were truncated
 */
GS_API_DECL gs_core_ai_tool_result_t 
gs_core_ai_tool_fs_search_files_execute(void* user_data, const char* args_json);

#endif // GS_CORE_AI_TOOLS_FS_H
