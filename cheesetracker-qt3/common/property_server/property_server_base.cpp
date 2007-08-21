#include "property_server_base.h"

#define MAX_STR_SIZE 255

PropertyServer* PropertyServer::instance = NULL;

PropertyServer* PropertyServer::get_singleton_instance() {

	return instance;
};


void PropertyServer::property_changed(string p_path, BridgeList::iterator bridge_iterator) {

	// do something
	if (p_path == current_bridge.path && bridge_iterator->second.bridge->get_short_name() == current_bridge.name) {

		return;
	};

	property_queue_internal(p_path, bridge_iterator, 0, 0);

	paths[p_path].signal.emit(p_path);
};

void PropertyServer::send_bridge(const string& p_path, Property_Bridge* p_bridge, mcmd_peer_t p_recipient) {

	if (comm.port == -1) {

		return;
	};

	mcmd_t* msg = (mcmd_t*)comm.snd_buffer;
	msg->cmdtype = MCMD_BRIDGE;
	BridgeConverter::BufferControl buffer;

	buffer.buffer = (Uint8*)comm.snd_buffer;
	buffer.size = sizeof(comm.snd_buffer);
	buffer.offset = sizeof(mcmd_t);

	msg->datalength = pack_bridge(p_bridge, p_path, buffer);

	if (msg->datalength >= 0) {
		send_data_internal(msg, p_recipient);
		//mcmd_send(&comm.info, msg, sizeof(mcmd_t) + msg->datalength, p_recipient);
	};
};

void PropertyServer::register_notification(string p_path, SigC::Slot1<void,string>& p_slot) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I != paths.end()) {

		I->second.signal.connect(p_slot);
	} else {
		WARN("Invalid path " << p_path);
		return;
	};
};

void PropertyServer::register_property_bridge(string p_path, Property_Bridge* p_bridge) {

	if (p_path.size() > MAX_STR_SIZE || p_bridge->get_short_name().size() > MAX_STR_SIZE) {

		ERROR("Name or path for "<< p_path << "/" << p_bridge->get_short_name() << " is longer than "<< MAX_STR_SIZE << ", Aborting");
		return;
	};
	PropertyPathList::iterator I = paths.find(p_path);
	if (I == paths.end()) {

		paths[p_path] = PropertyPath();
		I = paths.find(p_path);
	};

	BridgeList::iterator J = I->second.bridges.find(p_bridge->get_short_name());
	if (J == I->second.bridges.end()) {

		Bridge aux(p_bridge, p_path);
		I->second.bridges[p_bridge->get_short_name()] = aux;
		J = I->second.bridges.find(p_bridge->get_short_name());
		J->second.connection = p_bridge->changed_signal.connect(SigC::bind<string,BridgeList::iterator>(SigC::slot<void>(*this, &PropertyServer::property_changed), p_path, J));
	} else {

		WARN("Trying to register an alredy registered bridge: "<< p_path << "/" << p_bridge->get_short_name()<< ", aborting.");
		return;
	};
};

void PropertyServer::unregister_property_bridge(string p_path, string p_name) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I == paths.end()) {
		ERROR("Trying to unregister non-existent bridge "<<p_path<<"/"<<p_name);
		return;
	};

	BridgeList::iterator J = I->second.bridges.find(p_name);
	if (J == I->second.bridges.end()) {

		ERROR("Trying to unregister non-existent bridge "<<p_path<<"/"<<p_name);
		return;
	};

	clear_bridge(I, J);
};

void PropertyServer::unregister_path(string p_path) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I == paths.end()) {
		ERROR("Trying to unregister non-existent path "<<p_path);
		return;
	};

	BridgeList::iterator J = I->second.bridges.begin();
	while (J != I->second.bridges.end()) {

		clear_bridge(I, J);
		J++;
	};
};

void PropertyServer::clear_bridge(PropertyPathList::iterator &p_path, BridgeList::iterator &p_bridge) {

	p_bridge->second.connection.disconnect();
	p_path->second.bridges.erase(p_bridge);
};

Property_Bridge* PropertyServer::get_bridge(string p_path, string p_name) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I != paths.end()) {
		BridgeList::iterator J = I->second.bridges.find(p_name);
		if (J != I->second.bridges.end()) {

			return J->second.bridge;
		};
	};

	return NULL;
};

int PropertyServer::init(int p_port) { ///< returns true on error

	return connect_internal(p_port);
};

