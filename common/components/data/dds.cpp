#include <cstring>
#include "dds.h"

using std::vector;

DDS::Data_Node::~Data_Node() {

	if (type==T_DATA) {

//		printf("(%x) killing data_ptr %x\n",this,data_ptr);
		free(data_ptr);

	}

	if (type==T_DDS) {
		delete dynamic_data_struct;
	};

}

DDS::Data_Node::Data_Node() {

	// this is in case type gets accidentally set to 'T_DDS' or 'T_DATA', and then deleted
	type = T_MAX;
};

// copy constructor
DDS::Data_Node::Data_Node(const DDS::Data_Node& rvalue) {

	copy_from(&rvalue);

};


void DDS::set_int_var(string p_var,int p_value) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_INT) {

			ERROR("attempt to change the type of variable '");
			return;
		}
	}

	new_node.int_data=p_value;
	new_node.type=T_INT;
	nodes[p_var]=new_node;

}

void DDS::set_str_var(string p_var,string p_value) {

	Data_Node_List::const_iterator I;
	//Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_STRING) {

			ERROR("attempt to change the type of variable `" << (string)p_var << "' from str->num! Delete it first!");
			return;
		}
	}

	nodes[p_var].string_data=p_value;
	nodes[p_var].type=T_STRING;
}

void DDS::set_pointer_var(string p_var,void *p_ptr) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_POINTER) {

			ERROR("attempt to change the type of variable `" << (string)p_var << "' from str->num! Delete it first!");
			return;
		}
	}

	new_node.data_ptr=p_ptr;
	new_node.type=T_POINTER;
	nodes[p_var]=new_node;

}


void DDS::set_data_var(string p_var,void *p_ptr, int p_size) {


	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_DATA) {

			ERROR("attempt to change the type of variable `" << (string)p_var << "' from str->num! Delete it first!");
			return;
		}

		free(nodes[p_var].data_ptr);
	}


	new_node.data_ptr=malloc(p_size);

	memcpy(new_node.data_ptr,p_ptr,p_size);

	new_node.raw_data_size=p_size;
	new_node.type=T_DATA;
	nodes[p_var]=new_node;

}

void DDS::set_float_var(string p_var, float p_value) {

	Data_Node_List::const_iterator I;
        Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_FLOAT) {

			ERROR("attempt to change the type of variable '");
			return;
		}
	}

	new_node.float_data=p_value;
	new_node.type=T_FLOAT;
	nodes[p_var]=new_node;

};

vector_int& DDS::set_int_array_var(string p_var,const vector<int>& p_array) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_INT_ARRAY) {

			ERROR("attempt to change the type of variable '");
			return *((vector_int*)NULL);
		}
	}

	new_node.int_array=p_array;
	new_node.type=T_INT_ARRAY;

	nodes[p_var]=new_node;

	return nodes[p_var].int_array;
}

vector_string& DDS::set_str_array_var(string p_var,const vector<string> &p_array) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_STRING_ARRAY) {

			ERROR("attempt to change the type of variable '");
			return *((vector_string*)NULL);
		}
	}

	new_node.type=T_STRING_ARRAY;
	new_node.string_array=p_array;

	nodes[p_var]=new_node;

	return nodes[p_var].string_array;
}


vector_float& DDS::set_float_array_var(string p_var, const vector<float> &p_array) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_FLOAT_ARRAY) {

			ERROR("attempt to change the type of variable '");
			return *((vector_float*)NULL);
		}
	}

	new_node.type=T_FLOAT_ARRAY;
	new_node.float_array=p_array;

	nodes[p_var]=new_node;

	return nodes[p_var].float_array;
};



DDS& DDS::set_dds_var(string p_var, const DDS& p_dds) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_DDS) {

			ERROR("attempt to change the type of variable '");
			return *((DDS*)NULL);
		}
	}

	new_node.type=T_DDS;

	new_node.dynamic_data_struct = new DDS;
	new_node.dynamic_data_struct->copy_from(&p_dds);
	nodes[p_var]=new_node;

	return *nodes[p_var].dynamic_data_struct;
};


vector_dds& DDS::set_dds_array_var(string p_var, const VectorPointer<DDS>& p_dds_array) {

	Data_Node_List::const_iterator I;
	Data_Node new_node;

	if ((I=nodes.find(p_var))!=nodes.end()) {

		if (I->second.type!=T_DDS_ARRAY) {

			ERROR("attempt to change the type of variable '");
			return *((vector_dds*)NULL);
		}
//		delete nodes[p_var].dynamic_data_struct;
	}

	new_node.type=T_DDS_ARRAY;
	new_node.dds_array=p_dds_array;

	nodes[p_var]=new_node;

//	nodes[p_var].type=T_DDS_ARRAY;
//	nodes[p_var].dds_array = p_dds_array;

	return nodes[p_var].dds_array;
};


