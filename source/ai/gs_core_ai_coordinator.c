/*==============================================================================================================
    * Copyright: 2025 John Jackson 
    * File: gs_core_ai_coordinator.c

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

#include "gs_core_ai_coordinator.h"
#include "tools/gs_core_ai_tools_fs.h"
#include "tools/gs_core_ai_tools_http.h"
#include "tools/gs_core_ai_tools_code.h"
#include <cJSON/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*================================================================================
// Internal Helper Functions
================================================================================*/

// Generate base system prompt with tool descriptions (shared across all modes)
static char* _generate_base_system_prompt(gs_core_ai_coordinator_t* coordinator)
{
    size_t total_size = 1024;
    for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->tools); i++) {
        gs_core_ai_tool_t* tool = &coordinator->tools[i];
        total_size += strlen(tool->name) + strlen(tool->description) + strlen(tool->parameters_schema) + 256;
    }
    
    char* prompt = gs_malloc(total_size);
    if (!prompt) return NULL;
    
    strcpy(prompt, "You are a helpful AI assistant with access to tools.\n\n");
    strcat(prompt, "CRITICAL RULES:\n");
    strcat(prompt, "1. You can ONLY use tools from the list below - no other tools exist\n");
    strcat(prompt, "2. You MUST use the EXACT tool names listed - do not invent or guess tool names\n");
    strcat(prompt, "3. If a tool you want doesn't exist in the list, use the closest available alternative\n");
    strcat(prompt, "4. DO NOT guess or make assumptions - always gather information using tools first\n");
    strcat(prompt, "5. For questions about code or files, you MUST use the available tools\n");
    strcat(prompt, "6. When searching for code symbols (functions, variables), use case-insensitive patterns\n");
    strcat(prompt, "   Example: For 'MyFunction' search with pattern '[Mm][Yy][Ff]unction' or use read_file first\n");
    strcat(prompt, "   IMPORTANT: Use find_definition tool for functions (case-insensitive by default),\n");
    strcat(prompt, "               or search_code with case_insensitive:true for custom patterns\n\n");
    
    // Count and display available tools
    uint32_t tool_count = gs_dyn_array_size(coordinator->tools);
    char count_buf[64];
    snprintf(count_buf, sizeof(count_buf), "Available tools (%u total):\n\n", tool_count);
    strcat(prompt, count_buf);
    
    for (uint32_t i = 0; i < tool_count; i++) {
        gs_core_ai_tool_t* tool = &coordinator->tools[i];
        char tool_desc[4096];
        snprintf(tool_desc, sizeof(tool_desc), 
            "Tool #%u: %s\nDescription: %s\nParameters Schema:\n%s\n\n",
            i + 1, tool->name, tool->description, tool->parameters_schema);
        strcat(prompt, tool_desc);
    }
    
    strcat(prompt, "REMINDER: These are the ONLY tools available. Use exact tool names from this list.\n\n");
    
    return prompt;
}

// Generate agentic mode prompt (ReAct pattern)
static char* _generate_agentic_mode_prompt(gs_core_ai_coordinator_t* coordinator)
{
    char* base = _generate_base_system_prompt(coordinator);
    if (!base) return NULL;
    
    size_t total_size = strlen(base) + 2048;
    char* prompt = gs_malloc(total_size);
    if (!prompt) {
        gs_free(base);
        return NULL;
    }
    
    strcpy(prompt, base);
    gs_free(base);
    
    strcat(prompt, "\nYou use the ReAct (Reasoning and Acting) framework:\n\n");
    strcat(prompt, "Format each response as:\n");
    strcat(prompt, "Thought: <your reasoning about what to do next>\n");
    strcat(prompt, "Action: <tool call JSON OR final answer>\n\n");
    strcat(prompt, "Tool call format:\n");
    strcat(prompt, "{\"tool\": \"tool_name\", \"arguments\": {...}}\n\n");
    strcat(prompt, "Example:\n");
    strcat(prompt, "Thought: I need to read the file to answer the question.\n");
    strcat(prompt, "Action: {\"tool\": \"read_file\", \"arguments\": {\"path\": \"example.txt\"}}\n\n");
    strcat(prompt, "After seeing tool results (Observation), continue with another Thought and Action.\n");
    strcat(prompt, "When you have enough information, provide your final answer as plain text.\n");
    
    return prompt;
}

// Generate plan-reflect mode prompt
static char* _generate_plan_reflect_mode_prompt(gs_core_ai_coordinator_t* coordinator, bool is_planning_phase)
{
    char* base = _generate_base_system_prompt(coordinator);
    if (!base) return NULL;
    
    size_t total_size = strlen(base) + 2048;
    char* prompt = gs_malloc(total_size);
    if (!prompt) {
        gs_free(base);
        return NULL;
    }
    
    strcpy(prompt, base);
    gs_free(base);
    
    if (is_planning_phase) {
        strcat(prompt, "\nPHASE 1: PLANNING\n");
        strcat(prompt, "Create a step-by-step plan in JSON format:\n");
        strcat(prompt, "{\n");
        strcat(prompt, "  \"plan\": {\n");
        strcat(prompt, "    \"goal\": \"What you're trying to accomplish\",\n");
        strcat(prompt, "    \"steps\": [\n");
        strcat(prompt, "      {\"step\": 1, \"action\": \"What to do\", \"tool\": \"tool_name\", \"reason\": \"Why\"},\n");
        strcat(prompt, "      {\"step\": 2, \"action\": \"Next action\", \"tool\": \"tool_name\", \"reason\": \"Why\"}\n");
        strcat(prompt, "    ]\n");
        strcat(prompt, "  }\n");
        strcat(prompt, "}\n");
    } else {
        strcat(prompt, "\nPHASE 2: EXECUTION WITH REFLECTION\n");
        strcat(prompt, "Execute each step. After seeing results, you can:\n");
        strcat(prompt, "1. Continue to next step\n");
        strcat(prompt, "2. Revise remaining steps based on what you learned\n\n");
        strcat(prompt, "To revise, respond with:\n");
        strcat(prompt, "{\"revision\": {\"revised_steps\": [{\"step\": N, \"action\": \"...\", \"tool\": \"...\", \"reason\": \"...\"}]}}\n\n");
        strcat(prompt, "Tool call format:\n");
        strcat(prompt, "{\"tool\": \"tool_name\", \"arguments\": {...}}\n");
    }
    
    return prompt;
}

// Top-level system prompt generator (dispatches to mode-specific functions)
static char* _generate_system_prompt(gs_core_ai_coordinator_t* coordinator, bool is_planning_phase)
{
    switch (coordinator->config.execution_mode) {
        case GS_CORE_AI_MODE_AGENTIC:
            return _generate_agentic_mode_prompt(coordinator);
        
        case GS_CORE_AI_MODE_PLAN_REFLECT:
        default:
            return _generate_plan_reflect_mode_prompt(coordinator, is_planning_phase);
    }
}