void PropertyServer::poll() {

	if (comm.port < 0) {

		WARN("Trying to poll without connection.");
		return;
	};
	int res = 1;

	/*
	while (res > 0) {

		//res=mcmd_receive_nowait(&comm.info, comm.rcv_buffer, MSG_MAXSIZE);
		res=mcmd_receive_nowait(&comm.info, comm.rcv_buffer, MSG_MAXSIZE);

		if (res > 0) {
			handle_message((mcmd_t*)comm.rcv_buffer);
		};
	};
	*/
        poll_internal();
};

void PropertyServer::handle_message(mcmd_t* p_msg) {

	std::cout << "got a message, type is " << p_msg->cmdtype << ", size is " << p_msg->datalength << ", sender is " << p_msg->reply_mtype << ", mtype is " << p_msg->mtype << std::endl;
	switch (p_msg->cmdtype) { // "p_msg+1" means the end of the mcmd_t struct

		case MCMD_ACK:
			// someging
			break;
		case MCMD_GET_BRIDGE:
			msg_get_bridge((Uint8*)(p_msg+1), p_msg->reply_mtype, p_msg->datalength);
			break;
		case MCMD_GET_PATH:
			msg_get_path((Uint8*)(p_msg+1), p_msg->reply_mtype, p_msg->datalength);
			break;
		case MCMD_BRIDGE:
			msg_set_bridge((Uint8*)(p_msg+1), p_msg->reply_mtype, p_msg->datalength);
			break;
		case MCMD_BRIDGE_GROUP:
			msg_set_bridges((Uint8*)(p_msg+1), p_msg->reply_mtype, p_msg->datalength);
			break;
		case MCMD_PING: {

			mcmd_pong_t pong, *ping;
			ping = (mcmd_ping_t*)p_msg;
			set_mcmd_pong(&pong, ntohl(ping->timestamp));
			send_data_internal(&pong.cmdheader, comm.port);
			//mcmd_send(&comm.info, &pong, sizeof(mcmd_pong_t), comm.port);
		}; break;
		default:
                        handle_message_internal(p_msg);
			break;
	};
};

bool PropertyServer::get_bridge_name(BridgeConverter::BufferControl& buf, string& path, string& name, string& type) {

	if (BridgeConverter::get_short_string(buf, path) < 0) {
		WARN("Error unpacking bridge path");
		return true;
	};

	if (BridgeConverter::get_short_string(buf, name) < 0) {
		WARN("Error unpacking bridge name");
		return true;
	};

	if (BridgeConverter::get_short_string(buf, type) < 0) {
		WARN("Error unpacking bridge type");
		return true;
	};
	return false;
};

void PropertyServer::msg_get_path(Uint8* p_buf, mcmd_peer_t p_recipient, int msg_size) {

	BridgeConverter::BufferControl buf;
	buf.buffer = p_buf;
	BridgeConverter::get_dword(buf, buf.size);
	if (buf.size > msg_size) {

		ERROR("Invalid message size!");
		return;
	};

	string aux_path;
	if (BridgeConverter::get_short_string(buf, aux_path) < 0) {

		WARN("Error unpacking path");
		return;
	};

	PropertyPathList::iterator I = paths.find(aux_path);
	if (I != paths.end()) {

		BridgeList::iterator J = I->second.bridges.begin();
		while (J != I->second.bridges.end()) {

			property_queue_internal(aux_path, J, 0, p_recipient);

			J++;
		};
	} else {
  		WARN("Requesting non-existing path");
		return;
	};
};

void PropertyServer::request_bridge(const string &p_path, const string &p_name, mcmd_peer_t p_client) {

	mcmd_t* msg = (mcmd_t*)comm.snd_buffer;
	msg->cmdtype = MCMD_GET_BRIDGE;
	msg->datalength = p_path.size() +1 + p_name.size() +1 + 1 + 4;
	// path size + name size + 1 byte for empty type + total size

	BridgeConverter::BufferControl buf;
	buf.buffer = comm.snd_buffer;
	buf.size = sizeof(comm.snd_buffer);
	buf.offset = sizeof(mcmd_t);

	BridgeConverter::set_dword(buf, msg->datalength);
	BridgeConverter::set_short_string(buf, p_path);
	BridgeConverter::set_short_string(buf, p_name);
	buf.buffer[buf.offset++] = 0;

	send_data_internal(msg, p_client<0?comm.port:p_client);
	//mcmd_send(&comm.info, msg, sizeof(mcmd_t) + msg->datalength, p_client<0?comm.port:p_client);
};