DDS& DDS::get_dds_var(string p_var) {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined DDS variable: " << (string)p_var << ".");
//		I->second.string_array.clear();
	}

	return *(I->second.dynamic_data_struct);

};


VectorPointer<DDS>& DDS::get_dds_array_var(string p_var) {

//	Data_Node_List::const_iterator I;
	Data_Node_List::iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
//		I->second.string_array.clear();
	}

	return I->second.dds_array;

};



int DDS::get_int_var(string p_var) {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined numeric variable: " << (string)p_var << ".");
		return 0;

	} else {

		return I->second.int_data;
	}
}

float DDS::get_float_var(string p_var) {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined float variable: " << (string)p_var << ".");
		return 0;

	} else {

		return I->second.float_data;
	}
}

string DDS::get_str_var(string p_var) {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined string variable: " << (string)p_var << ".");
		return "<ERROR!>";

	} else {

		return I->second.string_data;
	}
}

void* DDS::get_pointer_var(string p_var)  {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined pointer variable: " << (string)p_var << ".");
		return NULL;

	} else {

		return I->second.data_ptr;
	}

}

void* DDS::get_data_var(string p_var)  {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
		return NULL;

	} else {

		return I->second.data_ptr;
	}

}

const vector<int>& DDS::get_int_array_var(string p_var)  {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
//		I->second.int_array.clear();
	}

	return I->second.int_array;


}

const vector<string>& DDS::get_str_array_var(string p_var)  {

	Data_Node_List::const_iterator I;


	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
//		I->second.string_array.clear();
	}

	return I->second.string_array;


}

const vector<float>& DDS::get_float_array_var(string p_var) {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
//		I->second.string_array.clear();
	}

	return I->second.float_array;

};


void DDS::write_data_var(string p_var,void *p_data_ptr) {

	Data_Node_List::iterator I;


	 I=nodes.find(p_var);

	if ( I==nodes.end()) {

		ERROR("attempt to retrieve undefined data variable: " << (string)p_var << ".");
		return  ;

	} else {

		memcpy(p_data_ptr,I->second.data_ptr,I->second.raw_data_size);
	}
}

int DDS::get_data_var_size(string p_var)  {

	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to retrieve undefined data size of variable: " << (string)p_var << ".");
		return 0;

	} else {

		return I->second.raw_data_size;
	}

}


void DDS::delete_var(string p_var) {

	if (nodes.find(p_var)==nodes.end()) {

		ERROR("attempt to delete undefined string variable: " << (string)p_var << ".");

	} else {

		nodes.erase(p_var);
	}
}

bool DDS::is_var(string p_var) {

	return (nodes.find(p_var)!=nodes.end());

}

bool DDS::is_var(string p_var, Data_Types p_type) {

	Data_Node_List::const_iterator I = nodes.find(p_var);

	if (I != nodes.end()) {

		return I->second.type == p_type;
	};

	return false;
};

int DDS::get_var_count() {

	return nodes.size();
}

DDS::Data_Types DDS::get_var_type(int p_index) { return T_INT; };

DDS::Data_Types DDS::get_var_type(string p_var) {


	Data_Node_List::const_iterator I;

	if ((I=nodes.find(p_var))==nodes.end()) {

		ERROR("attempt to get type of unexisting variable: " << (string)p_var << ".");
		return T_MAX;

	} else {

		return I->second.type;
	}

}
string DDS::get_var_name(int p_index) {

	int i;
	Data_Node_List::const_iterator I;

	if ((p_index<0) || ((unsigned)p_index>=nodes.size())) {

		ERROR("Get variable name, index out of range!");
		return 0;
	}

	for (I=nodes.begin(),i=0;I!=nodes.end();++i,++I) {

                if (i==p_index) return I->first;
	}

	ERROR("Danger! Variable index not found!");

	return "<error>"; // tho, it should never reach here
}


void DDS::clear() {

	nodes.clear();
}

DDS::Data_Node_List::const_iterator DDS::get_iterator_begin() {

  return (Data_Node_List::const_iterator)nodes.begin();
};

DDS::Data_Node_List::const_iterator DDS::get_iterator_end() {

  return (Data_Node_List::const_iterator)nodes.end();
};


void DDS::copy_from(const DDS *rvalue) {

	Data_Node_List::const_iterator I;
	nodes.clear();

	/*
	for (I=rvalue->nodes.begin();I!=rvalue->nodes.end();++I) {
		nodes[I->first]=I->second;
	}
	*/
	nodes = rvalue->nodes;
}


const DDS& DDS::operator=(const DDS &rvalue) {

	copy_from(&rvalue);
	return *this;
}


const DDS::Data_Node& DDS::Data_Node::operator=(const Data_Node &rvalue) {


	copy_from(&rvalue);
	return *this;
};


