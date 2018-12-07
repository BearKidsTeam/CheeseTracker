#include <cstring>
#include "dds_packer.h"


void DDSPacker::pack(DDS* p_struct, data& p_data, Uint32 p_offset) {

	DDS::Data_Node_List::const_iterator I, i_begin, i_end;
	i_begin = p_struct->get_iterator_begin();
	i_end = p_struct->get_iterator_end();
	int offset = p_offset;

	for (I=i_begin; I!=i_end; ++I) {
		offset += add_to_pack(p_data, offset, I);
	}
};

int DDSPacker::add_to_pack(data& p_data, Uint32 p_index, DDS::Data_Node_List::const_iterator p_I) {

	Uint32 data_index = p_index;

	//cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "ddsp: packing " << p_I->first.c_str() << " at " << p_index << " size is " << p_data.size() << endl;

	// name
	p_data.resize(p_data.size() + p_I->first.length()+4+1);
	store_dword(p_data, data_index, (Uint32)p_I->first.length());
	memcpy(&p_data[data_index + 4], p_I->first.c_str(), p_I->first.length());
	data_index += p_I->first.length()+4;

	// type
//	p_data.resize(p_data.size() + 1); // this is on the resize for name
	p_data[data_index] = (Uint8)p_I->second.type;
	data_index++;

	Uint32* aux_int;

	// [size] & data
	switch (p_I->second.type) {
		case DDS::T_STRING: {
			p_data.reserve(p_data.size()+4+p_I->second.string_data.length());
			p_data.resize(p_data.size() + 4);
			store_dword(p_data, data_index, (Uint32)p_I->second.string_data.length());
			data_index += 4;
			p_data.resize(p_data.size() + p_I->second.string_data.length());
			store_data(p_data, data_index, (Uint8*)p_I->second.string_data.c_str(), p_I->second.string_data.length());
			data_index += p_I->second.string_data.length();

			return (data_index - p_index);
		};
		case DDS::T_DATA: {
			p_data.reserve(p_data.size() + 4 + p_I->second.raw_data_size);
			p_data.resize(p_data.size() + 4);
			store_dword(p_data, data_index, (Uint32)p_I->second.raw_data_size);
			data_index += 4;
			p_data.resize(p_data.size() + p_I->second.raw_data_size);
			store_data(p_data, data_index, (Uint8*)p_I->second.data_ptr, p_I->second.raw_data_size);
			data_index += p_I->second.raw_data_size;
			return (data_index - p_index);
		};
		case DDS::T_INT: {
			aux_int = (Uint32*)&p_I->second.int_data;
		}; break;
		case DDS::T_FLOAT: {
			aux_int = (Uint32*)&p_I->second.float_data;
		}; break;

		case DDS::T_POINTER: {
			// warning
			aux_int = (Uint32*)&p_I->second.data_ptr;
		}; break;

		case DDS::T_INT_ARRAY: {
			data_index += store_int_array(p_data, data_index, p_I->second.int_array);
			return (data_index - p_index);
		}; break;

		case DDS::T_STRING_ARRAY: {
			data_index += store_string_array(p_data, data_index, p_I->second.string_array);
			return (data_index - p_index);
		}; break;

		case DDS::T_FLOAT_ARRAY: {

			data_index += store_float_array(p_data, data_index, p_I->second.float_array);
			return (data_index - p_index);
		}; break;

		case DDS::T_DDS: {


			data_index += store_dds(p_data, data_index, p_I->second.dynamic_data_struct);

			return (data_index - p_index);
		} break;

		case DDS::T_DDS_ARRAY: {

			data_index += store_dds_array(p_data, data_index, p_I->second.dds_array);

			return (data_index - p_index);

		} break;

		default : {

			//winnie the pooh
		} break;
	};

	p_data.resize(p_data.size() + 4);
	store_dword(p_data, data_index, *aux_int);

	data_index += 4;
	return (data_index - p_index);
};

int DDSPacker::store_dds_array(data& p_data, Uint32 p_index, VectorPointer<DDS> p_array) {

//	cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "store_dds_array: storing whole dds array at " << p_index << " size is " << p_data.size() << endl;
	Uint32 aux_data_index = p_index;

	// size
	p_data.resize(p_data.size() + 4);
	store_dword(p_data, aux_data_index, p_array.size());
	aux_data_index += 4;

	for (int i=0; i<(signed)p_array.size(); i++) {

		aux_data_index += store_dds(p_data, aux_data_index, &p_array[i]);
	};

	return aux_data_index - p_index;

};

int DDSPacker::store_dds(data& p_data, Uint32 p_data_index, DDS* p_dds) {

	Uint32 aux_data_index = p_data_index;

	DDSPacker::data aux_data;

	DDSPacker::pack(p_dds, aux_data);

	p_data.resize(p_data.size() + 4);
	store_dword(p_data, aux_data_index, (Uint32)aux_data.size());
	aux_data_index += 4;

	//@todo fix this crap
	for (int i=0; i<(signed)aux_data.size(); i++) {
		p_data.push_back(aux_data[i]);
	};
//	p_data.push_back(aux_data);

	aux_data_index += aux_data.size();

	return aux_data_index - p_data_index;

};


