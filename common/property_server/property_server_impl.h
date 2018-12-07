#ifndef PROPERTY_SERVER_H
#define PROPERTY_SERVER_H

#include "property_server_base.h"

class PropertyServerMaster : public PropertyServer {

	list<BridgeList::iterator> bridge_queue;
	void property_changed_internal(string p_path, BridgeList::iterator bridge_iterator)=0;

	PropertyServerMaster();
};

class PropertyServerSlave : public PropertyServer {

	void property_changed_internal(string p_path, BridgeList::iterator bridge_iterator)=0;
};


#endif