// Check if response contains a plan
static bool _is_plan(const char* response)
{
    if (!response) return false;
    const char* json_start = strchr(response, '{');
    if (!json_start) return false;
    return strstr(json_start, "\"plan\"") != NULL;
}

// Extract steps array from plan JSON and store as cJSON objects in coordinator
static bool _extract_steps_from_plan(gs_core_ai_coordinator_t* coordinator, cJSON* plan_json)
{
    gs_println("[DEBUG] _extract_steps_from_plan: Starting...");
    
    if (!plan_json || !coordinator) {
        gs_println("[DEBUG] _extract_steps_from_plan: NULL parameter");
        return false;
    }
    
    // Find the 'plan' object
    cJSON* plan_obj = cJSON_GetObjectItem(plan_json, "plan");
    if (!plan_obj) {
        gs_println("[DEBUG] _extract_steps_from_plan: No 'plan' object in JSON");
        return false;
    }
    
    // Find the 'steps' array
    cJSON* steps_array = cJSON_GetObjectItem(plan_obj, "steps");
    if (!steps_array || !cJSON_IsArray(steps_array)) {
        gs_println("[DEBUG] _extract_steps_from_plan: No 'steps' array in plan");
        return false;
    }
    
    int step_count = cJSON_GetArraySize(steps_array);
    gs_println("[DEBUG] _extract_steps_from_plan: Found %d steps in plan", step_count);
    
    // Initialize the array if needed
    if (!coordinator->current_plan_steps) {
        coordinator->current_plan_steps = NULL; // gs_dyn_array will auto-initialize
    }
    
    // Store each step as a separate cJSON object (duplicate to own)
    for (int i = 0; i < step_count; i++) {
        cJSON* step = cJSON_GetArrayItem(steps_array, i);
        if (step) {
            cJSON* step_copy = cJSON_Duplicate(step, 1); // Deep copy
            gs_dyn_array_push(coordinator->current_plan_steps, (void*)step_copy);
            gs_println("[DEBUG] _extract_steps_from_plan: Copied step %d", i + 1);
        }
    }
    
    gs_println("[DEBUG] _extract_steps_from_plan: Returning %s (%u steps stored)", 
        gs_dyn_array_size(coordinator->current_plan_steps) > 0 ? "TRUE" : "FALSE",
        gs_dyn_array_size(coordinator->current_plan_steps));
    
    return gs_dyn_array_size(coordinator->current_plan_steps) > 0;
}

// Extract and store plan from response
static bool _extract_plan(const char* response, char** out_plan)
{
    gs_println("[DEBUG] _extract_plan: Starting extraction...");
    gs_println("[DEBUG] _extract_plan: Response first 200 chars: '%.200s'", response);
    
    const char* json_start = strchr(response, '{');
    if (!json_start) {
        gs_println("[DEBUG] _extract_plan: No opening brace found");
        return false;
    }
    
    gs_println("[DEBUG] _extract_plan: Found opening brace at offset %zu", json_start - response);
    
    // Find the plan object
    const char* plan_start = strstr(json_start, "\"plan\"");
    if (!plan_start) {
        gs_println("[DEBUG] _extract_plan: No 'plan' field found");
        return false;
    }
    
    gs_println("[DEBUG] _extract_plan: Found 'plan' field");
    
    // Find the opening brace of the plan object
    const char* plan_obj_start = strchr(plan_start, '{');
    if (!plan_obj_start) {
        gs_println("[DEBUG] _extract_plan: No opening brace after 'plan' field");
        return false;
    }
    
    gs_println("[DEBUG] _extract_plan: Starting brace counting...");
    
    // Find matching closing brace for the entire JSON
    int brace_count = 1;
    const char* ptr = json_start + 1;
    int max_chars = 50000; // Safety limit
    int chars_processed = 0;
    
    while (*ptr && brace_count > 0 && chars_processed < max_chars) {
        if (*ptr == '{') brace_count++;
        else if (*ptr == '}') brace_count--;
        ptr++;
        chars_processed++;
    }
    
    gs_println("[DEBUG] _extract_plan: Brace counting done. Final count: %d, chars processed: %d", brace_count, chars_processed);
    
    if (brace_count != 0) {
        gs_println("[DEBUG] _extract_plan: Unbalanced braces (count=%d)", brace_count);
        gs_println("[DEBUG] _extract_plan: Response length: %zu", strlen(response));
        gs_println("[DEBUG] _extract_plan: JSON start offset: %zu", json_start - response);
        if (strlen(response) > 100) {
            gs_println("[DEBUG] _extract_plan: Last 100 chars: '%.100s'", response + strlen(response) - 100);
        }
        return false;
    }
    
    // Extract the entire plan JSON (including outer braces)
    size_t plan_len = ptr - json_start;
    gs_println("[DEBUG] _extract_plan: Allocating %zu bytes for plan", plan_len + 1);
    
    *out_plan = gs_malloc(plan_len + 1);
    if (!*out_plan) {
        gs_println("[DEBUG] _extract_plan: Failed to allocate memory");
        return false;
    }
    
    memcpy(*out_plan, json_start, plan_len);
    (*out_plan)[plan_len] = '\0';
    
    gs_println("[DEBUG] _extract_plan: Successfully extracted plan (%zu chars)", plan_len);
    gs_println("[DEBUG] _extract_plan: Extracted JSON: '%.500s'", *out_plan);
    gs_println("[DEBUG] _extract_plan: Last 100 chars: '%.100s'", plan_len > 100 ? (*out_plan) + plan_len - 100 : *out_plan);
    
    return true;
}

// Check if response is a tool call (contains '{' and "tool" field)
static bool _is_tool_call(const char* response)
{
    if (!response) return false;
    // Look for JSON object with "tool" field anywhere in the response
    // Skip past any "plan" JSON objects to find a standalone tool call
    const char* search_pos = response;
    while (true) {
        const char* json_start = strchr(search_pos, '{');
        if (!json_start) return false;
        
        // Find the end of this JSON object (simple bracket matching)
        int bracket_count = 1;
        const char* pos = json_start + 1;
        const char* json_end = NULL;
        while (*pos && bracket_count > 0) {
            if (*pos == '{') bracket_count++;
            else if (*pos == '}') {
                bracket_count--;
                if (bracket_count == 0) json_end = pos;
            }
            pos++;
        }
        
        if (!json_end) return false;
        
        // Check if this JSON object contains "tool" field
        size_t json_len = json_end - json_start + 1;
        char* json_str = gs_malloc(json_len + 1);
        memcpy(json_str, json_start, json_len);
        json_str[json_len] = '\0';
        
        bool has_tool = strstr(json_str, "\"tool\"") != NULL;
        bool has_plan = strstr(json_str, "\"plan\"") != NULL;
        gs_free(json_str);
        
        // Found a tool call (has "tool" but not "plan")
        if (has_tool && !has_plan) return true;
        
        // Continue searching after this JSON object
        search_pos = json_end + 1;
    }
    
    return false;
}