int DDSPacker::store_int_array(data& p_data, Uint32 p_index, vector<int> p_vector) {

	Uint32 aux_data_index = p_index;
	p_data.resize(p_data.size() + p_vector.size() * 4 + 4);

	// size
	store_dword(p_data, p_index, p_vector.size());
	aux_data_index += 4;

	for(int i=0; i<(signed)p_vector.size(); i++) {

		store_dword(p_data, aux_data_index, p_vector[i]);
		aux_data_index += 4;
	};

	return aux_data_index - p_index;
};


int DDSPacker::store_float_array(data& p_data, Uint32 p_index, vector<float> p_vector) {

//	cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "Storing float array at " << p_index << endl;
	Uint32 aux_data_index = p_index;
	p_data.resize(p_data.size() + p_vector.size() * 4 + 4);

	// size
	store_dword(p_data, p_index, p_vector.size());
	aux_data_index += 4;
	Uint32* aux_int;

	for(int i=0; i<(signed)p_vector.size(); i++) {

		aux_int = (Uint32*)&p_vector[i];

		store_dword(p_data, aux_data_index, *aux_int);
		aux_data_index += 4;
	};

	return aux_data_index - p_index;
};



int DDSPacker::store_string_array(data& p_data, Uint32 p_index, vector<string> p_list) {

//	cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "store_string_array: storing whole string array at " << p_index << " size is " << p_data.size() << endl;
	Uint32 aux_data_index = p_index;
	// size
	p_data.resize(p_data.size() + 4);
	store_dword(p_data, aux_data_index, p_list.size());
	aux_data_index += 4;

	for(int i=0; i<(signed)p_list.size(); i++) {

//		p_data.resize(p_data.size() + p_list[i].length() + 4);
		p_data.resize(p_data.size() + p_list[i].size() + 4);
		store_dword(p_data, aux_data_index, p_list[i].length());
		store_data(p_data, aux_data_index + 4, (Uint8*)p_list[i].c_str(), p_list[i].length());
		aux_data_index += p_list[i].length() + 4;
	};

	return aux_data_index - p_index;

};


void DDSPacker::store_data(data& p_data, Uint32 p_index, const Uint8* p_data_ptr, Uint32 p_size) {

	if (p_data.size() < p_index - 1) {
//		printf("store_data: warning!: %i - %i\n", p_data.size(), p_index);
	};
	if (p_size) {
		//p_data.resize(p_data.size() + p_size);
		memcpy(&p_data[p_index], (void*)p_data_ptr, p_size);
	};

};

void DDSPacker::store_dword(data& p_data, Uint32 data_index, Uint32 p_dword) {

	//p_data.resize(p_data.size() + 4);
	for (int i =0; i<4; i++) {
		Uint8 byte = p_dword & 0xFF;
		p_data[data_index + i] = byte;
		p_dword>>=8;
	};

};

bool DDSPacker::unpack(DDS* p_struct, const Uint8* p_data, Uint32 p_size) {

	if (!p_size || !p_data) {
		return 1;
	};

	Uint32 data_index = 0;
	Sint32 aux_extract_size;

	while (data_index < p_size) {
		if ((aux_extract_size = load_var(p_struct, p_data, data_index, p_size)) == -1) {
			ERROR("dds unpack: error loading var at " << data_index);
			return 0;
		};
		data_index += aux_extract_size;
	};

	return 1;
};

