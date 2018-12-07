#ifndef PROPERTY_SERVER_BASE_H
#define PROPERTY_SERVER_BASE_H

#include "components/data/property_bridges.h"
#include "bridge_converter.h"
#include <sigc++/sigc++.h>

#include "mremotecmd.h"

typedef long int mcmd_peer_t;

class PropertyServer : public SigC::Object {
public:
	struct Bridge {
		Property_Bridge* bridge;
		Uint32 last_modified;
		string path;
		SigC::Connection connection;
		Bridge(Property_Bridge* p_bridge, string p_path);
		Bridge(){};
	};
	typedef hash_map<string,Bridge,String_Hash> BridgeList;

	struct PropertyPath {
		SigC::Signal1<void,string> signal;
		BridgeList bridges;
		PropertyPath& operator=(const PropertyPath& rv) {
			bridges = rv.bridges;
			return *this;
		};
	};
	typedef hash_map<string,PropertyPath,String_Hash> PropertyPathList;

	typedef hash_map<string,BridgeConverter*,String_Hash> BridgeConverterList;

private:

	struct {
		string name;
		string path;
		void clear() {
			name = "";
			path = "";
		};
	} current_bridge;

	static PropertyServer* instance;

	BridgeConverterList bridge_converter_list;
	PropertyPathList paths;

	void property_changed(string p_path, BridgeList::iterator bridge_iterator);

	void unpack_bridge(Property_Bridge* p_bridge, BridgeConverter::BufferControl& p_buffer);
	int pack_bridge(Property_Bridge* p_bridge, const string& p_path, BridgeConverter::BufferControl& p_buffer);
	void clear_bridge_converter_list();

	Property_Bridge* get_bridge(string p_path, string p_name);

	bool get_bridge_name(BridgeConverter::BufferControl& buf, string& path, string& name, string& type);
	void clear_bridge(PropertyPathList::iterator &p_path, BridgeList::iterator &p_bridge);

	void msg_get_bridge(Uint8* p_buf, mcmd_peer_t p_recipient, int msg_size);
	void msg_get_path(Uint8* p_buf, mcmd_peer_t p_recipient, int msg_size);
	void msg_set_bridges(Uint8* p_buf, mcmd_peer_t sender, int msg_size);
	int msg_set_bridge(Uint8* p_buf, mcmd_peer_t sender, int msg_size);

protected:

	void handle_message(mcmd_t* p_msg);

	void send_ack(int p_recipient);

	struct {
		mcmd_info_t info;
		mcmd_peer_t port;
		Uint8 rcv_buffer[MSG_MAXSIZE];
		Uint8 snd_buffer[MSG_MAXSIZE];
	} comm;

	virtual void property_queue_internal(const string& p_path, BridgeList::iterator bridge_iterator, mcmd_peer_t p_sender, mcmd_peer_t p_recipient)=0;
	virtual int connect_internal(mcmd_peer_t p_port)=0;

	virtual void handle_message_internal(mcmd_t* p_msg) {};
	virtual void poll_internal()=0;

	virtual int send_data_internal(mcmd_t* p_msg, int p_recipient)=0;

	void send_bridge(const string& p_path, Property_Bridge* p_bridge, mcmd_peer_t p_connection);

public:

	/**** temp ****/
	void save_path(string p_path, string p_filename);
	void load_path(string p_filename);
	/** end temp **/

	int init(int p_port); ///< returns true on error

	static PropertyServer* get_singleton_instance();

	void request_bridge(const string &p_path, const string &p_name, mcmd_peer_t p_client = -1);
	void request_path(const string &p_path, mcmd_peer_t p_client = -1);

	void register_property_bridge(string path, Property_Bridge* p_bridge);
	void register_bridge_converter(string p_type, BridgeConverter* p_converter);

	void unregister_property_bridge(string p_path, string p_name);
	void unregister_path(string p_path);

	void register_notification(string p_path, SigC::Slot1<void,string>& p_slot);

	void poll();

	PropertyServer();
	virtual ~PropertyServer();
};

static void hton_mcmd(mcmd_t& p_cmd) {

	p_cmd.mtype = htonl(p_cmd.mtype);
	p_cmd.reply_mtype = htonl(p_cmd.reply_mtype);
	p_cmd.cmdtype = htons(p_cmd.cmdtype);
	p_cmd.datalength = htons(p_cmd.datalength);
};

static void ntoh_mcmd(mcmd_t& p_cmd) {

	p_cmd.mtype = ntohl(p_cmd.mtype);
	p_cmd.reply_mtype = ntohl(p_cmd.reply_mtype);
	p_cmd.cmdtype = ntohs(p_cmd.cmdtype);
	p_cmd.datalength = ntohs(p_cmd.datalength);
};

#endif


