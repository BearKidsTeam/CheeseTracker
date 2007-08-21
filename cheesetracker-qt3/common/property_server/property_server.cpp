#include "property_server.h"

// slave

void PropertyServerSlave::property_queue_internal(const string &p_path, BridgeList::iterator bridge_iterator,mcmd_peer_t p_sender, mcmd_peer_t p_recipient) {

	send_bridge(p_path, bridge_iterator->second.bridge, comm.port);
};

int PropertyServerSlave::connect_internal(mcmd_peer_t p_port) {

	bool res;

	client->connect_to_host("localhost", p_port);
	res = client->is_connected();
	//res = mcmd_open_client_port(p_port, &comm.info);
	if (res) {

		comm.port = p_port;

		mcmd_t msg;
		msg.cmdtype = MCMD_REGISTER;
		send_data_internal(&msg, 0);
		//mcmd_send(&comm.info, &msg, sizeof(msg), comm.port);
	};
	return res?0:-1;
};

int PropertyServerSlave::send_data_internal(mcmd_t* p_msg, int p_recipient) {

	if (!client->is_connected()) {

		ERROR("Trying to send data while not connected!");
		return -1;
	};
	int size = sizeof(mcmd_t) + p_msg->datalength;
	hton_mcmd(*p_msg);
	client->send_data((Uint8*)p_msg, size);

	return 0;
};

void PropertyServerSlave::got_message_signal(Uint8* p_buf, int p_size) {

	mcmd_t* msg = (mcmd_t*)p_buf;
	ntoh_mcmd(*msg);
	msg->reply_mtype = -1;

	handle_message(msg);
};

PropertyServerSlave::PropertyServerSlave() {

	client = TcpClient::create_client();
	client->deliver_event.connect(SigC::slot<void,Uint8*,int>(*this,&PropertyServerSlave::got_message_signal));
};

PropertyServerSlave::~PropertyServerSlave() {

	if (comm.port != -1) {

		mcmd_t msg;
		msg.cmdtype = MCMD_UNREGISTER;
		send_data_internal(&msg, 0);
		//mcmd_send(&comm.info, &msg, sizeof(msg), comm.port);
	};

	delete client;
};

// master

void PropertyServerMaster::handle_message_internal(mcmd_t* p_msg) {

	switch(p_msg->cmdtype) {

		case MCMD_PONG: {

			mcmd_pong_t* pong = (mcmd_pong_t*)p_msg;
			for (int i=0; i<MAX_CLIENTS; i++) {

				if (!clients[i].id) continue;
				if (p_msg->reply_mtype == clients[i].id && ntohl(pong->timestamp) == clients[i].pending_ping) {

					// off the hook
					clients[i].pending_ping = 0;
					break;
				};
			};
		}; break;
		case MCMD_REGISTER: {

			send_ack(p_msg->reply_mtype);
			if (get_client_slot(p_msg->reply_mtype) >= 0) {

				WARN("Trying to register an alredy registered client "<<p_msg->reply_mtype<<std::endl);
				break;
			};
			int i = get_first_client_slot();
			if (i < 0) {

				// undefined behaviour.. maybe kill the client?
				break;
			};
			clients[i].id = p_msg->reply_mtype;
			clients[i].pending_ping = 0;
			printf("************** registering client %ld\n", clients[i].id);
		}; break;
		case MCMD_UNREGISTER: {

			send_ack(p_msg->reply_mtype);
			int i = get_client_slot(p_msg->reply_mtype);
			if (i < 0) {
				WARN("Trying to unregister a non-existent client "<<p_msg->reply_mtype<<std::endl);
				break;
			};
			remove_client(i);
		}; break;
        };
};

void PropertyServerMaster::poll_internal() {

	server->poll_connections();

	int elapsed_time = 1000; // this should be with a timer or something
	ping_ttl -= elapsed_time;
	if (ping_ttl < 0) {

		ping_clients();
	};
	send_queue();
};

