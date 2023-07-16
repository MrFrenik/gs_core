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

#ifndef GS_CORE_NETWORK_H
#define GS_CORE_NETWORK_H

#include "gs_core_gs.h"

#define GS_CORE_NETWORK_ADDRESS_STR_MAX    64 
#define GS_CORE_NETWORK_SERVER_ID          0
#define GS_CORE_NETWORK_CLIENT_ID_START    (GS_CORE_NETWORK_SERVER_ID + 1)

// Netsync flags
enum 
{
    GS_CORE_NETWORK_FLAG_HAS_AUTHORITY = (1 << 0),
    GS_CORE_NETWORK_FLAG_IS_LOCAL      = (1 << 1),
    GS_CORE_NETWORK_FLAG_HAS_OWNERSHIP = (1 << 2)
};

// Message event callback
typedef void (* gs_core_network_event_message_cb_t)(uint32_t peer_id);

gs_force_inline bool 
gs_core_network_is_local(uint32_t flags)
{
    return (flags & GS_CORE_NETWORK_FLAG_IS_LOCAL);
}

gs_force_inline bool 
gs_core_network_has_authority(uint32_t flags)
{
    return (flags & GS_CORE_NETWORK_FLAG_HAS_AUTHORITY);
}

gs_force_inline bool 
gs_core_network_has_ownership(uint32_t flags)
{
    return (flags & GS_CORE_NETWORK_FLAG_HAS_OWNERSHIP);
}

enum
{
    GS_CORE_NETWORK_CHANNEL_UNRELIABLE = 0x00,
    GS_CORE_NETWORK_CHANNEL_RELIABLE
};

// Macros
#define gs_core_network_rpc_send(NET, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        gs_core_cls_init(T, &_RPC);\
        gs_core_network_rpc_send_internal((NET), &_RPC);\
    } while (0)

#define gs_core_network_rpc_local(NET, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        gs_core_cls_init(T, &_RPC);\
        gs_core_network_rpc_local_internal((NET), &_RPC);\
    } while (0)

#define gs_core_network_rpc_send_id(NET, ID, T, ...)\
    do {\
        T _RPC = (T)__VA_ARGS__;\
        gs_core_cls_init(T, &_RPC);\
        gs_core_network_rpc_send_id_internal((NET), (ID), &_RPC);\
    } while (0) 

#define gs_core_network_rpc_receive(NET, EVT)\
    do {\
        gs_core_network_rpc_receive_internal((NET), (EVT));\
    } while (0) 

typedef enum
{
    GS_CORE_NETWORK_HOST_INVALID = 0x00,
    GS_CORE_NETWORK_HOST_CLIENT,
    GS_CORE_NETWORK_HOST_SERVER,
} gs_core_network_host_type_t; 

typedef enum
{
    GS_CORE_NETWORK_DELIVERY_UNRELIABLE = 0x00,
    GS_CORE_NETWORK_DELIVERY_RELIABLE
} gs_core_network_delivery_type_t;

_introspect()
typedef struct gs_core_network_rpc_t
{
    gs_core_base(gs_core_obj_t);

    // VTable
    _vtable(
        void (* callback)(struct gs_core_network_rpc_t* rpc) = gs_core_network_rpc_cb_default;
    )

    gs_core_network_delivery_type_t delivery;
    uint32_t id;                                // Sending client peer id
} gs_core_network_rpc_t;

_introspect()
typedef struct
{
    gs_core_base(gs_core_network_rpc_t);
} gs_core_network_rpc_reliable_t;

_introspect()
typedef struct
{
    gs_core_base(gs_core_network_rpc_t);
} gs_core_network_rpc_unreliable_t;

GS_API_DECL void
gs_core_network_rpc_cb_default(gs_core_network_rpc_t* rpc);

// Core packet
typedef struct
{
    size_t start;
    gs_byte_buffer_t* buffer;
} gs_core_network_packet_t;

typedef void* gs_core_network_host_handle_t;

typedef enum
{
    GS_CORE_NETWORK_MESSAGE_CONNECT = 0x00,
    GS_CORE_NETWORK_MESSAGE_DISCONNECT,
    GS_CORE_NETWORK_MESSAGE_DATA,
    GS_CORE_NETWORK_MESSAGE_RPC
} gs_core_network_event_type_t;