// Parse tool call JSON and extract tool name and arguments
// Returns true on success, false on parse error
static bool _parse_tool_call(const char* json, char** tool_name, char** arguments_json)
{
    // Find the JSON object containing "tool" but not "plan"
    const char* search_pos = json;
    const char* tool_json_start = NULL;
    
    while (true) {
        const char* json_start = strchr(search_pos, '{');
        if (!json_start) return false;
        
        // Find the end of this JSON object
        int bracket_count = 1;
        const char* pos = json_start + 1;
        const char* json_end = NULL;
        while (*pos && bracket_count > 0) {
            if (*pos == '{') bracket_count++;
            else if (*pos == '}') {
                bracket_count--;
                if (bracket_count == 0) json_end = pos;
            }
            pos++;
        }
        
        if (!json_end) return false;
        
        // Check if this JSON object contains "tool" field without "plan"
        size_t json_len = json_end - json_start + 1;
        char* json_str = gs_malloc(json_len + 1);
        memcpy(json_str, json_start, json_len);
        json_str[json_len] = '\0';
        
        bool has_tool = strstr(json_str, "\"tool\"") != NULL;
        bool has_plan = strstr(json_str, "\"plan\"") != NULL;
        gs_free(json_str);
        
        // Found the tool call JSON
        if (has_tool && !has_plan) {
            tool_json_start = json_start;
            break;
        }
        
        // Continue searching after this JSON object
        search_pos = json_end + 1;
    }
    
    if (!tool_json_start) return false;
    
    const char* tool_start = strstr(tool_json_start, "\"tool\"");
    if (!tool_start) return false;
    
    const char* name_start = strchr(tool_start + 6, '\"');
    if (!name_start) return false;
    name_start++;
    
    const char* name_end = strchr(name_start, '\"');
    if (!name_end) return false;
    
    size_t name_len = name_end - name_start;
    *tool_name = gs_malloc(name_len + 1);
    if (!*tool_name) return false;
    memcpy(*tool_name, name_start, name_len);
    (*tool_name)[name_len] = '\0';
    
    // Extract arguments JSON object
    const char* args_start = strstr(json, "\"arguments\"");
    if (!args_start) {
        // No arguments provided, use empty object
        *arguments_json = gs_malloc(3);
        strcpy(*arguments_json, "{}");
        return true;
    }
    
    const char* args_obj_start = strchr(args_start, '{');
    if (!args_obj_start) {
        *arguments_json = gs_malloc(3);
        strcpy(*arguments_json, "{}");
        return true;
    }
    
    // Find matching closing brace
    int brace_count = 1;
    const char* ptr = args_obj_start + 1;
    while (*ptr && brace_count > 0) {
        if (*ptr == '{') brace_count++;
        else if (*ptr == '}') brace_count--;
        ptr++;
    }
    
    if (brace_count != 0) {
        gs_free(*tool_name);
        return false;
    }
    
    size_t args_len = ptr - args_obj_start;
    *arguments_json = gs_malloc(args_len + 1);
    if (!*arguments_json) {
        gs_free(*tool_name);
        return false;
    }
    memcpy(*arguments_json, args_obj_start, args_len);
    (*arguments_json)[args_len] = '\0';
    
    return true;
}

/*================================================================================
// Reflection Helper Functions (for PLAN_REFLECT mode)
================================================================================*/

// Check if response contains a reflection
static bool _is_reflection(const char* response)
{
    if (!response) return false;
    const char* json_start = strchr(response, '{');
    if (!json_start) return false;
    return strstr(json_start, "\"reflection\"") != NULL || strstr(json_start, "\"revision\"") != NULL;
}

// Extract reflection from response
static bool _extract_reflection(const char* response, char** out_reflection)
{
    if (!response || !out_reflection) return false;
    
    const char* json_start = strchr(response, '{');
    if (!json_start) return false;
    
    // Find matching closing brace
    int brace_count = 1;
    const char* ptr = json_start + 1;
    while (*ptr && brace_count > 0) {
        if (*ptr == '{') brace_count++;
        else if (*ptr == '}') brace_count--;
        ptr++;
    }
    
    if (brace_count != 0) return false;
    
    size_t json_len = ptr - json_start;
    *out_reflection = gs_malloc(json_len + 1);
    if (!*out_reflection) return false;
    
    memcpy(*out_reflection, json_start, json_len);
    (*out_reflection)[json_len] = '\0';
    
    return true;
}

// Check if reflection contains a plan revision
static bool _is_plan_revision(const char* reflection_json)
{
    if (!reflection_json) return false;
    return strstr(reflection_json, "\"revision\"") != NULL || 
           strstr(reflection_json, "\"revised_steps\"") != NULL;
}

// Update plan from reflection (revise remaining steps)
static bool _update_plan_from_reflection(
    gs_core_ai_coordinator_t* coordinator,
    const char* reflection_json)
{
    if (!coordinator || !reflection_json) return false;
    
    cJSON* reflection = cJSON_Parse(reflection_json);
    if (!reflection) return false;
    
    // Look for revision object
    cJSON* revision = cJSON_GetObjectItem(reflection, "revision");
    if (!revision) {
        cJSON_Delete(reflection);
        return false;
    }
    
    // Get revised steps array
    cJSON* revised_steps = cJSON_GetObjectItem(revision, "revised_steps");
    if (!revised_steps || !cJSON_IsArray(revised_steps)) {
        cJSON_Delete(reflection);
        return false;
    }
    
    // Clear remaining steps from current_plan_steps
    uint32_t current_idx = coordinator->current_step_index;
    if (coordinator->current_plan_steps && current_idx < gs_dyn_array_size(coordinator->current_plan_steps)) {
        // Free steps from current_step_index onward
        for (uint32_t i = current_idx; i < gs_dyn_array_size(coordinator->current_plan_steps); i++) {
            cJSON_Delete((cJSON*)coordinator->current_plan_steps[i]);
        }
        // Manually resize array to remove freed steps
        gs_dyn_array_head(coordinator->current_plan_steps)->size = current_idx;
    }
    
    // Add revised steps
    int step_count = cJSON_GetArraySize(revised_steps);
    for (int i = 0; i < step_count; i++) {
        cJSON* step = cJSON_GetArrayItem(revised_steps, i);
        if (step) {
            cJSON* step_copy = cJSON_Duplicate(step, 1);
            gs_dyn_array_push(coordinator->current_plan_steps, (void*)step_copy);
        }
    }
    
    coordinator->plan_revised = true;
    cJSON_Delete(reflection);
    return true;
}

/*================================================================================
// Thought Helper Functions (for AGENTIC mode)
================================================================================*/

// Check if response follows thought-action format
static bool _is_thought_action_response(const char* response)
{
    if (!response) return false;
    // Look for "Thought:" marker (case-insensitive search would be better but keeping simple)
    return strstr(response, "Thought:") != NULL || strstr(response, "thought:") != NULL;
}

