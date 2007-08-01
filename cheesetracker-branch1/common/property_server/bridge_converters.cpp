#include "bridge_converters.h"

// Int
int BridgeConverterInt::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	Int_Property_Bridge* bridge = dynamic_cast<Int_Property_Bridge*>(p_bridge);
	if (!bridge) {

		ERROR("Invalid bridge type");
		return -1;
	};

	int dword;
	get_dword(p_buffer, (Uint32&)dword);

	bridge->set(dword);

	return 0;
};

int BridgeConverterInt::write(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	Int_Property_Bridge* bridge = dynamic_cast<Int_Property_Bridge*>(p_bridge);
	if (!bridge) {

		ERROR("Invalid bridge type");
		return -1;
	};
	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);
	set_dword(p_buffer, bridge->get());

	return size;
};

int BridgeConverterInt::get_size(Property_Bridge* p_bridge, const string& p_path) {

	return get_header_size(p_bridge, p_path) + 4;
};

// Float
int FloatBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	CAST_BRIDGE(Float_Property_Bridge);
	float val;
	get_float(p_buffer, val);

	bridge->set(val);
	return 4;

};

int FloatBridgeConverter::write(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	CAST_BRIDGE(Float_Property_Bridge);

	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);
	set_float(p_buffer, bridge->get());

	return size;
};

int FloatBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

        return get_header_size(p_bridge, p_path) + 4;
};

// String
int StringBridgeConverter::write(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	CAST_BRIDGE(String_Property_Bridge);

	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);

	set_string(p_buffer, bridge->get());

	return size;
};

int StringBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	CAST_BRIDGE(String_Property_Bridge);

	string val;
	if (get_string(p_buffer, val) == -1) {

		return -1;
	};
	bridge->set(val);

	return val.size();
};

int StringBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

    CAST_BRIDGE(String_Property_Bridge);
    return get_header_size(p_bridge, p_path) + 4 + bridge->get().size();
};

// bool
int BoolBridgeConverter::write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

    CAST_BRIDGE(Bool_Property_Bridge);
    int size = get_size(p_bridge, p_path);
    write_header(p_buffer, p_bridge, p_path, size);

    p_buffer.buffer[p_buffer.offset++] = (Uint8)bridge->get();

    return size;
};

int BoolBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

    CAST_BRIDGE(Bool_Property_Bridge);

    bridge->set(p_buffer.buffer[p_buffer.offset++] != 0);

    return 1;
};

int BoolBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

        return get_header_size(p_bridge, p_path) + 1;
};

// opetions
int OptionsBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	CAST_BRIDGE(Options_Property_Bridge);

	int dword;
	get_dword(p_buffer, (Uint32&)dword);

	bridge->set(dword);

	return 0;
};

int OptionsBridgeConverter::write(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	CAST_BRIDGE(Options_Property_Bridge);

	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);
	set_dword(p_buffer, bridge->get());

	return size;
};

int OptionsBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

	return get_header_size(p_bridge, p_path) + 4;
};

// int array
int IntListBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	CAST_BRIDGE(IntListPropertyBridge);

	int size;
	get_dword(p_buffer, (Uint32&)size);

	if (p_buffer.offset + (size * sizeof(float)) > p_buffer.size) {

		return -1;
	};

	for (int i=0; i<size; i++) {

		int val;
		get_dword(p_buffer, (Uint32&)val);
		bridge->set(i, val);
	};
	return 0;
};

int IntListBridgeConverter::write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	CAST_BRIDGE(IntListPropertyBridge);

	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);

	int arr_size = bridge->get_size();
	set_dword(p_buffer, arr_size);

	for (int i = 0; i<arr_size; i++) {

		set_dword(p_buffer, bridge->get(i));
	};

	return size;
};

int IntListBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

	CAST_BRIDGE(IntListPropertyBridge);
	return get_header_size(p_bridge, p_path) + 4 + (4 * bridge->get_size());
};


// float array
int FloatListBridgeConverter::read(BridgeConverter::BufferControl& p_buffer, Property_Bridge* p_bridge) {

	CAST_BRIDGE(FloatListPropertyBridge);

	Uint32 size;
	get_dword(p_buffer, size);

	if (p_buffer.offset + (size * sizeof(float)) > p_buffer.size) {

		return -1;
	};

	for (unsigned int i=0; i<size; i++) {

		float val;
		get_float(p_buffer, val);
		bridge->set(i, val);
	};
	return 0;
};

int FloatListBridgeConverter::write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	CAST_BRIDGE(FloatListPropertyBridge);

	int size = get_size(p_bridge, p_path);
	write_header(p_buffer, p_bridge, p_path, size);

	Uint32 arr_size = bridge->get_size();

	set_dword(p_buffer, arr_size);

	for (int i = 0; i<arr_size; i++) {

		set_float(p_buffer, bridge->get(i));
	};

	return size;
};

int FloatListBridgeConverter::get_size(Property_Bridge* p_bridge, const string& p_path) {

	CAST_BRIDGE(FloatListPropertyBridge);
	return get_header_size(p_bridge, p_path) + 4 + (4 * bridge->get_size());
};



