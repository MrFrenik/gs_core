/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: gs_core_util.cpp

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2022 John Jackson

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

// Core Includes
#include "gs_core_util.h"

// Third Party Includes
#include "filewatch/FileWatch.hpp"

#include <locale>
#include <codecvt>
#include <string>

GS_API_DECL gs_core_util_filewatch_t* 
gs_core_util_filewatch_new(const char* file_path, gs_core_util_filewatch_cb cb)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    // std::string narrow = converter.to_bytes(wide_utf16_source_string);
    std::wstring wide = converter.from_bytes(file_path);

    filewatch::FileWatch<std::wstring>* watch = new 
        filewatch::FileWatch<std::wstring>(
            wide,
            [&](const std::wstring& path, const filewatch::Event evt) { 
                cb();
            }
        );

    return (gs_core_util_filewatch_t*)watch; 
} 

GS_API_DECL void 
gs_core_util_filewatch_free(gs_core_util_filewatch_t* watch)
{
    delete (watch);
}