void PropertyServerMaster::ping_clients() {
	printf("********** pinging clients\n");
	for (int i=0; i<MAX_CLIENTS; i++) {

		if (!clients[i].id) continue;
		if (clients[i].pending_ping) {
			WARN("Removing client "<<clients[i].id<<" from list: ping timeout");
			remove_client(i);
			continue;
		};
		mcmd_ping_t ping;
		int timestamp = 1234; // this should be a timestamp
		set_mcmd_ping(&ping, timestamp);
		send_data_internal(&(ping.cmdheader), clients[i].id);
		//mcmd_send(&comm.info, &ping, sizeof(mcmd_ping_t), clients[i].id);
		clients[i].pending_ping = timestamp;
	};
	ping_ttl = ping_timeout;
};

int PropertyServerMaster::get_client_slot(mcmd_peer_t p_client) {

	for (int i=0; i<MAX_CLIENTS; i++) {

		if (clients[i].id == p_client) {

			return i;
		};
	};
	return -1;
};

int PropertyServerMaster::get_first_client_slot() {

	for(int i=0; i<MAX_CLIENTS; i++) {

		if (!clients[i].id) {
			return i;
		};
	};
	ERROR("Error! maximum number of clients reached!");
	return -1;
};

void PropertyServerMaster::remove_client(int client) {

        clients[client].id = 0;
};

int PropertyServerMaster::connect_internal(mcmd_peer_t p_port) {

	bool res;
	res = server->start_server(p_port);
	if (res) {

		comm.port = p_port;
	};
	return res?0:-1;
};

void PropertyServerMaster::property_queue_internal(const string &p_path, BridgeList::iterator bridge_iterator, mcmd_peer_t p_sender,mcmd_peer_t p_recipient) {

	if (p_recipient || !bridge_iterator->second.last_modified ||
		bridge_iterator->second.last_modified != p_sender) {

		bridge_queue.push_back(QueuedBridge(p_path, bridge_iterator, p_recipient));
		if (!p_recipient) {
			bridge_iterator->second.last_modified = p_sender;
		};
	};
};

void PropertyServerMaster::send_queue() {

	list<QueuedBridge>::iterator I = bridge_queue.begin();

	while (I != bridge_queue.end()) {

		if (I->recipient) {
			send_bridge(I->iterator->second.path, I->iterator->second.bridge, I->recipient);
		} else {

			for (int i=0; i<MAX_CLIENTS; i++) {

				if (clients[i].id == I->iterator->second.last_modified) continue;
				send_bridge(I->iterator->second.path, I->iterator->second.bridge, clients[i].id);
			};

		};
		I->iterator->second.last_modified = 0;
		I++;
	};
	bridge_queue.clear();
};

void PropertyServerMaster::got_message_signal(Uint8* p_buf, int p_size, int p_sender) {

	mcmd_t* msg = (mcmd_t*)p_buf;
	ntoh_mcmd(*msg);
	msg->reply_mtype = p_sender;

	handle_message(msg);
};

int PropertyServerMaster::send_data_internal(mcmd_t* p_msg, int p_recipient) {

	if (comm.port != -1) {

		ERROR("Trying to send data while not connected!");
		return -1;
	};
	int size = sizeof(mcmd_t) + p_msg->datalength;
	hton_mcmd(*p_msg);
	server->send_data(p_recipient, (Uint8*)p_msg, size);

	return 0;
};

PropertyServerMaster::PropertyServerMaster() {

	// register timer thing
	ping_timeout = 50000; // I don't know
	ping_ttl = ping_timeout;
	int i = MAX_CLIENTS;
	while (i--) {

		remove_client(i);
	};

	server = new TcpServerPosix;
	server->deliver_event.connect(SigC::slot<void,Uint8*,int,int>(*this,&PropertyServerMaster::got_message_signal));
};

PropertyServerMaster::~PropertyServerMaster() {

	delete server;
};