int DDSPacker::load_var(DDS* p_struct, const Uint8* p_data, Uint32 p_offset, Uint32 p_data_size) {

	Uint32 data_index = p_offset;
	Sint32 aux_length;

	string name;
	if ((aux_length = get_string(p_data, p_data_size, data_index, name)) == -1) {
		return -1;
	};
//	cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "Unpacking " << name.c_str() << " at " << data_index << endl;
	data_index += aux_length;

	// type
	DDS::Data_Types type = (DDS::Data_Types)p_data[data_index];
	data_index += 1;
	//	printf("(data index is %i)\n", data_index);

	if (data_index >= p_data_size) {
		return -1;
	};
	// [size] & data
	switch (type) {
		case DDS::T_INT: {
			Uint32 aux_int = get_dword(p_data, data_index);
			data_index += 4;
			p_struct->set_int_var(name, aux_int);
		}; break;

		case DDS::T_FLOAT: {
			Uint32 aux_int = get_dword(p_data, data_index);
			float* aux_float = (float*)&aux_int;
			data_index += 4;
			p_struct->set_float_var(name, *aux_float);
		}; break;

		case DDS::T_STRING: {

			string aux_string;
			if ((aux_length = get_string(p_data, p_data_size, data_index, aux_string)) == -1) {
				return -1;
			};
			data_index += aux_length;
			p_struct->set_str_var(name, aux_string);
		}; break;

		case DDS::T_POINTER: {
			// warning
			void* aux_pointer = (void*)get_dword(p_data, data_index);
			data_index += 4;
			p_struct->set_pointer_var(name, aux_pointer);
		}; break;

		case DDS::T_DATA: {
			Uint32 aux_size = get_dword(p_data, data_index);
			data_index += 4;

			if (data_index + aux_size > p_data_size) {
				return -1;
			};
			p_struct->set_data_var(name, (void*)&p_data[data_index], aux_size);
			data_index += aux_size;
		}; break;

		case DDS::T_INT_ARRAY: {

			vector<int> aux_vector;
			Uint32 aux_elements = get_dword(p_data, data_index);
			data_index += 4;
			if (data_index + aux_elements * 4 > p_data_size) {
				return -1;
			};
			aux_vector.reserve(aux_elements);
			for (Uint32 i=0; i<aux_elements; i++) {
				aux_vector.push_back(get_dword(p_data, data_index));
				data_index += 4;
			};
			p_struct->set_int_array_var(name, aux_vector);
		} break;

		case DDS::T_STRING_ARRAY: {

			vector<string> aux_vector;
			Uint32 aux_elements = get_dword(p_data, data_index);
			aux_vector.reserve(aux_elements);
//			cout << LIGHT_PURPLE << "INFO (DDS Packer): " << DEFAULT_COLOR << "load_var: loading string array with " << aux_elements << " elements at " << data_index << endl;
			data_index += 4;
			string aux_str;
			for (Uint32 i=0; i<aux_elements; i++) {

				if ((aux_length = get_string(p_data, p_data_size, data_index, aux_str)) == -1) {
					return -1;
				};
				data_index += aux_length;
				aux_vector.push_back(aux_str);

			};
			p_struct->set_str_array_var(name, aux_vector);
		} break;

		case DDS::T_DDS: {

			DDS aux_dds;
			Uint32 dds_size;

			if ((dds_size = get_dds(p_data, p_data_size, data_index, aux_dds)) < 0) {
				return -1;
			};

			data_index += dds_size;

			p_struct->set_dds_var(name, aux_dds);

		} break;

		case DDS::T_DDS_ARRAY: {

			VectorPointer<DDS> aux_vector;
			Uint32 aux_elements = get_dword(p_data, data_index);
//			printf("load_var: loading dds array with %i elements at %i\n", aux_elements, data_index);
			data_index += 4;
			for (Uint32 i=0; i<aux_elements; i++) {

				DDS aux_dds;
				Uint32 dds_size;

				if ((dds_size = get_dds(p_data, p_data_size, data_index, aux_dds)) < 0) {
					return -1;
				};

				data_index += dds_size;

				aux_vector.add_element(aux_dds);
			};

			p_struct->set_dds_array_var(name, aux_vector);

		} break;

		case DDS::T_FLOAT_ARRAY: {

			vector<float> aux_vector;
			Uint32 aux_elements = get_dword(p_data, data_index);
//			printf("load_var: loading float array with %i elements at %i\n", aux_elements, data_index);
			data_index += 4;
			if (data_index + aux_elements * 4 > p_data_size) {
				return -1;
			};
			aux_vector.reserve(aux_elements);
			Uint32 aux_int;
			float* aux_float;
			for (Uint32 i=0; i<aux_elements; i++) {
				aux_int = get_dword(p_data, data_index);

				aux_float = (float*)&aux_int;

				aux_vector.push_back(*aux_float);
				data_index += 4;
			};
			p_struct->set_float_array_var(name, aux_vector);

		} break;
		default: {
			ERROR("FIXME: Unhandled DDS TYPE!!");
		};
	};
	return data_index - p_offset;
};


int DDSPacker::get_dds(const Uint8* p_data, Uint32 p_data_size, Uint32 p_data_index, DDS &p_dds) {

	Uint32 aux_size = get_dword(p_data, p_data_index);
//	printf("load_var: loading DDS with %i size at %i\n", aux_size, p_data_index);

	p_data_index += 4;
	if (p_data_index + aux_size > p_data_size) {

		return -1;
	};

	DDSPacker::unpack(&p_dds, &p_data[p_data_index], aux_size);

	return aux_size + 4;

};

Uint32 DDSPacker::get_dword(const Uint8* p_data, Uint32 p_index) {

	Uint32 aux_value = 0;
	for (int i=3; i>=0; i--) {
		aux_value <<=8;
		aux_value |= p_data[p_index + i];
	};
	return aux_value;
};

Sint32 DDSPacker::get_string(const Uint8* p_data, Uint32 p_data_size, Uint32 p_index, string &p_str) {

	if (p_index >= p_data_size) {
		//printf("****************size\n");
		return -1;
	};

	Uint32 aux_size = get_dword(p_data, p_index);
	if (p_index + aux_size > p_data_size) {
		//printf("****************size2: %i, %i, %i\n", p_index, aux_size, p_data_size);
		return -1;
	};

	char* aux_tmp_cstr = (char*)calloc(aux_size +1, 1);
	memcpy(aux_tmp_cstr, &p_data[p_index + 4], aux_size);
	p_str = aux_tmp_cstr;
	return p_str.length() + 4;
};