// Extract thought from response
static bool _extract_thought(const char* response, char** out_thought)
{
    if (!response || !out_thought) return false;
    
    // Find "Thought:" marker
    const char* thought_start = strstr(response, "Thought:");
    if (!thought_start) {
        thought_start = strstr(response, "thought:");
    }
    if (!thought_start) return false;
    
    // Skip past "Thought:"
    thought_start += 8;
    while (*thought_start == ' ' || *thought_start == '\t') thought_start++;
    
    // Find end of thought (look for "Action:" or newline followed by "Action:")
    const char* thought_end = strstr(thought_start, "Action:");
    if (!thought_end) {
        thought_end = strstr(thought_start, "action:");
    }
    if (!thought_end) {
        // No explicit action marker, use end of line or string
        thought_end = thought_start;
        while (*thought_end && *thought_end != '\n') thought_end++;
    }
    
    // Trim trailing whitespace
    while (thought_end > thought_start && (*(thought_end-1) == ' ' || *(thought_end-1) == '\t' || *(thought_end-1) == '\n')) {
        thought_end--;
    }
    
    size_t thought_len = thought_end - thought_start;
    if (thought_len == 0) return false;
    
    *out_thought = gs_malloc(thought_len + 1);
    if (!*out_thought) return false;
    
    memcpy(*out_thought, thought_start, thought_len);
    (*out_thought)[thought_len] = '\0';
    
    return true;
}

/*================================================================================
// Query Execution Helper Functions
================================================================================*/

// Helper to add message to conversation history
static void _add_to_history(gs_core_ai_coordinator_t* coordinator, const char* message)
{
    if (!coordinator || !message) return;
    char* msg = gs_malloc(strlen(message) + 1);
    strcpy(msg, message);
    gs_dyn_array_push(coordinator->conversation_history, msg);
}

// Helper to build success result
static gs_core_ai_coordinator_result_t _build_success_result(
    const char* response,
    uint32_t iterations,
    uint32_t total_tokens)
{
    gs_core_ai_coordinator_result_t result = {0};
    result.success = true;
    result.response = gs_malloc(strlen(response) + 1);
    strcpy(result.response, response);
    result.iterations = iterations;
    result.total_tokens = total_tokens;
    return result;
}

// Helper to build error result
static gs_core_ai_coordinator_result_t _build_error_result(
    const char* error_message,
    uint32_t iterations,
    uint32_t total_tokens)
{
    gs_core_ai_coordinator_result_t result = {0};
    result.success = false;
    result.error_message = gs_malloc(strlen(error_message) + 1);
    strcpy(result.error_message, error_message);
    result.iterations = iterations;
    result.total_tokens = total_tokens;
    return result;
}

// Helper to prepare provider config
static gs_core_ai_provider_config_t _prepare_provider_config(
    gs_core_ai_coordinator_t* coordinator,
    const gs_core_ai_provider_config_t* provider_config,
    bool is_planning_phase)
{
    gs_core_ai_provider_config_t config;
    if (provider_config) {
        config = *provider_config;
    } else {
        config = GS_CORE_AI_PROVIDER_CONFIG_DEFAULT();
    }
    
    // Apply coordinator's num_ctx if not overridden in provider_config
    if (coordinator->config.num_ctx > 0 && (!provider_config || provider_config->num_ctx == 0)) {
        config.num_ctx = coordinator->config.num_ctx;
    }
    
    if (coordinator->config.system_prompt) {
        config.system_prompt = coordinator->config.system_prompt;
    } else {
        if (!coordinator->generated_system_prompt) {
            coordinator->generated_system_prompt = _generate_system_prompt(coordinator, is_planning_phase);
        }
        config.system_prompt = coordinator->generated_system_prompt;
    }
    
    return config;
}

// Helper to build initial execution prompt
static char* _build_initial_prompt(
    gs_core_ai_coordinator_t* coordinator,
    const char* query)
{
    uint32_t step_count = coordinator->current_plan_steps ? gs_dyn_array_size(coordinator->current_plan_steps) : 0;
    gs_println("[DEBUG CHECKPOINT 5] In _build_initial_prompt: current_plan_steps=%p, size=%u, current_step_index=%u",
        (void*)coordinator->current_plan_steps, step_count, coordinator->current_step_index);
    
    // Check if we have parsed plan steps
    if (coordinator->current_plan_steps && coordinator->current_step_index < gs_dyn_array_size(coordinator->current_plan_steps)) {
        // Plan-driven execution: explicitly tell model to execute this specific step
        cJSON* current_step = (cJSON*)coordinator->current_plan_steps[coordinator->current_step_index];
        
        // Extract the tool name and arguments from the step
        cJSON* tool_obj = cJSON_GetObjectItem(current_step, "tool");
        cJSON* args_obj = cJSON_GetObjectItem(current_step, "arguments");
        cJSON* action_obj = cJSON_GetObjectItem(current_step, "action");
        
        const char* tool_name = tool_obj && cJSON_IsString(tool_obj) ? tool_obj->valuestring : "unknown";
        const char* action = action_obj && cJSON_IsString(action_obj) ? action_obj->valuestring : "execute step";
        char* args_json = args_obj ? cJSON_PrintUnformatted(args_obj) : gs_strdup("{}");
        
        size_t prompt_size = strlen(query) + strlen(coordinator->current_plan) + strlen(action) + strlen(args_json) + 1024;
        char* prompt = gs_malloc(prompt_size);
        snprintf(prompt, prompt_size,
            "You created this plan:\n%s\n\n"
            "You are now on step %u of %u: %s\n\n"
            "EXECUTE THIS STEP NOW.\n\n"
            "RESPOND WITH ONLY THIS JSON (no other text):\n"
            "{\"tool\": \"%s\", \"arguments\": %s}\n\n"
            "Original query: %s",
            coordinator->current_plan, 
            coordinator->current_step_index + 1, 
            (uint32_t)gs_dyn_array_size(coordinator->current_plan_steps),
            action,
            tool_name,
            args_json,
            query);
        
        if (args_obj) cJSON_free(args_json);
        else gs_free(args_json);
        return prompt;
    }
    else if (coordinator->current_plan) {
        // Legacy: plan exists but no steps parsed (fallback)
        size_t prompt_size = strlen(query) + strlen(coordinator->current_plan) + 512;
        char* prompt = gs_malloc(prompt_size);
        snprintf(prompt, prompt_size,
            "You created this plan:\n%s\n\nNow execute step 1 EXACTLY as described. Use the EXACT tool name and arguments specified in step 1. Call the tool using JSON format:\n{\"tool\": \"tool_name\", \"arguments\": {...}}",
            coordinator->current_plan);
        return prompt;
    } else {
        // No planning: conversational mode
        char* prompt = gs_malloc(strlen(query) + 1);
        strcpy(prompt, query);
        return prompt;
    }
}