void PropertyServer::request_path(const string &p_path, mcmd_peer_t p_client) {

	mcmd_t* msg = (mcmd_t*)comm.snd_buffer;
	msg->cmdtype = MCMD_GET_PATH;
	msg->datalength = p_path.size() +1 + 4;
	// path size + total size

	BridgeConverter::BufferControl buf;
	buf.buffer = comm.snd_buffer;
	buf.size = sizeof(comm.snd_buffer);
	buf.offset = sizeof(mcmd_t);

	BridgeConverter::set_dword(buf, msg->datalength);
	BridgeConverter::set_short_string(buf, p_path);

	send_data_internal(msg,p_client<0?comm.port:p_client);
	//mcmd_send(&comm.info, msg, sizeof(mcmd_t) + msg->datalength, p_client<0?comm.port:p_client);
};

void PropertyServer::msg_get_bridge(Uint8* p_buf, mcmd_peer_t p_recipient, int msg_size) {

	BridgeConverter::BufferControl buf;
	buf.buffer = p_buf;

	BridgeConverter::get_dword(buf, buf.size);
	if (buf.size > msg_size) {

		ERROR("Invalid message size!");
		return;
	};

	string aux_type;
	if (get_bridge_name(buf, current_bridge.path, current_bridge.name, aux_type)) {

		current_bridge.clear();
		return;
	};

	// queue bridge to send
	if (get_bridge(current_bridge.path, current_bridge.name)) {

		// queue for send
		property_queue_internal(current_bridge.path, paths[current_bridge.path].bridges.find(current_bridge.name), comm.port, p_recipient);
	} else {

		ERROR("Requested invalid bridge from peer: "<<current_bridge.path <<"/"<<current_bridge.name);
	};
	current_bridge.clear();
};

void PropertyServer::msg_set_bridges(Uint8* p_buf, mcmd_peer_t p_sender, int msg_size) {

	int bsize;

	BridgeConverter::BufferControl buf;
	buf.buffer = p_buf;

	BridgeConverter::get_dword(buf, buf.size);
	if (buf.size > msg_size) {

		ERROR("Invalid message size!");
		return;
	};

	while (buf.offset < buf.size) {

		bsize = msg_set_bridge(buf.buffer + buf.offset, 0, msg_size - buf.offset);
		if (bsize < 0) {

			ERROR("Error setting bridges, aborting");
			return;
		} else {

			buf.offset += bsize;
		};
	};

	if (p_sender > 0) {
		send_ack(p_sender);
	};
};

int PropertyServer::msg_set_bridge(Uint8* p_buf, mcmd_peer_t p_sender, int msg_size) {

	BridgeConverter::BufferControl buf;
	buf.buffer = p_buf;

	BridgeConverter::get_dword(buf, buf.size);
	if (buf.size > msg_size) {

		ERROR("Invalid message size! "<<buf.size<<"/"<<msg_size);
		return -1;
	};

	string aux_type;
	if (get_bridge_name(buf, current_bridge.path, current_bridge.name, aux_type)) {

		current_bridge.clear();
		return -1;
	};

	Property_Bridge* bridge = get_bridge(current_bridge.path, current_bridge.name);
	if (bridge) {
		if (aux_type.size() && (bridge->get_type() != aux_type)) {

			ERROR("Bridge type mismatch for "<< current_bridge.path << "/" << current_bridge.name << "!!!");
			current_bridge.clear();
			return -1;
		};

		unpack_bridge(bridge, buf);

		if (p_sender >= 0) {

			// it's from a client
			property_queue_internal(current_bridge.path, paths[current_bridge.path].bridges.find(current_bridge.name), p_sender, 0);
			paths[current_bridge.path].signal.emit(current_bridge.path);
		};

		current_bridge.clear();
	};

	if (p_sender >= 0) {
		send_ack(p_sender);
	};

	return buf.size;
};

void PropertyServer::send_ack(int p_recipient) {

	return;
	mcmd_ack_t* ack = (mcmd_ack_t*)comm.snd_buffer;
	set_mcmd_ack(ack,0);
	send_data_internal(&(ack->cmdheader), p_recipient);
	//mcmd_send(&comm.info, ack, sizeof(mcmd_ack_t), p_recipient);
};

void PropertyServer::unpack_bridge(Property_Bridge* p_bridge, BridgeConverter::BufferControl& p_buffer) {

	if (bridge_converter_list.find(p_bridge->get_type()) == bridge_converter_list.end()) {

		ERROR("Bridge type unknown: "<<p_bridge->get_type() <<", aborting unpack.");
		return;
	};

	bridge_converter_list[p_bridge->get_type()]->read(p_buffer, p_bridge);
};

