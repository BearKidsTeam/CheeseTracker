#ifndef BRIDGE_CONVERTERS_H
#define BRIDGE_CONVERTERS_H

#include "bridge_converter.h"

class BridgeConverterInt : public BridgeConverter {

public:

	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class FloatBridgeConverter : public BridgeConverter {

public:

	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class StringBridgeConverter : public BridgeConverter {
public:

	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class BoolBridgeConverter : public BridgeConverter {
public:

	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class OptionsBridgeConverter : public BridgeConverter {
public:

	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class IntListBridgeConverter : public BridgeConverter {
public:
	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

class FloatListBridgeConverter : public BridgeConverter {

public:
	int read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge);
	int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	int get_size(Property_Bridge* p_bridge, const string& p_path);
};

#endif