// Mode-specific tool result prompt builders
static char* _build_agentic_tool_result_prompt(
    gs_core_ai_coordinator_t* coordinator,
    const char* tool_name,
    const gs_core_ai_tool_result_t* tool_result)
{
    char* prompt = NULL;
    
    if (tool_result->code == GS_CORE_AI_TOOL_SUCCESS) {
        size_t prompt_size = strlen(tool_result->data) + 256;
        prompt = gs_malloc(prompt_size);
        snprintf(prompt, prompt_size,
            "Observation: Tool '%s' returned: %s\n\n"
            "What is your next thought and action?",
            tool_name, tool_result->data);
    } else {
        size_t prompt_size = strlen(tool_result->error_message) + 256;
        prompt = gs_malloc(prompt_size);
        snprintf(prompt, prompt_size,
            "Observation: Tool '%s' failed: %s\n\n"
            "What is your next thought and action?",
            tool_name, tool_result->error_message);
    }
    
    return prompt;
}

static char* _build_plan_reflect_tool_result_prompt(
    gs_core_ai_coordinator_t* coordinator,
    const char* tool_name,
    const gs_core_ai_tool_result_t* tool_result,
    const char* original_query)
{
    char* prompt = NULL;
    
    if (tool_result->code == GS_CORE_AI_TOOL_SUCCESS) {
        size_t prompt_size = strlen(tool_result->data) + strlen(original_query) + 2048;
        if (coordinator->current_plan) {
            prompt_size += strlen(coordinator->current_plan);
        }
        prompt = gs_malloc(prompt_size);
        
        // Advance to next step after successful tool execution
        coordinator->current_step_index++;
        
        uint32_t total_steps = (uint32_t)gs_dyn_array_size(coordinator->current_plan_steps);
        
        if (coordinator->config.enable_debug_logging) {
            gs_println("[DEBUG] After step complete: current_step_index=%u, total_steps=%u", 
                coordinator->current_step_index, total_steps);
        }
        
        if (coordinator->current_step_index >= total_steps) {
            // All steps complete
            snprintf(prompt, prompt_size,
                "Tool '%s' returned: %s\n\n"
                "All plan steps completed! Provide a concise summary of what was accomplished.\n\n"
                "Original request: \"%s\"",
                tool_name, tool_result->data, original_query);
        } else {
            // Execute next step - extract the arguments from the step
            cJSON* next_step = (cJSON*)coordinator->current_plan_steps[coordinator->current_step_index];
            
            // Extract components from the step
            cJSON* tool_obj = cJSON_GetObjectItem(next_step, "tool");
            cJSON* args_obj = cJSON_GetObjectItem(next_step, "arguments");
            
            const char* next_tool = tool_obj && cJSON_IsString(tool_obj) ? tool_obj->valuestring : "unknown";
            char* args_json = args_obj ? cJSON_PrintUnformatted(args_obj) : gs_strdup("{}");
            
            snprintf(prompt, prompt_size,
                "Tool '%s' returned: %s\n\n"
                "Step %u of %u complete.\n\n"
                "NOW EXECUTE THE NEXT STEP.\n\n"
                "RESPOND WITH ONLY THIS JSON (no other text):\n"
                "{\"tool\": \"%s\", \"arguments\": %s}",
                tool_name, tool_result->data,
                coordinator->current_step_index, total_steps,
                next_tool, args_json);
            
            if (args_obj) cJSON_free(args_json);
            else gs_free(args_json);
        }
    } else {
        // Tool error - check if we have a next step or need to handle the error
        uint32_t total_steps = (uint32_t)gs_dyn_array_size(coordinator->current_plan_steps);
        
        if (coordinator->current_step_index < total_steps) {
            // Try to continue with next step despite error
            cJSON* next_step = (cJSON*)coordinator->current_plan_steps[coordinator->current_step_index];
            cJSON* tool_obj = cJSON_GetObjectItem(next_step, "tool");
            cJSON* args_obj = cJSON_GetObjectItem(next_step, "arguments");
            
            const char* next_tool = tool_obj && cJSON_IsString(tool_obj) ? tool_obj->valuestring : "unknown";
            char* args_json = args_obj ? cJSON_PrintUnformatted(args_obj) : gs_strdup("{}");
            
            size_t prompt_size = strlen(tool_result->error_message) + strlen(next_tool) + strlen(args_json) + 1024;
            prompt = gs_malloc(prompt_size);
            snprintf(prompt, prompt_size,
                "Tool '%s' failed: %s\n\n"
                "Continuing with next step anyway.\n\n"
                "RESPOND WITH ONLY THIS JSON (no other text):\n"
                "{\"tool\": \"%s\", \"arguments\": %s}",
                tool_name, tool_result->error_message,
                next_tool, args_json);
            
            if (args_obj) cJSON_free(args_json);
            else gs_free(args_json);
        } else {
            // No more steps, just report the error
            size_t prompt_size = strlen(tool_result->error_message) + 512;
            prompt = gs_malloc(prompt_size);
            snprintf(prompt, prompt_size,
                "Tool '%s' failed: %s\n\n"
                "Unable to complete the task due to this error.",
                tool_name, tool_result->error_message);
        }
    }
    
    return prompt;
}

// Helper to build tool result prompt (dispatches to mode-specific builders)
static char* _build_tool_result_prompt(
    gs_core_ai_coordinator_t* coordinator,
    const char* tool_name,
    const gs_core_ai_tool_result_t* tool_result,
    const char* original_query)
{
    switch (coordinator->config.execution_mode) {
        case GS_CORE_AI_MODE_AGENTIC:
            return _build_agentic_tool_result_prompt(coordinator, tool_name, tool_result);
        
        case GS_CORE_AI_MODE_PLAN_REFLECT:
        default:
            return _build_plan_reflect_tool_result_prompt(coordinator, tool_name, tool_result, original_query);
    }
    
    // Should never reach here
    return NULL;
}

// Execute a single tool call
static gs_core_ai_coordinator_result_t _execute_tool_call(
    gs_core_ai_coordinator_t* coordinator,
    const char* response_content,
    const char* original_query,
    uint32_t* iterations,
    uint32_t* total_tokens)
{
    gs_core_ai_coordinator_result_t result = {0};
    
    // Parse tool call
    char* tool_name = NULL;
    char* arguments_json = NULL;
    if (!_parse_tool_call(response_content, &tool_name, &arguments_json)) {
        return _build_error_result("Failed to parse tool call JSON", *iterations, *total_tokens);
    }
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("Tool call: %s", tool_name);
        gs_println("Arguments: %s", arguments_json);
    }
    
    // Find tool
    gs_core_ai_tool_t* tool = gs_core_ai_coordinator_find_tool(coordinator, tool_name);
    if (!tool) {
        char error[256];
        snprintf(error, sizeof(error), "Tool not found: %s", tool_name);
        gs_free(tool_name);
        gs_free(arguments_json);
        return _build_error_result(error, *iterations, *total_tokens);
    }
    
    // Execute tool
    gs_core_ai_tool_result_t tool_result = tool->execute(tool, arguments_json);
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("Tool result: %s", 
            tool_result.code == GS_CORE_AI_TOOL_SUCCESS ? tool_result.data : tool_result.error_message);
    }
    
    // Build next prompt with tool result
    char* next_prompt = _build_tool_result_prompt(coordinator, tool_name, &tool_result, original_query);
    _add_to_history(coordinator, next_prompt);
    
    // Store next prompt in result for continuation
    result.response = next_prompt;  // Reusing response field to pass prompt back
    result.success = true;  // Indicates tool executed successfully, need to continue
    
    // Cleanup
    gs_core_ai_tool_result_free(&tool_result);
    gs_free(tool_name);
    gs_free(arguments_json);
    
    return result;
}