typedef struct
{
    gs_core_network_delivery_type_t delivery;   // Delivery type
    uint32_t peer_id;                           // Id of peer to send message to
    void* data;                                 // Serialized message data
    size_t size;                                // Size of data packet
} gs_core_network_message_t;

typedef struct
{
    gs_core_network_event_type_t type;  // Type of event
    uint32_t peer_id;                   // Peer id
    void* data;                         // Data to receive
    size_t size;                        // Size of data packet
} gs_core_network_event_t; 

typedef struct
{
    gs_core_network_host_type_t type;
    uint32_t max_connections;
    uint32_t max_channels;
} gs_core_network_host_desc_t;

typedef struct gs_core_network_s
{
    void* user_data;
} gs_core_network_t;

//=====[ Core_Network ]====//

GS_API_DECL gs_core_network_t* 
gs_core_network_new(); 

GS_API_DECL gs_core_network_t* 
gs_core_network_instance(); 

GS_API_DECL void
gs_core_network_update();

GS_API_DECL void            
gs_core_network_shutdown(); 

GS_API_DECL gs_core_network_host_handle_t
gs_core_network_server_create(uint32_t port, uint32_t max_connections);

GS_API_DECL gs_core_network_host_handle_t 
gs_core_network_client_create(const char* address, uint32_t port);

GS_API_DECL bool
gs_core_network_is_server(gs_core_network_t* net);

GS_API_DECL bool
gs_core_network_is_client(gs_core_network_t* net);

GS_API_DECL int32_t 
gs_core_network_id(gs_core_network_t* net);

GS_API_DECL void
gs_core_network_peer_connect_callback_set(gs_core_network_t* net, gs_core_network_event_message_cb_t cb);

GS_API_DECL void
gs_core_network_peer_disconnect_callback_set(gs_core_network_t* net, gs_core_network_event_message_cb_t cb);

//====[ Core Network Packet ]====//

GS_API_DECL gs_core_network_packet_t
gs_core_network_packet_request(gs_core_network_t* net);

//=====[ Core_NetworkHost ]====//
//
GS_API_DECL bool 
gs_core_network_poll(gs_core_network_t* net, 
        gs_core_network_event_t* evt); 

GS_API_DECL void 
gs_core_network_host_destroy(gs_core_network_host_handle_t hndl); 

GS_API_DECL void 
gs_core_network_disconnect(gs_core_network_t* net); 

GS_API_DECL gs_core_network_host_type_t 
gs_core_network_get_type(gs_core_network_t* net); 

GS_API_DECL uint32_t 
gs_core_network_num_connections(gs_core_network_t* net); 

GS_API_DECL bool 
gs_core_network_is_connected(gs_core_network_t* net);

GS_API_DECL bool
gs_core_network_is_valid(gs_core_network_t* net);

//====[ Core_NetworkMessage ]====//
//
GS_API_DECL void 
gs_core_network_send_message(gs_core_network_t* net, 
        const gs_core_network_message_t* msg);

GS_API_DECL void 
gs_core_network_broadcast(gs_core_network_t* net,
        const gs_core_network_message_t* msg); 

//====[ Core RPC ]====//

GS_API_DECL void
gs_core_network_rpc_receive_internal(gs_core_network_t* net, gs_core_network_event_t* evt);

GS_API_DECL void
gs_core_network_rpc_send_internal(gs_core_network_t* net, gs_core_network_rpc_t* rpc);

GS_API_DECL void
gs_core_network_rpc_local_internal(gs_core_network_t* net, gs_core_network_rpc_t* rpc);

GS_API_DECL void
gs_core_network_rpc_send_id_internal(gs_core_network_t* net, const uint32_t id, gs_core_network_rpc_t* rpc);

//====[ Core Packet ]====//

GS_API_DECL gs_core_network_packet_t
gs_core_network_packet_request(gs_core_network_t* net);

GS_API_DECL uint8_t*
gs_core_network_packet_data(gs_core_network_t* net, const gs_core_network_packet_t* packet);

GS_API_DECL size_t 
gs_core_network_packet_size(gs_core_network_t* net, const gs_core_network_packet_t* packet); 

#endif // GS_CORE_NETWORK_H












