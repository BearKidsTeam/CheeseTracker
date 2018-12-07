#ifndef BRIDGE_CONVERTER_H
#define BRIDGE_CONVERTER_H

#include "components/data/property_bridges.h"
#include "string_hash.h"

#define CAST_BRIDGE(t)   \
	t* bridge = dynamic_cast<t*>(p_bridge);\
	if (!bridge) {\
		ERROR("Invalid bridge type");\
		return -1;\
	};

class BridgeConverter {

public:
	struct BufferControl {

		Uint8* buffer;
		Uint32 size;
		Uint32 offset;
		BufferControl() {

			buffer = NULL;
			size = 0;
			offset = 0;
		};
	};

	static int get_short_string(BufferControl& p_buffer, string& p_string);
	static int get_dword(BufferControl& p_buffer, Uint32& p_dword);
	static int get_float(BufferControl& p_buffer, float& p_float);
	static int get_string(BufferControl& p_buffer, string& p_string);

	static Uint32 get_dword(BufferControl& p_buffer);

	static int set_short_string(BufferControl& p_buffer, const string& p_string);
	static int set_string(BufferControl& p_buffer, const string& p_string);
	static int set_dword(BufferControl& p_buffer, Uint32 p_dword);
	static int set_float(BufferControl& p_buffer, float p_float);

	static int write_header(BufferControl& p_buffer, Property_Bridge* p_bridge, const string& path, int p_size);
	static int get_header_size(Property_Bridge* p_bridge, const string& p_path);
protected:

	BridgeConverter();

	virtual int write_internal(BufferControl& p_buffer, Property_Bridge* p_bridge, int p_size) { return 0; };

public:

	virtual int read(BufferControl& p_buffer, Property_Bridge* p_bridge)=0;
	virtual int write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path);
	virtual int get_size(Property_Bridge* p_bridge, const string& p_path)=0;

	virtual ~BridgeConverter();
};

#endif
