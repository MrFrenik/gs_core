/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: sm_network.h

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

#ifndef CORE_NETWORK_H
#define CORE_NETWORK_H

#include "core_gs.h"

#define CORE_NETWORK_ADDRESS_STR_MAX    64 
#define CORE_NETWORK_SERVER_ID          0
#define CORE_NETWORK_CLIENT_ID_START    (CORE_NETWORK_SERVER_ID + 1)

// Netsync flags
enum 
{
    CORE_NETWORK_FLAG_HAS_AUTHORITY = (1 << 0),
    CORE_NETWORK_FLAG_IS_LOCAL      = (1 << 1)
};

gs_force_inline bool 
gs_core_network_is_local(uint32_t flags)
{
    return (flags & CORE_NETWORK_FLAG_IS_LOCAL);
}

gs_force_inline bool 
gs_core_network_has_authority(uint32_t flags)
{
    return (flags & CORE_NETWORK_FLAG_HAS_AUTHORITY);
}

enum
{
    CORE_NETWORK_CHANNEL_UNRELIABLE = 0x00,
    CORE_NETWORK_CHANNEL_RELIABLE
};

// Macros
#define core_network_rpc_send(NET, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        core_cls_init(T, &_RPC);\
        core_cast(&_RPC, core_network_rpc_t)->delivery = CORE_NETWORK_CHANNEL_UNRELIABLE;\
        core_network_rpc_send_internal((NET), &_RPC);\
    } while (0)

#define core_network_rpc_send_reliable(NET, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        core_cls_init(T, &_RPC);\
        core_cast(&_RPC, core_network_rpc_t)->delivery = CORE_NETWORK_CHANNEL_RELIABLE;\
        core_network_rpc_send_internal((NET), &_RPC);\
    } while (0)

#define core_network_rpc_send_id(NET, ID, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        core_cls_init(T, &_RPC);\
        core_cast(&_RPC, core_network_rpc_t)->delivery = CORE_NETWORK_CHANNEL_UNRELIABLE;\
        core_network_rpc_send_id_internal((NET), (ID), &_RPC);\
    } while (0) 

#define core_network_rpc_send_id_reliable(NET, ID, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        core_cls_init(T, &_RPC);\
        core_cast(&_RPC, core_network_rpc_t)->delivery = CORE_NETWORK_CHANNEL_RELIABLE;\
        core_network_rpc_send_id_internal((NET), (ID), &_RPC);\
    } while (0) 

#define core_network_rpc_receive(NET, EVT)\
    do {\
        core_network_rpc_receive_internal((NET), (EVT));\
    } while (0)

// Forward Decls
struct core_network_rpc_s;

typedef void (* core_network_rpc_cb)(struct core_obj_t* rpc); 

typedef enum
{
    CORE_NETWORK_HOST_INVALID = 0x00,
    CORE_NETWORK_HOST_CLIENT,
    CORE_NETWORK_HOST_SERVER,
} core_network_host_type_t; 

typedef enum
{
    CORE_NETWORK_DELIVERY_UNRELIABLE = 0x00,
    CORE_NETWORK_DELIVERY_RELIABLE
} core_network_delivery_type_t;

_introspect()
typedef struct
{
    core_base(core_obj_t);
    core_network_rpc_cb callback;
    core_network_delivery_type_t delivery;
    uint32_t id;                           // Sending client peer id
} core_network_rpc_t;

// Core packet
typedef struct
{
    size_t start;
    gs_byte_buffer_t* buffer;
} core_network_packet_t;

typedef void* core_network_host_handle_t;

typedef enum
{
    CORE_NETWORK_MESSAGE_CONNECT = 0x00,
    CORE_NETWORK_MESSAGE_DISCONNECT,
    CORE_NETWORK_MESSAGE_DATA,
    CORE_NETWORK_MESSAGE_RPC
} core_network_event_type_t;

typedef void (* core_network_rpc_cb)(void* data, size_t sz); 

typedef struct
{
    core_network_delivery_type_t delivery;  // Delivery type
    uint32_t peer_id;                       // Id of peer to send message to
    void* data;                             // Serialized message data
    size_t size;                            // Size of data packet
} core_network_message_t;

typedef struct
{
    core_network_event_type_t type; // Type of event
    uint32_t peer_id;               // Peer id
    void* data;                     // Data to receive
    size_t size;                    // Size of data packet
} core_network_event_t; 

typedef struct
{
    core_network_host_type_t type;
    uint32_t max_connections;
    uint32_t max_channels;
} core_network_host_desc_t;

typedef struct core_network_s
{
    void* user_data;
} core_network_t;

//=====[ Core_Network ]====//

GS_API_DECL core_network_t* 
core_network_new(); 

GS_API_DECL core_network_t* 
core_network_instance(); 

GS_API_DECL void
core_network_update();

GS_API_DECL void            
core_network_shutdown(); 

GS_API_DECL core_network_host_handle_t
core_network_server_create(uint32_t port, uint32_t max_connections);

GS_API_DECL core_network_host_handle_t 
core_network_client_create(const char* address, uint32_t port);

GS_API_DECL bool
core_network_is_server(core_network_t* net);

GS_API_DECL bool
core_network_is_client(core_network_t* net);

GS_API_DECL int32_t 
core_network_id(core_network_t* net);

//====[ Core Network Packet ]====//

GS_API_DECL core_network_packet_t
core_network_packet_request(core_network_t* net);

//=====[ Core_NetworkHost ]====//
//
GS_API_DECL bool 
core_network_poll(core_network_t* net, 
        core_network_event_t* evt); 

GS_API_DECL void 
core_network_host_destroy(core_network_host_handle_t hndl); 

GS_API_DECL void 
core_network_disconnect(core_network_t* net); 

GS_API_DECL core_network_host_type_t 
core_network_get_type(core_network_t* net); 

GS_API_DECL uint32_t 
core_network_num_connections(core_network_t* net); 

GS_API_DECL bool 
core_network_is_connected(core_network_t* net);

GS_API_DECL bool
core_network_is_valid(core_network_t* net);

//====[ Core_NetworkMessage ]====//
//
GS_API_DECL void 
core_network_send_message(core_network_t* net, 
        const core_network_message_t* msg);

GS_API_DECL void 
core_network_broadcast(core_network_t* net,
        const core_network_message_t* msg); 

//====[ Core RPC ]====//

GS_API_DECL void
core_network_rpc_receive_internal(core_network_t* net, core_network_event_t* evt);

GS_API_DECL void
core_network_rpc_send_internal(core_network_t* net, core_network_rpc_t* rpc);

GS_API_DECL void
core_network_rpc_send_id_internal(core_network_t* net, const uint32_t id, core_network_rpc_t* rpc);

//====[ Core Packet ]====//

GS_API_DECL core_network_packet_t
core_network_packet_request(core_network_t* net);

GS_API_DECL uint8_t*
core_network_packet_data(core_network_t* net, const core_network_packet_t* packet);

GS_API_DECL size_t 
core_network_packet_size(core_network_t* net, const core_network_packet_t* packet); 

#endif // CORE_NETWORK_H