int PropertyServer::pack_bridge(Property_Bridge* p_bridge, const string& p_path, BridgeConverter::BufferControl& p_buffer) {

	if (bridge_converter_list.find(p_bridge->get_type()) == bridge_converter_list.end()) {

		ERROR("Bridge type unknown: "<<p_bridge->get_type() <<", aborting pack.");
		return -1;
	};

	return bridge_converter_list[p_bridge->get_type()]->write(p_buffer, p_bridge, p_path);
};

void PropertyServer::register_bridge_converter(string p_type, BridgeConverter* p_converter) {

	bridge_converter_list[p_type] = p_converter;
	// the objects are owned
};

void PropertyServer::clear_bridge_converter_list() {

	BridgeConverterList::iterator I = bridge_converter_list.begin();

	while (I != bridge_converter_list.end()) {

		delete I->second;
	};
	bridge_converter_list.clear();
};


void PropertyServer::save_path(string p_path, string p_filename) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I == paths.end()) {

  		WARN("Requesting non-existing path");
		return;
	};

	FILE* f = fopen(p_filename.c_str(), "wb");
	if (!f) {

		WARN("Unable to open "<<p_filename<<" for write.");
		return;
	};

	// the buffer
	BridgeConverter::BufferControl buffer;
	buffer.size = 1024*10;			// 10KB
	buffer.buffer = (Uint8*)malloc(buffer.size);
	buffer.offset = 0;

	BridgeList::iterator J = I->second.bridges.begin();
	while (J != I->second.bridges.end()) {

		//printf("saving bridge %s/%s at %i\n", J->second.path.c_str(), J->second.bridge->get_short_name().c_str(), ftell(f));
		if (pack_bridge(J->second.bridge, J->second.path, buffer) < 0) {

			ERROR("Error packing bridge "<<J->second.bridge->get_short_name());
			free(buffer.buffer);
			fclose(f);
			return;
		};

		if (fwrite(buffer.buffer, 1, buffer.offset, f) < buffer.offset) {

			WARN("Error writing to "<<p_filename);
			free(buffer.buffer);
			fclose(f);
			return;
		};
		buffer.offset = 0;

		J++;
	};

	fclose(f);

	free(buffer.buffer);
};

void PropertyServer::load_path(string p_filename) {

	FILE* f = fopen(p_filename.c_str(), "rb");
	if (!f) {
		ERROR("Error opening filename "<<p_filename<<" for read.");
		return;
	};

	fseek(f, 0, SEEK_END);

	BridgeConverter::BufferControl buffer;
	buffer.size = ftell(f);
	buffer.buffer = (Uint8*)malloc(buffer.size);
	buffer.offset = 0;

	fseek(f, 0, SEEK_SET);

	if (fread(buffer.buffer, 1, buffer.size, f) < buffer.size) {

		ERROR("Error reading filename "<<p_filename);
		free(buffer.buffer);
		return;
	};
	fclose(f);

	while (buffer.offset < buffer.size) {

		Uint32 size, size_offset;
		size_offset = buffer.offset;
		BridgeConverter::get_dword(buffer, size);

		if (!size || ((buffer.offset -4) + size > buffer.size)) {

			ERROR("Invalid file format? Aborting.");
			free(buffer.buffer);
			return;
		};

		string aux_type;
		if (get_bridge_name(buffer, current_bridge.path, current_bridge.name, aux_type)) {

			current_bridge.clear();
			free(buffer.buffer);
			return;
		};
		//printf("loading bridge %s/%s\n", current_bridge.path.c_str(), current_bridge.name.c_str());

		Property_Bridge* bridge = get_bridge(current_bridge.path, current_bridge.name);
		if (bridge) {
			unpack_bridge(bridge, buffer);
		} else {

			buffer.offset = size_offset + size;
			WARN("Unexistent bridge "<<current_bridge.path<<"/"<<current_bridge.name<<", ignoring");
		};
		if (buffer.offset < size_offset + size) {

			buffer.offset = size_offset + size;
			WARN("Error while loading bridge "<<current_bridge.path<<"/"<<current_bridge.name<<", ignoring");
		};

		current_bridge.clear();
	};

	free(buffer.buffer);
};


PropertyServer::Bridge::Bridge(Property_Bridge* p_bridge, string p_path) {
	bridge = p_bridge;
	last_modified = 0;
	path = p_path;
};


PropertyServer::PropertyServer() {

	if (instance) {

		ERROR("Property Server alredy initialized!");
		return;
	};
	instance = this;
	comm.port = -1;
	current_bridge.clear();
};

PropertyServer::~PropertyServer() {

	clear_bridge_converter_list();
};

