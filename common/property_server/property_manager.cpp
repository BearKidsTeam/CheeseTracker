#include "property_manager.h"

#define MAX_STR_SIZE 255

PropertyManager::BridgeList::iterator PropertyManager::register_property_bridge(string p_path, Property_Bridge* p_bridge) {

	if (p_path.size() > MAX_STR_SIZE || p_bridge->get_short_name().size() > MAX_STR_SIZE) {

		ERROR("Name or path for "<< p_path << "/" << p_bridge->get_short_name() << " is longer than "<< MAX_STR_SIZE << ", Aborting");
		return bridge_npos;
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
	} else {

		WARN("Trying to register an alredy registered bridge: "<< p_path << "/" << p_bridge->get_short_name()<< ", aborting.");
		return bridge_npos;
	};

	return J;
};

void PropertyManager::register_bridge_converter(string p_type, BridgeConverter* p_converter) {

	bridge_converter_list[p_type] = p_converter;
};

Property_Bridge* PropertyManager::get_bridge(string p_path, string p_name) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I != paths.end()) {
		BridgeList::iterator J = I->second.bridges.find(p_name);
		if (J != I->second.bridges.end()) {

			return J->second.bridge;
		};
	};

	return NULL;
};

PropertyManager::PropertyPath* PropertyManager::get_path(string p_path) {

	PropertyPathList::iterator I = paths.find(p_path);
	if (I != paths.end()) {

		return &I->second;
	};

	return NULL;
};

int PropertyManager::save_bridge(BridgeConverter::BufferControl& p_buffer, Bridge& p_bridge) { return 0; };
int PropertyManager::load_bridge(BridgeConverter::BufferControl& p_buffer) { return 0; };

int PropertyManager::save_path(BridgeConverter::BufferControl& p_buffer, string p_path) { return 0; };
int PropertyManager::save_path(BridgeConverter::BufferControl& p_buffer, PropertyPath& p_path) { return 0; };

int PropertyManager::save_all(BridgeConverter::BufferControl& p_buffer) { return 0; };

int PropertyManager::load_patch(BridgeConverter::BufferControl& p_buffer) { return 0; };


PropertyManager::PropertyManager() {

};

PropertyManager::~PropertyManager() {

};