void DDS::Data_Node::copy_from(const DDS::Data_Node *rvalue) {

	type=rvalue->type;

	switch(type) {
		case T_INT: {

  			int_data=rvalue->int_data;
		} break;

		case T_FLOAT: {

			float_data=rvalue->float_data;
		} break;

		case T_STRING: {
			string_data=rvalue->string_data;
		} break;

		case T_POINTER: {
			data_ptr=rvalue->data_ptr;
		} break;

		case T_DATA: {
			raw_data_size=rvalue->raw_data_size;
			data_ptr=malloc(raw_data_size);
			memcpy(data_ptr,rvalue->data_ptr,raw_data_size);
		} break;

		case T_INT_ARRAY: {

			int_array=rvalue->int_array;
		} break;

		case T_STRING_ARRAY: {
			string_array=rvalue->string_array;
		} break;

		case T_FLOAT_ARRAY: {
			float_array = rvalue->float_array;
		} break;

		case T_DDS: {
			dynamic_data_struct = new DDS;
			dynamic_data_struct->copy_from(rvalue->dynamic_data_struct);
		} break;

		case T_DDS_ARRAY: {
			dds_array = rvalue->dds_array;
		} break;

		case T_MAX: {};
	}

//	return *this;
}

bool DDS::load_from_string(string p_dds) {

	int pos = 0;
	int old_pos = 0;

	bool on_string = false;
	int escaping = false;
	bool on_name = true;

	// a parser (needs work)
	while(pos < (int)p_dds.size()) {

		switch(p_dds[pos]) {

			case '=': {

				if (!on_string) {

					if (!on_name) {  // '=' at the begining?

						ERROR("error loading string '"<< p_dds << "' at position "<<pos);
						return false;
					} else {

						on_name = false;
					};
				};

			}; break;

			case '\\': {

				if (!escaping && on_string) {

					escaping = 2;
				};
			}; break;

			case '\'': {

				if (on_string) {

					if (!escaping) {

						on_string = false;
					};
				} else {

					if (on_name) {

						ERROR("error loading string '"<< p_dds << "' at position "<<pos);
						return false;
					} else {

						on_string = true;
					};
				};

			}; break;

			case ',': {

				if (!on_string) {

					if (on_name) {

						ERROR("error loading string '"<< p_dds << "' at position "<<pos);
						return false;
					} else {

						on_name = true;
						if (!load_pair(p_dds, old_pos, pos)) {

							ERROR("error loading pair from string '"<< p_dds << "' at position "<<pos);
							return false;
						};
						old_pos = pos+1;
					};
				};
			}; break;
		};

		if (escaping) {

			escaping--;

			if(!escaping) {

				p_dds.erase(pos - 1);
				pos--;
			};
		};
		pos++;
	};

	if (on_string) {

		ERROR("unterminated string on dds string '"<<p_dds<<"'");
		return false;
	};

	if (!on_name) {

		if (!load_pair(p_dds, old_pos, pos)) {

			ERROR("error loading pair from string '"<< p_dds << "' at position "<<pos);
			return false;
		};
	};


	return true;
};

bool DDS::load_pair(string& p_dds, int p_begin, int p_end) {

	unsigned middle = p_dds.find("=", p_begin);

	if (middle > p_end || middle == string::npos) {

		printf("pair %i-%i has no =\n", p_begin, p_end);
		return false;
	};

	string name = get_trim(p_dds, p_begin, middle);
	string value = get_trim(p_dds, middle +1, p_end);
	if (!name.size()) {

		return false;
	};

	if (value[0] == '\'') {

		printf("****** adding string %s with value %s\n", name.c_str(), value.c_str());
		set_str_var(name, value.substr(1, value.size() -2));
	} else {

		if (value.find(".") != string::npos) {

			printf("****** adding float %s with value %s\n", name.c_str(), value.c_str());
			set_float_var(name, atof(value.c_str()));
		} else {

			printf("****** adding int %s with value %s\n", name.c_str(), value.c_str());
			set_int_var(name, atoi(value.c_str()));
		};
	};
	return true;
};

string DDS::get_trim(string& p_str, int p_begin, int p_end) {

	while(p_begin < (int)p_str.size() && p_begin < p_end && p_str[p_begin] == ' ') {

		p_begin++;
	};

	while(p_end >= 0 && p_end > p_begin && (p_str[p_end -1] == ' ' || p_str[p_end -1] == '\n' || p_str[p_end -1] == '\t'|| p_str[p_end - 1] == '\r')) {

		--p_end;
	};

	return p_str.substr(p_begin, p_end - p_begin);
};

// copy constructor
DDS::DDS(const DDS& rvalue) {

	copy_from(&rvalue);

};

DDS::DDS(string p_value) {

	load_from_string(p_value);
};


DDS::DDS(){
}
DDS::~DDS(){
}
