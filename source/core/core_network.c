/*==============================================================================================================
    * Copyright: 2022 John Jackson 
    * File: core_network.c

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
#include "core_gs.h"
#include "core_network.h"

// Third party includes
#include <enet/enet.h>

#define CORE_ENET()       ((core_enet_t*)g_network->user_data)

// Broadcast to all clients
// Server holds list of all connected clients
// Ability to poll all messages stored, for both client and server
// Ability to start/stop server and client hosts 

typedef struct
{
    ENetAddress address;            // Hosting address
    ENetHost* host;                 // Either server/client
    core_network_host_type_t type;  // Type of host
    uint16_t max_connections;       // Max number of outgoing connections
    uint16_t max_channels;          // Max number of channels
    uint32_t id;                    // Id of host
    union
    {
        struct 
        {
            ENetPeer* server;
        } client;

        struct 
        {
            gs_hash_table(uint32_t, ENetPeer*) clients;
        } server;
    };
    gs_byte_buffer_t message_data;
} core_enet_t;

static core_network_t* g_network = NULL;

GS_API_DECL void 
core_enet_server_disconnect(core_enet_t* enet);

GS_API_DECL void 
core_enet_server_remove_client(core_enet_t* enet, uint32_t peer_id);

GS_API_DECL bool 
core_enet_server_poll(core_enet_t* enet, core_network_message_t* msg);

GS_API_DECL void 
core_enet_server_destroy(core_enet_t* enet);

GS_API_DECL uint32_t 
core_enet_server_num_clients(core_enet_t* enet);

GS_API_DECL void 
core_enet_server_send_message(core_enet_t* enet, const core_network_message_t* msg);

GS_API_DECL void 
core_enet_server_broadcast(core_enet_t* enet, const core_network_message_t* msg);

GS_API_DECL ENetPeer* 
core_enet_server_get_client(core_enet_t* enet, uint32_t id); 

GS_API_DECL void 
core_enet_client_disconnect(core_enet_t* enet);

GS_API_DECL bool 
core_enet_client_poll(core_enet_t* enet, core_network_message_t* msg);

GS_API_DECL void 
core_enet_client_destroy(core_enet_t* enet);

GS_API_DECL void 
core_enet_client_send_message(core_enet_t* enet, const core_network_message_t* msg);

//============[ Implementation ] ============//

GS_API_DECL core_network_t* 
core_network_new()
{
    if (g_network) return g_network;

    core_network_t* network = gs_malloc_init(core_network_t);
    g_network = network;

    // Set user data for enet
    network->user_data = (core_enet_t*)gs_malloc_init(core_enet_t);

    // Construct message data buffer
    CORE_ENET()->message_data = gs_byte_buffer_new();

    if (enet_initialize() != 0)
    {
        gs_log_error("core_network_t::New::Unable to initialize ENet");
        return NULL;
    } 

    return network;
}

GS_API_DECL core_network_t* 
core_network_instance()
{
    return g_network;
} 

GS_API_DECL void core_network_shutdown()
{ 
    core_enet_t* enet = CORE_ENET();
    core_network_host_destroy(enet);
    gs_byte_buffer_free(&enet->message_data);
}

GS_API_DECL core_network_host_handle_t
core_network_server_create(uint32_t port, uint32_t max_connections)
{
    core_enet_t* enet = CORE_ENET(); 
    if (enet->host) return enet; 

    enet->type = CORE_NETWORK_HOST_SERVER;
    enet->max_channels = 2;
    enet->max_connections = max_connections; 

    enet->address.host = ENET_HOST_ANY;
    enet->address.port = port;
    enet->id = CORE_NETWORK_SERVER_ID;

    // Create server 
    enet->host = enet_host_create(
            &enet->address,         // Address to bind server host to
            enet->max_connections,  // Allow up to N client/outgoing connections
            enet->max_channels,     // Allow up to N channels to be used
            0,                      // Assume any amount of incoming bandwidth
            0                       // Assume any amount of outgoing bandwidth
    );

    if (enet->host == NULL)
    {
        gs_log_warning("Core_Network_Server::Error occurred while trying to create server host");
        return NULL;
    }

    // Successful server connection
    gs_log_success("core_network_host_connect::Server started on %x : port %u.", 
        enet->host, enet->address.port); 

    return enet;
}

GS_API_DECL core_network_host_handle_t 
core_network_client_create(const char* address, uint32_t port)
{
    core_enet_t* enet = CORE_ENET(); 

    // Create client
    enet->type = CORE_NETWORK_HOST_CLIENT;
    enet->host = enet_host_create(NULL, 1, 1, 0, 0);
    enet->max_channels = 2;
    enet->max_connections = 1;

    if (enet->host == NULL)
    {
        gs_log_warning("Core_Network_Host_Connect::No available peers for initiating ENet connection.");
        return NULL;
    } 

    enet_address_set_host(&enet->address, address);
    enet->address.port = port;

    // Initiate connect, allocating two channels 0 and 1
    enet->client.server = enet_host_connect(enet->host, &enet->address, enet->max_channels, 0);

    if (!enet->client.server)
    {
        gs_log_warning("Core_Network_Host_Connect::No available peers for initiating ENet connection.");
        return NULL;
    }

    // Attempt to connect to server
    ENetEvent evt;

    // Wait 5000ms for connection to succeed
    const uint32_t timeout_ms = 5000;
    if (enet_host_service(enet->host, &evt, timeout_ms) > 0 && evt.type == ENET_EVENT_TYPE_CONNECT)
    {
        // Set peer id
        enet->id = evt.peer->outgoingPeerID + CORE_NETWORK_CLIENT_ID_START;

        // Successful connection
        gs_log_success("Core_Network_Host_Connect::Connected to %x on port %zu, id: %zu", 
            enet->host, enet->address.port, enet->id); 

        return enet;
    }

    // Fail to connect
    gs_log_warning("Core_Network_Host_Connect::Failed to connect to %x on port %zu.", 
        enet->host, enet->address.port);
    enet_peer_reset(enet->client.server);
    enet->client.server = NULL;
    core_network_host_destroy(enet);
    return NULL;
} 

GS_API_DECL bool
core_network_is_server(core_network_t* net) 
{
    core_enet_t* enet = CORE_ENET();
    if (!enet->host) return false;
    return core_network_get_type(net) == CORE_NETWORK_HOST_SERVER;
}

GS_API_DECL bool
core_network_is_client(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET();
    if (!enet->host) return false;
    return core_network_get_type(net) == CORE_NETWORK_HOST_CLIENT;
}

GS_API_DECL void 
core_network_host_destroy(core_network_host_handle_t hndl)
{
    core_enet_t* enet = CORE_ENET();
    if (hndl != enet) return;
    switch (enet->type)
    {
        case CORE_NETWORK_HOST_SERVER:    core_enet_server_destroy(enet); break;
        case CORE_NETWORK_HOST_CLIENT:    core_enet_client_destroy(enet); break;
    }
    enet->type = CORE_NETWORK_HOST_INVALID;
}

GS_API_DECL bool 
core_network_poll(core_network_t* net, core_network_event_t* evt)
{ 
    core_enet_t* enet = CORE_ENET();
    if (!evt || !enet->host) return false;
	bool ret = false;

	// Seek to beginning for writing
	gs_byte_buffer_seek_to_beg(&enet->message_data);

    switch (enet->type)
    {
        case CORE_NETWORK_HOST_SERVER: ret = core_enet_server_poll(enet, evt); break;
        case CORE_NETWORK_HOST_CLIENT: ret = core_enet_client_poll(enet, evt); break;
    }

	// Seek to beginning for reading
	gs_byte_buffer_seek_to_beg(&enet->message_data);

    return ret;
} 

GS_API_DECL void 
core_network_disconnect(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET(); 
    if (net->user_data != enet || !core_network_is_connected(net)) return;
    switch (enet->type)
    {
        case CORE_NETWORK_HOST_SERVER: core_enet_server_disconnect(enet); break;
        case CORE_NETWORK_HOST_CLIENT: core_enet_client_disconnect(enet); break;
    } 
}

GS_API_DECL bool 
core_network_is_connected(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET(); 
    if (net->user_data != enet || !enet->host) return false;
    return (enet->host->connectedPeers > 0);
}

GS_API_DECL bool
core_network_is_valid(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET(); 
    if (net->user_data != enet || !enet->host) return false;
    return true;
}

GS_API_DECL core_network_host_type_t
core_network_get_type(core_network_t* net)
{ 
    core_enet_t* enet = CORE_ENET(); 
    if (net->user_data != enet || !enet->host)
    {
        // Log warning
        gs_log_warning("Core Network Host::Enet host is invalid.");
        return CORE_NETWORK_HOST_INVALID;
    }

    return enet->type;
}

GS_API_DECL uint32_t 
core_network_num_connections(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET(); 
    if (net->user_data != enet || !enet->host)
    {
        return 0;
    }

    return enet->host->connectedPeers;
}

GS_API_DECL int32_t 
core_network_id(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET(); 
    if (!enet || !enet->host)
    {
        return -1;
    }

    return enet->id;
} 

GS_API_DECL void 
core_network_send_message(core_network_t* net, 
    const core_network_message_t* message)
{
    core_enet_t* enet = CORE_ENET(); 
    if (!enet->host) return;
    switch (enet->type)
    {
        case CORE_NETWORK_HOST_SERVER: core_enet_server_send_message(enet, message); break;
        case CORE_NETWORK_HOST_CLIENT: core_enet_client_send_message(enet, message); break;
    }
}

GS_API_DECL void 
core_network_broadcast(core_network_t* net, 
    const core_network_message_t* message)
{
    core_enet_t* enet = CORE_ENET(); 
    if (!enet->host) return;
    switch (enet->type)
    { 
        case CORE_NETWORK_HOST_SERVER: core_enet_server_broadcast(enet, message); break;
        case CORE_NETWORK_HOST_CLIENT: core_enet_client_send_message(enet, message); break;
    } 
} 

GS_API_DECL void
core_network_rpc_send_internal(core_network_t* net, core_network_rpc_t* rpc) 
{
    core_enet_t* enet = CORE_ENET(); 

    // Request internal packet from network using message buffer
    core_network_packet_t packet = core_network_packet_request(net);

    // Write out cls id of rpc
    gs_byte_buffer_write(packet.buffer, uint32_t, core_obj_cls_id(rpc));

    // Net serialize rpc into buffer
    core_obj_net_serialize(packet.buffer, rpc);
	
	uint8_t* data = core_network_packet_data(net, &packet);
	size_t sz = core_network_packet_size(net, &packet);

    // Construct message 
    core_network_message_t _msg = {
        .delivery = core_cast(&rpc, core_network_rpc_t)->delivery,
        .data = core_network_packet_data(net, &packet),
        .size = core_network_packet_size(net, &packet)
    };

    // Broadcast message
    core_network_broadcast(net, &_msg);
} 

GS_API_DECL void
core_network_rpc_send_id_internal(core_network_t* net, 
        const uint32_t id, 
        core_network_rpc_t* rpc)
{
    core_enet_t* enet = CORE_ENET(); 

    // Request internal packet from network using message buffer
    core_network_packet_t packet = core_network_packet_request(net);

    // Write out cls id of rpc
    gs_byte_buffer_write(packet.buffer, uint32_t, core_obj_cls_id(rpc));

    // Net serialize rpc into buffer
    core_obj_net_serialize(packet.buffer, rpc);
	
	uint8_t* data = core_network_packet_data(net, &packet);
	size_t sz = core_network_packet_size(net, &packet);

    // Construct message 
    core_network_message_t _msg = {
        .delivery = core_cast(&rpc, core_network_rpc_t)->delivery,
        .peer_id = id,
        .data = core_network_packet_data(net, &packet),
        .size = core_network_packet_size(net, &packet)
    };

    // Broadcast message
    core_network_send_message(net, &_msg);
}

GS_API_DECL void
core_network_rpc_receive_internal(core_network_t* net, 
    core_network_event_t* evt)
{
    core_enet_t* enet = CORE_ENET(); 

    // Read class id
    gs_byte_buffer_readc(&enet->message_data, uint32_t, cls_id); 

    // Get class
    const core_meta_info_t* info = core_info_w_cls_id(cls_id);

    // Make sure IS base of network rpc
    if (!core_info_base_of(info, core_network_rpc_t))
    {
        // Log warning
        gs_log_warning("Class does not inherit from rpc");
        return;
    }

    // Get reference to static object storage for class id
    core_obj_t* sobj = core_obj_static_ref_w_info(info);

    // Deserialize rpc
    core_obj_net_deserialize(&enet->message_data, sobj);

    // Cast to net rpc
    core_network_rpc_t* rpc = core_cast(sobj, core_network_rpc_t);

    // Set peer id for received rpc
    rpc->id = evt->peer_id;

    // Assert callback exists for rpc
    gs_assert(rpc && rpc->callback);

    // Call function pointer for net rpc
    rpc->callback(rpc, info->size);
}

//=========[ Core Network Packet ]========//

GS_API_DECL core_network_packet_t
core_network_packet_request(core_network_t* net)
{
    core_enet_t* enet = CORE_ENET();
    core_network_packet_t packet = {0};
    packet.buffer = &enet->message_data;
    packet.start = enet->message_data.position;
    return packet;
}

GS_API_DECL uint8_t*
core_network_packet_data(core_network_t* net, const core_network_packet_t* packet)
{
    return (uint8_t*)(packet->buffer->data + packet->start);
}

GS_API_DECL size_t 
core_network_packet_size(core_network_t* net, const core_network_packet_t* packet)
{
    return (packet->buffer->position - packet->start);
}

//=========[ Core_ENet_Server ]========//

GS_API_DECL void 
core_enet_server_destroy(core_enet_t* enet)
{
    if (core_network_is_connected(core_network_instance()))
    {
        gs_println("SERVER: disconnecting server");
        core_enet_server_disconnect(enet);
    }
    enet_host_destroy(enet->host);
}

GS_API_DECL void 
core_enet_server_disconnect(core_enet_t* enet)
{ 
    // Have to keep track of all peers and disconnect them manually
    for (
        gs_hash_table_iter it = gs_hash_table_iter_new(enet->server.clients);
        gs_hash_table_iter_valid(enet->server.clients, it);
        gs_hash_table_iter_advance(enet->server.clients, it)
    )
    {
        ENetPeer* client = gs_hash_table_iter_get(enet->server.clients, it);
        enet_peer_disconnect(client, 0);
    }

    ENetEvent evt;
    while (enet_host_service(enet->host, &evt, 0) > 0)
    {
        switch (evt.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                enet_packet_destroy(evt.packet);
            } break;

            case ENET_EVENT_TYPE_CONNECT:
            {
                if (evt.peer)
                {
                    enet_peer_disconnect(evt.peer, 0);
                }
            } break;

            case ENET_EVENT_TYPE_DISCONNECT:
            { 
                core_enet_server_remove_client(enet, evt.peer->incomingPeerID + CORE_NETWORK_CLIENT_ID_START);
            } break;
        }
    } 
}

GS_API_DECL ENetPeer* 
core_enet_server_get_client(core_enet_t* enet, uint32_t id)
{
    if (gs_hash_table_exists(enet->server.clients, id))
    {
        return gs_hash_table_get(enet->server.clients, id);
    }
    return NULL;
}

GS_API_DECL void 
core_enet_server_remove_client(core_enet_t* enet, uint32_t peer_id)
{
    gs_hash_table_erase(enet->server.clients, peer_id);
}

GS_API_DECL bool 
core_enet_server_poll(core_enet_t* enet, core_network_event_t* out)
{ 
    ENetEvent evt;
    gs_byte_buffer_t* mdata = &enet->message_data;

    while (enet_host_service(enet->host, &evt, 0) > 0)
    {
        uint32_t type = evt.type;
        uint32_t peer_id = (evt.peer->incomingPeerID + CORE_NETWORK_CLIENT_ID_START);
        switch (evt.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            { 
                gs_hash_table_insert(enet->server.clients, peer_id, evt.peer);
                out->type = CORE_NETWORK_MESSAGE_CONNECT;
                out->peer_id = peer_id;
                return true;
            } break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                ENetPacket* packet = evt.packet;

                out->type = CORE_NETWORK_MESSAGE_DATA;
                out->peer_id = peer_id;
                out->data = mdata->data;
                out->size = packet->dataLength;

                // Store data into message data buffer
                gs_byte_buffer_write_bulk(mdata, packet->data, packet->dataLength); 

                // Destroy packet after using
                enet_packet_destroy(evt.packet); 

                return true;
            } break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                core_enet_server_remove_client(enet, peer_id);
                out->type = CORE_NETWORK_MESSAGE_DISCONNECT;
                out->peer_id = peer_id;
                return true;
            } break;
        }
    }

    // Clear all message data for next poll
    gs_byte_buffer_clear(mdata);
    gs_byte_buffer_seek_to_beg(mdata);

    return false;
}

GS_API_DECL uint32_t 
core_enet_server_num_clients(core_enet_t* enet)
{
    return enet->host->connectedPeers;
} 

GS_API_DECL void 
core_enet_server_send_message(core_enet_t* enet, const core_network_message_t* message)
{
    // Grab peer and check if valid
    ENetPeer* peer = core_enet_server_get_client(enet, message->peer_id);
    if (!peer)
    {
        gs_log_warning("Send Message: Peer %zu does not exist.", message->peer_id);
    }

    uint32_t flags = 0; 
    uint32_t channel = 0;
    switch (message->delivery)
    {
        case CORE_NETWORK_DELIVERY_RELIABLE:   
        {
            flags = ENET_PACKET_FLAG_RELIABLE;
            channel = CORE_NETWORK_CHANNEL_RELIABLE;
        } break;
        case CORE_NETWORK_DELIVERY_UNRELIABLE: 
        {
            flags = ENET_PACKET_FLAG_UNSEQUENCED;
            channel = CORE_NETWORK_CHANNEL_UNRELIABLE;
        } break;
    }

    // Create a packet
    ENetPacket* p = enet_packet_create(
        message->data, 
        message->size, 
        flags
    );

    // Send packet to peer
	enet_peer_send(peer, channel, p);

    // Flush packet
    enet_host_flush(enet->host);
}

GS_API_DECL void 
core_enet_server_broadcast(core_enet_t* enet, const core_network_message_t* message)
{
	if (!core_enet_server_num_clients(enet)) return;

    uint32_t flags = 0; 
    uint32_t channel = 0;
    switch (message->delivery)
    {
        case CORE_NETWORK_DELIVERY_RELIABLE:   
        {
            flags = ENET_PACKET_FLAG_RELIABLE;
            channel = CORE_NETWORK_CHANNEL_RELIABLE;
        } break;
        case CORE_NETWORK_DELIVERY_UNRELIABLE: 
        {
            flags = ENET_PACKET_FLAG_UNSEQUENCED;
            channel = CORE_NETWORK_CHANNEL_UNRELIABLE;
        } break;
    }

    // Create a packet
    ENetPacket* p = enet_packet_create(
        message->data,
        message->size,
        flags
    );

	// send the packet to the peer
	enet_host_broadcast(enet->host, channel, p);

	// flush / send the packet queue
	enet_host_flush(enet->host);
}

//===========[ Core_ENet_Client ]=========// 

GS_API_DECL void 
core_enet_client_destroy(core_enet_t* enet)
{
    if (core_network_is_connected(core_network_instance()))
    {
        core_enet_client_disconnect(enet);
    }
    enet_host_destroy(enet->host);
} 

GS_API_DECL void 
core_enet_client_disconnect(core_enet_t* enet)
{
	if (!enet->client.server) return;

    enet_peer_disconnect(enet->client.server, 0);

    ENetEvent evt;
    bool success = false;
    while (enet_host_service(enet->host, &evt, 0) > 0)
    {
        switch (evt.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Throw away package data
                enet_packet_destroy(evt.packet);
            } break;

            case ENET_EVENT_TYPE_DISCONNECT:
            { 
                // Disconnect successful
                success = true;
            } break;
        }
    }

    if (core_network_is_connected(core_network_instance()))
    {
        gs_log_warning("Core_Network_Host_Disconnect::Server not acknowledging disconnect. Forcing disconnect.");
        enet_peer_reset(enet->client.server);
    }

    enet->client.server = NULL;
}

GS_API_DECL bool 
core_enet_client_poll(core_enet_t* enet, core_network_event_t* out)
{
    ENetEvent evt;
    gs_byte_buffer_t* mdata = &enet->message_data; 

    while (enet_host_service(enet->host, &evt, 0) > 0)
    {
        uint32_t type = evt.type;
        switch (evt.type) 
        { 
            case ENET_EVENT_TYPE_RECEIVE:
            {
                ENetPacket* packet = evt.packet;

                out->type = CORE_NETWORK_MESSAGE_DATA;
                out->peer_id = CORE_NETWORK_SERVER_ID;
                out->data = mdata->data;
                out->size = packet->dataLength;

                // Store data into message data buffer
                gs_byte_buffer_write_bulk(mdata, packet->data, packet->dataLength);

                // Destroy packet after using
                enet_packet_destroy(evt.packet);

                return true;
            } break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                core_enet_client_disconnect(enet);
                out->type = CORE_NETWORK_MESSAGE_DISCONNECT;
                out->peer_id = CORE_NETWORK_SERVER_ID;
                return true;
            } break;
        }
    }

    return false;
}

GS_API_DECL void 
core_enet_client_send_message(core_enet_t* enet, const core_network_message_t* message)
{
    if (!enet->client.server) return;

    uint32_t flags = 0; 
    uint32_t channel = 0;
    switch (message->delivery)
    {
        case CORE_NETWORK_DELIVERY_RELIABLE:   
        {
            flags = ENET_PACKET_FLAG_RELIABLE;
            channel = CORE_NETWORK_CHANNEL_RELIABLE;
        } break;
        case CORE_NETWORK_DELIVERY_UNRELIABLE: 
        {
            flags = ENET_PACKET_FLAG_UNSEQUENCED;
            channel = CORE_NETWORK_CHANNEL_UNRELIABLE;
        } break;
    }

    // Create a packet
    ENetPacket* p = enet_packet_create(
        message->data, 
        message->size, 
        flags
    );

    // Send packet to server
    enet_peer_send(enet->client.server, channel, p);

    // Flush packet
    enet_host_flush(enet->host);
} 









































