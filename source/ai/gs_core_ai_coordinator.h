/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_coordinator.h
    * 
    * AI Coordinator - Main Orchestrator
    * 
    * The coordinator manages tool registration, conversation history, and orchestrates
    * the interaction between the LLM provider and registered tools. It implements the
    * tool-calling loop where the AI can invoke tools and receive results.

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

#ifndef GS_CORE_AI_COORDINATOR_H
#define GS_CORE_AI_COORDINATOR_H

/*================================================================================
// Includes
================================================================================*/

#include <gs/gs.h>
#include "gs_core_ai_tool.h"
#include "gs_core_ai_provider.h"

/*================================================================================
// Coordinator Query Result
================================================================================*/

typedef struct gs_core_ai_coordinator_result_t {
    bool success;                // True if query completed successfully
    char* response;              // Final response text (owned by this struct)
    char* error_message;         // Error message if failed (owned by this struct)
    uint32_t iterations;         // Number of tool-calling iterations performed
    uint32_t total_tokens;       // Total tokens used across all requests
} gs_core_ai_coordinator_result_t;

/*================================================================================
// Default Tools Configuration
================================================================================*/

typedef struct gs_core_ai_default_tools_config_t {
    bool enable_fs_tools;        // Enable file system tools (default: true)
    bool enable_http_tools;      // Enable HTTP tools (default: true)
    bool enable_code_tools;      // Enable code search tools (default: true)
} gs_core_ai_default_tools_config_t;

#define GS_CORE_AI_DEFAULT_TOOLS_CONFIG_DEFAULT() (gs_core_ai_default_tools_config_t){ \
    .enable_fs_tools = true,    \
    .enable_http_tools = true,  \
    .enable_code_tools = true   \
}

#define GS_CORE_AI_DEFAULT_TOOLS_CONFIG_ALL_DISABLED() (gs_core_ai_default_tools_config_t){ \
    .enable_fs_tools = false,   \
    .enable_http_tools = false, \
    .enable_code_tools = false  \
}

/*================================================================================
// Execution Mode
================================================================================*/

/**
 * Execution mode determines how the coordinator orchestrates tool calls.
 * 
 * - AGENTIC: Pure ReAct loop - model thinks, acts, observes, repeats
 *   No upfront planning. Model decides next action after seeing each result.
 *   Best for: Exploratory tasks, dynamic problem-solving
 * 
 * - PLAN_REFLECT: Plan upfront, then execute with reflection
 *   Model creates complete plan, executes steps, reflects after each result,
 *   and can revise remaining steps based on observations.
 *   Best for: Complex multi-step tasks requiring structured approach
 */
typedef enum gs_core_ai_execution_mode_e {
    GS_CORE_AI_MODE_AGENTIC,      // Pure ReAct: Thought → Action → Observation loop
    GS_CORE_AI_MODE_PLAN_REFLECT  // Plan upfront, reflect and adapt after each step
} gs_core_ai_execution_mode_t;

/*================================================================================
// Coordinator Configuration
================================================================================*/

typedef struct gs_core_ai_coordinator_config_t {
    uint32_t max_iterations;     // Maximum tool-calling iterations (default: 10)
    bool enable_debug_logging;   // Enable verbose debug output (default: false)
    const char* system_prompt;   // Custom system prompt (NULL = auto-generate from tools)
    
    // Execution mode configuration
    gs_core_ai_execution_mode_t execution_mode; // Execution strategy (default: PLAN_REFLECT)
    
    // Model parameters
    uint32_t num_ctx;            // Context window size in tokens (0 = use model default)
    
    // Temperature configuration
    float planning_temperature;  // Temperature for planning phase (default: 0.3 for deterministic plans)
    float execution_temperature; // Temperature for execution phase (default: 0.7 for creative tool use)
    
    // Reflection configuration (used in PLAN_REFLECT mode)
    bool enable_reflection;      // Whether to ask model to reflect after each step (default: true in PLAN_REFLECT)
    const char* reflection_prompt; // Custom reflection prompt (NULL = auto-generate)
} gs_core_ai_coordinator_config_t;