// Execute planning phase
static gs_core_ai_coordinator_result_t _execute_planning_phase(
    gs_core_ai_coordinator_t* coordinator,
    const char* query,
    gs_core_ai_provider_config_t* config)
{
    gs_core_ai_coordinator_result_t result = {0};
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("=== PLANNING PHASE ===");
    }
    
    // Create planning prompt
    size_t planning_prompt_size = strlen(query) + 1024;
    char* planning_prompt = gs_malloc(planning_prompt_size);
    snprintf(planning_prompt, planning_prompt_size,
        "%s\n\n"
        "You MUST respond with ONLY a JSON plan in this exact format (no other text):\n"
        "{\"plan\": {\"goal\": \"description\", \"steps\": [\n"
        "  {\"step\": 1, \"action\": \"what to do\", \"tool\": \"tool_name\", \"arguments\": {\"param\": \"value\"}, \"reason\": \"why\"}\n"
        "]}}\n\n"
        "CRITICAL: Every step MUST include an 'arguments' object with the actual parameter values needed for that tool.\n"
        "If a tool takes no parameters, use empty object: \"arguments\": {}", 
        query);
    
    // Use lower temperature for planning
    gs_core_ai_provider_config_t planning_config = *config;
    planning_config.temperature = coordinator->config.planning_temperature;
    
    gs_core_ai_provider_response_t planning_response = coordinator->provider->request(
        coordinator->provider,
        planning_prompt,
        &planning_config
    );
    
    result.iterations = 1;
    if (planning_response.metadata.total_tokens > 0) {
        result.total_tokens = planning_response.metadata.total_tokens;
    }
    
    if (!gs_core_ai_provider_response_is_success(&planning_response)) {
        char error[512];
        snprintf(error, sizeof(error), "Planning failed: %s", planning_response.error_message);
        gs_core_ai_provider_response_free(&planning_response);
        gs_free(planning_prompt);
        return _build_error_result(error, result.iterations, result.total_tokens);
    }
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("Planning response: %s", planning_response.content);
        gs_println("[DEBUG] Planning response length: %zu", strlen(planning_response.content));
        gs_println("[DEBUG] First 50 chars: '%.50s'", planning_response.content);
        gs_println("[DEBUG] Calling _is_plan...");
    }
    
    // Extract and store plan
    if (!_is_plan(planning_response.content)) {
        if (coordinator->config.enable_debug_logging) {
            gs_println("[DEBUG] _is_plan returned FALSE");
        }
        gs_core_ai_provider_response_free(&planning_response);
        gs_free(planning_prompt);
        return _build_error_result("Model failed to create a valid plan. Response was not in expected JSON format.", 
            result.iterations, result.total_tokens);
    }
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("[DEBUG] _is_plan returned TRUE, calling _extract_plan...");
    }
    
    if (!_extract_plan(planning_response.content, &coordinator->current_plan)) {
        gs_core_ai_provider_response_free(&planning_response);
        gs_free(planning_prompt);
        return _build_error_result("Failed to extract plan from response", result.iterations, result.total_tokens);
    }
    
    if (coordinator->config.enable_debug_logging) {
        gs_println("Extracted plan: %s", coordinator->current_plan);
    }
    
    // Parse plan JSON to extract steps for step-by-step execution
    cJSON* plan_json = cJSON_Parse(coordinator->current_plan);
    if (plan_json) {
        coordinator->current_plan_steps = NULL; // Initialize array
        gs_println("[DEBUG CHECKPOINT 1] Before _extract_steps_from_plan: current_plan_steps=%p", 
            (void*)coordinator->current_plan_steps);
        
        if (_extract_steps_from_plan(coordinator, plan_json)) {
            coordinator->current_step_index = 0; // Start at first step
            uint32_t step_count = gs_dyn_array_size(coordinator->current_plan_steps);
            gs_println("[DEBUG CHECKPOINT 2] After _extract_steps_from_plan: current_plan_steps=%p, size=%u", 
                (void*)coordinator->current_plan_steps, step_count);
            
            if (coordinator->config.enable_debug_logging) {
                gs_println("[INFO] Extracted %u steps from plan for execution", step_count);
            }
        } else {
            gs_println("[DEBUG CHECKPOINT 3] _extract_steps_from_plan returned FALSE");
            if (coordinator->config.enable_debug_logging) {
                gs_println("[WARNING] Failed to extract steps from plan");
            }
        }
        cJSON_Delete(plan_json);
        
        // Check array state AFTER cJSON_Delete
        gs_println("[DEBUG CHECKPOINT 4] After cJSON_Delete: current_plan_steps=%p, size=%u", 
            (void*)coordinator->current_plan_steps, 
            coordinator->current_plan_steps ? gs_dyn_array_size(coordinator->current_plan_steps) : 0);
    } else {
        if (coordinator->config.enable_debug_logging) {
            const char* error_ptr = cJSON_GetErrorPtr();
            gs_println("[WARNING] Failed to parse plan JSON for step extraction");
            if (error_ptr) {
                gs_println("[DEBUG] cJSON error near: '%.100s'", error_ptr);
            }
            gs_println("[DEBUG] Plan JSON length: %zu", strlen(coordinator->current_plan));
            gs_println("[DEBUG] Last 200 chars of plan: '%.200s'", 
                strlen(coordinator->current_plan) > 200 ? coordinator->current_plan + strlen(coordinator->current_plan) - 200 : coordinator->current_plan);
        }
    }
    
    // Add plan to conversation history
    char plan_msg[1024];
    snprintf(plan_msg, sizeof(plan_msg), "Plan created: %s", coordinator->current_plan);
    _add_to_history(coordinator, plan_msg);
    
    gs_core_ai_provider_response_free(&planning_response);
    gs_free(planning_prompt);
    
    result.success = true;
    return result;
}

