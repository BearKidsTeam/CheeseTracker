#ifndef PROPERTY_SERVER_H
#define PROPERTY_SERVER_H

#include "property_server_base.h"
#include <list>
using std::list;

#include "os/tcp_client.h"
#include "drivers/posix/tcp_server_posix.h"

#define MAX_CLIENTS 32

class PropertyServerMaster : public PropertyServer {

	TcpServerPosix* server;

	struct QueuedBridge {

		string path;
		PropertyServer::BridgeList::iterator iterator;

		mcmd_peer_t recipient;


		QueuedBridge(const string &p_path, PropertyServer::BridgeList::iterator& p_it, mcmd_peer_t p_port=0) {

			iterator = p_it;
			recipient = p_port;
			path = p_path;
		};
	};

	struct {

		mcmd_peer_t id;
                int pending_ping;
	} clients[MAX_CLIENTS];

	int ping_ttl;
        int ping_timeout;

	list<QueuedBridge> bridge_queue;
	void property_queue_internal(const string &p_path, BridgeList::iterator bridge_iterator, mcmd_peer_t p_sender, mcmd_peer_t p_recipient);

	void send_queue();

	int connect_internal(mcmd_peer_t p_port);

	void handle_message_internal(mcmd_t* p_msg);
	void poll_internal();
	int get_client_slot(mcmd_peer_t p_client);
	void ping_clients();
	int get_first_client_slot();
	void remove_client(int client);

	void got_message_signal(Uint8* p_buf, int p_size, int p_sender);

	int send_data_internal(mcmd_t* p_msg, int p_recipient);

public:
	PropertyServerMaster();
	~PropertyServerMaster();
};

class PropertyServerSlave : public PropertyServer {

	TcpClient* client;

	int connect_internal(mcmd_peer_t p_port);

	void property_queue_internal(const string &p_path, BridgeList::iterator bridge_iterator, mcmd_peer_t p_sender, mcmd_peer_t p_recipient);

	void got_message_signal(Uint8* p_buf, int p_size);
	int send_data_internal(mcmd_t* p_msg, int p_recipient);

public:
        PropertyServerSlave();
        ~PropertyServerSlave();
};


#endif
