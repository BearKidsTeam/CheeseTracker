#ifndef DDS_PACKER_H
#define DDS_PACKER_H

#include "typedefs.h"
#include <vector>
using std::vector;
#include "dds.h"
#include "pvector.h"

class DDSPacker {

public:

	typedef vector<Uint8> data;


private:
	static int add_to_pack(data& p_data, Uint32 p_index, DDS::Data_Node_List::const_iterator p_I);
	static int load_var(DDS* p_struct, const Uint8* p_data, Uint32 p_offset, Uint32 p_data_size);
	static int store_int_array(data& p_data, Uint32 p_index, vector<int> p_vector);
	static int store_string_array(data& p_data, Uint32 p_index, vector<string> p_list);
	static int store_float_array(data& p_data, Uint32 p_index, vector<float> p_list);

	static int store_dds_array(data& p_data, Uint32 p_index, VectorPointer<DDS> p_array);
	static int store_dds(data& p_data, Uint32 p_data_index, DDS* p_dds);

	static int get_dds(const Uint8* p_data, Uint32 p_data_size, Uint32 p_data_index, DDS &p_dds);


public:
	static void store_data(data& p_data, Uint32 p_index, const Uint8* p_data_ptr, Uint32 p_size);
	static void store_dword(data& p_data, Uint32 data_index, Uint32 p_dword);
	static Uint32 get_dword(const Uint8* p_data, Uint32 p_index);
	static Sint32 get_string(const Uint8* p_data, Uint32 p_data_size, Uint32 p_index, string &p_str);

	static void pack(DDS* p_struct, data& p_vector, Uint32 p_offset = 0);
	static bool unpack(DDS* p_struct, const Uint8* p_data, Uint32 p_size); ///< returns true on success

};
#endif
