/**
 * @file    dynamic_data_struct.h
 * @brief   Definition of class DDS
 * @date    Sat Nov 24 00:56:00 ART 2001
 *
 * Suggestions:
 *
 */

#ifndef DDS_H
#define DDS_H
#include "typedefs.h"
#include "hash_stuff.h"

#include <list>
using std::list;

#include "pvector.h"

#ifdef _USINGCL
#pragma function(memcpy)  // VC++ users, for retail builds, I had to disable memcpy intrinsic function since it
                          // was performing conflicts with the actual code, seems memcpy could not be expanded
			  // instrically inside DDS
#endif

class DDS;

typedef std::vector<int> vector_int;
typedef std::vector<string> vector_string;
typedef std::vector<float> vector_float;
typedef VectorPointer<DDS> vector_dds;
typedef int data_size;

/**
 * @class DDS
 * @brief Dynamic data struct, for storing variables and other miscelaneous things!
 *
 * Responsibilities:
 *
 * Collaborations:
 *
 * Interface:
 *
 *
 */

class DDS {
public:


	enum Data_Types {

		T_INT,
		T_FLOAT,
		T_STRING,
		T_POINTER,
		T_DATA,
		T_INT_ARRAY,
		T_STRING_ARRAY,
		T_FLOAT_ARRAY,
		T_DDS,
		T_DDS_ARRAY,
		T_MAX
	};

protected:

	class Data_Node {

	public:
		Data_Types type;

		union {

			int int_data;
			void * data_ptr;
			float float_data;
			DDS* dynamic_data_struct;

		};

		std::vector<string> string_array;
		std::vector<int> int_array;
		std::vector<float> float_array;

		VectorPointer<DDS> dds_array;


		//not in union.
		string string_data;
		int raw_data_size;

		const Data_Node& operator=(const Data_Node &rvalue);

		void copy_from(const DDS::Data_Node *rvalue);

		// copy constructor
		Data_Node(const Data_Node& rvalue);
		~Data_Node();
		Data_Node();
	};


public:

	typedef hash_map<string,Data_Node,String_Hash> Data_Node_List;


protected:
	Data_Node_List nodes;

private:
	// helpers for load_from_string
	bool load_pair(string& p_dds, int p_begin, int p_end);
	string get_trim(string& p_str, int p_begin, int p_end);

public:

	typedef std::vector<int> vector_int;
	typedef std::vector<string> vector_string;
	typedef std::vector<float> vector_float;
	typedef VectorPointer<DDS> vector_dds;

	Data_Node_List::const_iterator get_iterator_begin();
	Data_Node_List::const_iterator get_iterator_end();

	const DDS& operator=(const DDS &rvalue);
	void copy_from(const DDS *rvalue);

	void set_int_var(string p_var,int p_value);
	void set_str_var(string p_var,string p_value);
	void set_pointer_var(string p_var,void *p_ptr);
	void set_data_var(string p_var,void *p_ptr, int p_size);
	void set_float_var(string p_var, float p_value);

	vector_int& set_int_array_var(string p_var,const vector_int& p_array = vector_int());
	vector_string& set_str_array_var(string p_var,const vector_string &p_array = vector_string());
	vector_float& set_float_array_var(string p_var, const vector_float &p_array = vector_float());

	DDS& set_dds_var(string p_var, const DDS& p_dds = DDS());
	vector_dds& set_dds_array_var(string p_var, const vector_dds& p_dds_array = vector_dds());

	DDS& get_dds_var(string p_var);
	vector_dds& get_dds_array_var(string p_var);

	int get_int_var(string p_var);
	float get_float_var(string p_var);
	string get_str_var(string p_var);
	void* get_pointer_var(string p_var);

	const vector_int& get_int_array_var(string p_var);
	const vector_string& get_str_array_var(string p_var);
	const vector_float& get_float_array_var(string p_var);

	void* get_data_var(string p_var);
	void write_data_var(string p_var,void *p_data_ptr);

	int get_data_var_size(string p_var);

	// returns false on error
	bool load_from_string(string p_msg);

	void delete_var(string p_var);

	bool is_var(string p_var);
	bool is_var(string p_var, Data_Types p_type);

	int get_var_count();
	Data_Types get_var_type(string p_var);

	string get_var_name(int p_index);
	Data_Types get_var_type(int p_index);

	void clear();

	// copy constructor
	DDS(const DDS& rvalue);
	DDS(string p_value);

	DDS();
	~DDS();
};

/*
#ifdef _USINGCL
#pragma intrinsic(memcpy) // check line 19
#endif
*/
#endif
