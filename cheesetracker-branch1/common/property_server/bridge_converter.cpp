#include <cstring>
#include "bridge_converter.h"

int BridgeConverter::get_short_string(BufferControl& p_buffer, string& p_string) {

	char strsize;

	strsize = p_buffer.buffer[p_buffer.offset++];
	if (strsize > p_buffer.size - p_buffer.offset) {

		return -1;
	};

	p_string.assign((const char*)p_buffer.buffer + p_buffer.offset, strsize);
	p_buffer.offset += strsize;

	return strsize;
};

Uint32 BridgeConverter::get_dword(BufferControl& p_buffer) {

	Uint32 t;
	get_dword(p_buffer, t);
	return t;
};

int BridgeConverter::get_dword(BufferControl& p_buffer, Uint32& p_dword) {

	int i = 4;
	while (i--) {

		p_dword <<= 8;
		p_dword |= p_buffer.buffer[p_buffer.offset + i];
	};
	p_buffer.offset += 4;
	return 4;
};

int BridgeConverter::set_dword(BufferControl& p_buffer, Uint32 p_dword) {

	int i = 0;
	while (i < 4) {

		Uint8 byte = p_dword & 0xFF;
		p_buffer.buffer[p_buffer.offset++] = byte;
		p_dword>>=8;
		i++;
	};
	return 4;
};


int BridgeConverter::get_string(BufferControl& p_buffer, string& p_string) {

	Uint32 size;
	get_dword(p_buffer, size);
	if (size > p_buffer.size - p_buffer.offset) {

		return -1;
	};

	p_string.assign((const char*)p_buffer.buffer + p_buffer.offset, size);
	p_buffer.offset += size;

	return size;
};

int BridgeConverter::get_float(BufferControl& p_buffer, float& p_float) {

	return get_dword(p_buffer, *((Uint32*)&p_float));
};

int BridgeConverter::set_short_string(BufferControl& p_buffer, const string& p_string) {

	if (p_string.size() > 255) {
		return -1;
	};
	if (p_buffer.offset + p_string.size() + 1 > p_buffer.size) {

		return -1;
	};
	p_buffer.buffer[p_buffer.offset++] = (Uint8)p_string.size();
	memcpy(p_buffer.buffer + p_buffer.offset, &p_string[0], p_string.size());
	p_buffer.offset += p_string.size();

	return p_string.size();
};
int BridgeConverter::set_string(BufferControl& p_buffer, const string& p_string) {

	if (p_buffer.offset + p_string.size() + 4 > p_buffer.size) {

		return -1;
	};

	set_dword(p_buffer, p_string.size());
	memcpy(p_buffer.buffer + p_buffer.offset, &p_string[0], p_string.size());
	p_buffer.offset += p_string.size();

	return p_string.size();
};

int BridgeConverter::set_float(BufferControl& p_buffer, float p_float) {

	return set_dword(p_buffer, *((Uint32*)&p_float));
};

int BridgeConverter::write_header(BufferControl& p_buffer, Property_Bridge* p_bridge, const string& p_path, int p_size) {

	if (p_size > p_buffer.size - p_buffer.offset) {

		return -1;
	};
	set_dword(p_buffer, p_size);
	set_short_string(p_buffer, p_path);
	set_short_string(p_buffer, p_bridge->get_short_name());
	set_short_string(p_buffer, p_bridge->get_type());

	return 0;
};

int BridgeConverter::get_header_size(Property_Bridge* p_bridge, const string& p_path) {

	return 4 + p_path.size() +1 + p_bridge->get_short_name().size()+1 + p_bridge->get_type().size()+1;
};



int BridgeConverter::write(BufferControl& p_buffer, Property_Bridge* p_bridge, string p_path) {

	int size = get_size(p_bridge, p_path);

	if (write_header(p_buffer, p_bridge, p_path, size) == -1) {

		return -1;
	};

	int pete = write_internal(p_buffer, p_bridge, size);
	return pete?pete:size;
};

BridgeConverter::BridgeConverter() {};
BridgeConverter::~BridgeConverter() {};

