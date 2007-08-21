#ifndef PROPERTY_MANAGER_H
#define PROPERTY_MANAGER_H

#include "components/data/property_bridges.h"
#include "bridge_converter.h"

class PropertyManager {

public:
	struct Bridge {
		Property_Bridge* bridge;
		Uint32 last_modified;
		string path;
		Bridge(Property_Bridge* p_bridge, string p_path);
		Bridge(){};
	};
	typedef hash_map<string,Bridge,String_Hash> BridgeList;
	static BridgeList::iterator bridge_npos;

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

	PropertyPathList paths;
	BridgeConverterList bridge_converter_list;

public:
	Property_Bridge* get_bridge(string p_path, string p_name);
	PropertyPath* get_path(string p_path);

	BridgeList::iterator register_property_bridge(string path, Property_Bridge* p_bridge);
	void register_bridge_converter(string p_type, BridgeConverter* p_converter);

	int save_bridge(BridgeConverter::BufferControl& p_buffer, Bridge& p_bridge);
	int load_bridge(BridgeConverter::BufferControl& p_buffer);

	int save_path(BridgeConverter::BufferControl& p_buffer, string p_path);
	int save_path(BridgeConverter::BufferControl& p_buffer, PropertyPath& p_path);

	int save_all(BridgeConverter::BufferControl& p_buffer);

	int load_patch(BridgeConverter::BufferControl& p_buffer);

	PropertyManager();
	~PropertyManager();
};

#endif