// Execute agentic loop (ReAct pattern: Thought → Action → Observation)
static gs_core_ai_coordinator_result_t _execute_agentic_loop(
    gs_core_ai_coordinator_t* coordinator,
    const char* query,
    gs_core_ai_provider_config_t* config)
{
    uint32_t iterations = 0;
    uint32_t total_tokens = 0;
    
    // Initialize thoughts array
    if (!coordinator->thoughts) {
        coordinator->thoughts = NULL;
    }
    
    char* current_prompt = gs_malloc(strlen(query) + 1);
    strcpy(current_prompt, query);
    
    for (uint32_t iteration = 0; iteration < coordinator->config.max_iterations; iteration++) {
        if (coordinator->config.enable_debug_logging) {
            gs_println("=== Agentic Iteration %u ===", iteration);
            gs_println("Prompt: %s", current_prompt);
        }
        
        // Query provider
        gs_core_ai_provider_response_t response = coordinator->provider->request(
            coordinator->provider,
            current_prompt,
            config
        );
        
        iterations++;
        if (response.metadata.total_tokens > 0) {
            total_tokens += response.metadata.total_tokens;
        }
        
        if (!gs_core_ai_provider_response_is_success(&response)) {
            gs_core_ai_provider_response_free(&response);
            gs_free(current_prompt);
            return _build_error_result(response.error_message, iterations, total_tokens);
        }
        
        if (coordinator->config.enable_debug_logging) {
            gs_println("Response: %s", response.content);
        }
        
        // Extract thought if present
        char* thought = NULL;
        if (_extract_thought(response.content, &thought)) {
            gs_dyn_array_push(coordinator->thoughts, thought);
            if (coordinator->config.enable_debug_logging) {
                gs_println("Extracted thought: %s", thought);
            }
        }
        
        // Check if this is a tool call or final answer
        if (!_is_tool_call(response.content)) {
            // Final answer
            _add_to_history(coordinator, response.content);
            gs_core_ai_coordinator_result_t result = _build_success_result(response.content, iterations, total_tokens);
            gs_core_ai_provider_response_free(&response);
            gs_free(current_prompt);
            return result;
        }
        
        // Execute tool call
        gs_core_ai_coordinator_result_t tool_result = _execute_tool_call(
            coordinator, response.content, query, &iterations, &total_tokens);
        
        gs_core_ai_provider_response_free(&response);
        gs_free(current_prompt);
        
        if (!tool_result.success) {
            return tool_result;
        }
        
        // Continue with next prompt (observation from tool)
        current_prompt = tool_result.response;
        tool_result.response = NULL;
    }
    
    // Max iterations reached
    gs_free(current_prompt);
    char error[128];
    snprintf(error, sizeof(error), "Maximum iterations (%u) reached without final answer",
        coordinator->config.max_iterations);
    return _build_error_result(error, iterations, total_tokens);
}

// Execute plan-reflect loop (Plan → Execute step → Reflect → Adapt)
static gs_core_ai_coordinator_result_t _execute_plan_reflect_loop(
    gs_core_ai_coordinator_t* coordinator,
    const char* query,
    gs_core_ai_provider_config_t* config)
{
    uint32_t iterations = 0;
    uint32_t total_tokens = 0;
    
    coordinator->reflection_count = 0;
    coordinator->plan_revised = false;
    
    // Build initial prompt
    char* current_prompt = _build_initial_prompt(coordinator, query);
    
    for (uint32_t iteration = 0; iteration < coordinator->config.max_iterations; iteration++) {
        if (coordinator->config.enable_debug_logging) {
            gs_println("=== Plan-Reflect Iteration %u ===", iteration);
            gs_println("Prompt: %s", current_prompt);
        }
        
        // Query provider
        gs_core_ai_provider_response_t response = coordinator->provider->request(
            coordinator->provider,
            current_prompt,
            config
        );
        
        iterations++;
        if (response.metadata.total_tokens > 0) {
            total_tokens += response.metadata.total_tokens;
        }
        
        if (!gs_core_ai_provider_response_is_success(&response)) {
            gs_core_ai_provider_response_free(&response);
            gs_free(current_prompt);
            return _build_error_result(response.error_message, iterations, total_tokens);
        }
        
        if (coordinator->config.enable_debug_logging) {
            gs_println("Response: %s", response.content);
        }
        
        // Check for reflection/revision
        if (_is_reflection(response.content)) {
            char* reflection = NULL;
            if (_extract_reflection(response.content, &reflection)) {
                if (coordinator->last_reflection) {
                    gs_free(coordinator->last_reflection);
                }
                coordinator->last_reflection = reflection;
                coordinator->reflection_count++;
                
                if (_is_plan_revision(reflection)) {
                    if (_update_plan_from_reflection(coordinator, reflection)) {
                        if (coordinator->config.enable_debug_logging) {
                            gs_println("Plan revised based on reflection");
                        }
                    }
                }
                
                // Continue to next step after reflection
                gs_core_ai_provider_response_free(&response);
                gs_free(current_prompt);
                current_prompt = _build_initial_prompt(coordinator, query);
                continue;
            }
        }
        
        // Check if this is a tool call or final answer
        if (!_is_tool_call(response.content)) {
            // Final answer
            _add_to_history(coordinator, response.content);
            gs_core_ai_coordinator_result_t result = _build_success_result(response.content, iterations, total_tokens);
            gs_core_ai_provider_response_free(&response);
            gs_free(current_prompt);
            return result;
        }
        
        // Execute tool call
        gs_core_ai_coordinator_result_t tool_result = _execute_tool_call(
            coordinator, response.content, query, &iterations, &total_tokens);
        
        gs_core_ai_provider_response_free(&response);
        gs_free(current_prompt);
        
        if (!tool_result.success) {
            return tool_result;
        }
        
        // Continue with next prompt
        current_prompt = tool_result.response;
        tool_result.response = NULL;
    }
    
    // Max iterations reached
    gs_free(current_prompt);
    char error[128];
    snprintf(error, sizeof(error), "Maximum iterations (%u) reached without final answer",
        coordinator->config.max_iterations);
    return _build_error_result(error, iterations, total_tokens);
}

/*================================================================================
// Coordinator API Implementation
================================================================================*/

GS_API_DECL gs_core_ai_coordinator_t* 
gs_core_ai_coordinator_create(
    gs_core_ai_provider_t* provider,
    const gs_core_ai_coordinator_config_t* config)
{
    if (!provider) return NULL;
    
    gs_core_ai_coordinator_t* coordinator = gs_malloc(sizeof(gs_core_ai_coordinator_t));
    if (!coordinator) {
        if (provider->free) provider->free(provider);
        return NULL;
    }
    
    memset(coordinator, 0, sizeof(gs_core_ai_coordinator_t));
    coordinator->provider = provider;
    coordinator->tools = gs_dyn_array_new(gs_core_ai_tool_t);
    coordinator->conversation_history = gs_dyn_array_new(char*);
    
    // Initialize new fields for reflection and thought tracking
    coordinator->last_reflection = NULL;
    coordinator->thoughts = NULL;  // Will be initialized when needed
    coordinator->reflection_count = 0;
    coordinator->plan_revised = false;
    
    if (config) {
        coordinator->config = *config;
    } else {
        coordinator->config = GS_CORE_AI_COORDINATOR_CONFIG_DEFAULT();
    }
    
    return coordinator;
}

