#ifndef %APP%_H
#define %APP%_H

#include "core/core.h"

typedef struct %APP%_s
{ 
    // Core
    core_t* core;

    // Internal data
    void* data; 

} %APP%_t; 

GS_API_DECL void 
%APP%_init(); 

GS_API_DECL void 
%APP%_update(); 

GS_API_DECL void 
%APP%_shutdown(); 

#endif