#define GS_CORE_AI_COORDINATOR_CONFIG_DEFAULT() (gs_core_ai_coordinator_config_t){ \
    .max_iterations = 10,           \
    .enable_debug_logging = false,  \
    .system_prompt = NULL,          \
    .execution_mode = GS_CORE_AI_MODE_PLAN_REFLECT, \
    .num_ctx = 0,                   \
    .planning_temperature = 0.3f,   \
    .execution_temperature = 0.7f,  \
    .enable_reflection = true,      \
    .reflection_prompt = NULL       \
}

/*================================================================================
// Coordinator Structure
================================================================================*/

typedef struct gs_core_ai_coordinator_t {
    gs_dyn_array(gs_core_ai_tool_t) tools;          // Registered tools
    gs_dyn_array(char*) conversation_history;       // Conversation messages
    gs_core_ai_provider_t* provider;                // LLM provider (owned by coordinator)
    gs_core_ai_coordinator_config_t config;         // Configuration
    char* generated_system_prompt;                  // Auto-generated system prompt
    char* current_plan;                             // Current execution plan (owned, can be NULL)
    gs_dyn_array(void*) current_plan_steps;         // Parsed plan steps array as cJSON* (owned)
    uint32_t current_step_index;                    // Current step being executed (0-based)
    
    // Reflection tracking (used in PLAN_REFLECT mode)
    char* last_reflection;                          // Most recent reflection from model (owned)
    uint32_t reflection_count;                      // Number of reflections in current query
    bool plan_revised;                              // Flag indicating plan was revised mid-execution
    
    // Thought tracking (used in AGENTIC mode)
    gs_dyn_array(char*) thoughts;                   // Thought history for ReAct pattern (owned)
} gs_core_ai_coordinator_t;

/*================================================================================
// Coordinator API
================================================================================*/

GS_API_DECL gs_core_ai_coordinator_t* 
gs_core_ai_coordinator_create(
    gs_core_ai_provider_t* provider,
    const gs_core_ai_coordinator_config_t* config
);

GS_API_DECL void 
gs_core_ai_coordinator_free(gs_core_ai_coordinator_t* coordinator);

GS_API_DECL bool 
gs_core_ai_coordinator_register_tool(
    gs_core_ai_coordinator_t* coordinator,
    gs_core_ai_tool_t tool
);

GS_API_DECL gs_core_ai_coordinator_result_t 
gs_core_ai_coordinator_query(
    gs_core_ai_coordinator_t* coordinator,
    const char* query,
    const gs_core_ai_provider_config_t* provider_config
);

GS_API_DECL void 
gs_core_ai_coordinator_clear_history(gs_core_ai_coordinator_t* coordinator);

GS_API_DECL uint32_t 
gs_core_ai_coordinator_get_tool_count(const gs_core_ai_coordinator_t* coordinator);

GS_API_DECL gs_core_ai_tool_t* 
gs_core_ai_coordinator_find_tool(
    gs_core_ai_coordinator_t* coordinator,
    const char* tool_name
);

/*================================================================================
// Default Tools Registration
================================================================================*/

/**
 * Register all default tools with the coordinator.
 * 
 * This function registers tools from the following categories based on the config:
 * - File system tools (read_file, list_directory, file_exists, search_files)
 * - HTTP tools (http_get)
 * - Code search tools (search_code, list_functions, find_definition)
 * 
 * Use gs_core_ai_default_tools_config_t to selectively enable/disable tool categories.
 * 
 * @param coordinator The coordinator to register tools with
 * @param config Configuration specifying which tool categories to enable (NULL = all enabled)
 * @return Number of tools successfully registered
 * 
 * Example:
 * @code
 *   // Register all default tools
 *   gs_core_ai_register_default_tools(coordinator, NULL);
 * 
 *   // Register only file system tools
 *   gs_core_ai_default_tools_config_t config = {
 *       .enable_fs_tools = true,
 *       .enable_http_tools = false,
 *       .enable_code_tools = false
 *   };
 *   gs_core_ai_register_default_tools(coordinator, &config);
 * @endcode
 */
GS_API_DECL uint32_t 
gs_core_ai_register_default_tools(
    gs_core_ai_coordinator_t* coordinator,
    const gs_core_ai_default_tools_config_t* config
);

/*================================================================================
// Result Helper Functions
================================================================================*/

GS_API_DECL void 
gs_core_ai_coordinator_result_free(gs_core_ai_coordinator_result_t* result);

#endif // GS_CORE_AI_COORDINATOR_H