GS_API_DECL void 
gs_core_ai_coordinator_free(gs_core_ai_coordinator_t* coordinator)
{
    if (!coordinator) return;
    
    // Free tools array
    gs_dyn_array_free(coordinator->tools);
    
    // Free conversation history
    for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->conversation_history); i++) {
        gs_free(coordinator->conversation_history[i]);
    }
    gs_dyn_array_free(coordinator->conversation_history);
    
    // Free generated system prompt
    if (coordinator->generated_system_prompt) {
        gs_free(coordinator->generated_system_prompt);
    }
    
    // Free current plan
    if (coordinator->current_plan) {
        gs_free(coordinator->current_plan);
    }
    
    // Free plan steps array
    if (coordinator->current_plan_steps) {
        for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->current_plan_steps); i++) {
            cJSON_Delete((cJSON*)coordinator->current_plan_steps[i]);
        }
        gs_dyn_array_free(coordinator->current_plan_steps);
    }
    
    // Free reflection tracking
    if (coordinator->last_reflection) {
        gs_free(coordinator->last_reflection);
    }
    
    // Free thought history
    if (coordinator->thoughts) {
        for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->thoughts); i++) {
            gs_free(coordinator->thoughts[i]);
        }
        gs_dyn_array_free(coordinator->thoughts);
    }
    
    // Free provider
    if (coordinator->provider && coordinator->provider->free) {
        coordinator->provider->free(coordinator->provider);
    }
    
    gs_free(coordinator);
}

GS_API_DECL bool 
gs_core_ai_coordinator_register_tool(
    gs_core_ai_coordinator_t* coordinator,
    gs_core_ai_tool_t tool)
{
    if (!coordinator) return false;
    
    // Check for duplicate tool names
    for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->tools); i++) {
        if (strcmp(coordinator->tools[i].name, tool.name) == 0) {
            return false; // Tool already exists
        }
    }
    
    // Add tool to array
    gs_dyn_array_push(coordinator->tools, tool);
    
    // Regenerate system prompt (not for any specific phase)
    if (coordinator->generated_system_prompt) {
        gs_free(coordinator->generated_system_prompt);
    }
    coordinator->generated_system_prompt = _generate_system_prompt(coordinator, false);
    
    return true;
}

GS_API_DECL gs_core_ai_coordinator_result_t 
gs_core_ai_coordinator_query(
    gs_core_ai_coordinator_t* coordinator,
    const char* query,
    const gs_core_ai_provider_config_t* provider_config)
{
    // Validate inputs
    if (!coordinator || !query) {
        return _build_error_result("Invalid coordinator or query", 0, 0);
    }
    
    // Prepare provider config
    gs_core_ai_provider_config_t config = _prepare_provider_config(
        coordinator, provider_config, 
        coordinator->config.execution_mode == GS_CORE_AI_MODE_PLAN_REFLECT);
    
    // Clear previous plan and state
    if (coordinator->current_plan) {
        gs_free(coordinator->current_plan);
        coordinator->current_plan = NULL;
    }
    if (coordinator->last_reflection) {
        gs_free(coordinator->last_reflection);
        coordinator->last_reflection = NULL;
    }
    coordinator->reflection_count = 0;
    coordinator->plan_revised = false;
    
    // Add user query to conversation history
    _add_to_history(coordinator, query);
    
    // Dispatch based on execution mode
    switch (coordinator->config.execution_mode) {
        case GS_CORE_AI_MODE_AGENTIC: {
            if (coordinator->config.enable_debug_logging) {
                gs_println("=== AGENTIC MODE ===");
            }
            return _execute_agentic_loop(coordinator, query, &config);
        }
        
        case GS_CORE_AI_MODE_PLAN_REFLECT:
        default: {
            if (coordinator->config.enable_debug_logging) {
                gs_println("=== PLAN-REFLECT MODE ===");
            }
            
            // Phase 1: Planning
            gs_core_ai_coordinator_result_t planning_result = _execute_planning_phase(coordinator, query, &config);
            if (!planning_result.success) {
                return planning_result;
            }
            
            // Regenerate system prompt WITHOUT planning instructions for execution
            char* execution_system_prompt = _generate_system_prompt(coordinator, false);
            config.system_prompt = execution_system_prompt;
            
            // Phase 2: Execute with reflection
            gs_core_ai_coordinator_result_t exec_result = _execute_plan_reflect_loop(coordinator, query, &config);
            
            // Adjust token counts and iterations to include planning phase
            exec_result.iterations += planning_result.iterations;
            exec_result.total_tokens += planning_result.total_tokens;
            
            gs_free(execution_system_prompt);
            return exec_result;
        }
    }
}

GS_API_DECL void 
gs_core_ai_coordinator_clear_history(gs_core_ai_coordinator_t* coordinator)
{
    if (!coordinator) return;
    
    for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->conversation_history); i++) {
        gs_free(coordinator->conversation_history[i]);
    }
    gs_dyn_array_clear(coordinator->conversation_history);
}

GS_API_DECL uint32_t 
gs_core_ai_coordinator_get_tool_count(const gs_core_ai_coordinator_t* coordinator)
{
    if (!coordinator) return 0;
    return gs_dyn_array_size(coordinator->tools);
}

GS_API_DECL gs_core_ai_tool_t* 
gs_core_ai_coordinator_find_tool(
    gs_core_ai_coordinator_t* coordinator,
    const char* tool_name)
{
    if (!coordinator || !tool_name) return NULL;
    
    for (uint32_t i = 0; i < gs_dyn_array_size(coordinator->tools); i++) {
        if (strcmp(coordinator->tools[i].name, tool_name) == 0) {
            return &coordinator->tools[i];
        }
    }
    
    return NULL;
}

GS_API_DECL uint32_t 
gs_core_ai_register_default_tools(
    gs_core_ai_coordinator_t* coordinator,
    const gs_core_ai_default_tools_config_t* config)
{
    if (!coordinator) return 0;
    
    // Use default config if none provided
    gs_core_ai_default_tools_config_t default_config = GS_CORE_AI_DEFAULT_TOOLS_CONFIG_DEFAULT();
    if (!config) {
        config = &default_config;
    }
    
    // Track initial tool count
    uint32_t initial_count = gs_core_ai_coordinator_get_tool_count(coordinator);
    
    // Register file system tools
    if (config->enable_fs_tools) {
        gs_core_ai_tools_fs_register(coordinator);
    }
    
    // Register HTTP tools
    if (config->enable_http_tools) {
        gs_core_ai_tools_http_register(coordinator);
    }
    
    // Register code search tools
    if (config->enable_code_tools) {
        gs_core_ai_tools_code_register(coordinator);
    }
    
    // Calculate how many tools were registered
    uint32_t final_count = gs_core_ai_coordinator_get_tool_count(coordinator);
    return final_count - initial_count;
}

GS_API_DECL void 
gs_core_ai_coordinator_result_free(gs_core_ai_coordinator_result_t* result)
{
    if (!result) return;
    
    if (result->response) {
        gs_free(result->response);
        result->response = NULL;
    }
    
    if (result->error_message) {
        gs_free(result->error_message);
        result->error_message = NULL;
    }
}
