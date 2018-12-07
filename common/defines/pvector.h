#ifndef PVECTOR_H
#define PVECTOR_H

#include <vector>

template<class T>
class VectorPointer {

	std::vector<T*> vector;

public:

	void add_element(T& p_element) {

		T* aux_t = new T;
		(*aux_t) = p_element;
		vector.push_back(aux_t);

	};

	inline void push_back(T& p_element) {

		add_element(p_element);
	};

	T& get_element(unsigned int p_index)  {

		if (!(p_index >=0 && p_index < vector.size())) {
			T* ret_val = NULL;
			return (T&)(*ret_val);
		};

		return *(vector[p_index]);

	};

	const T& get_element(unsigned int p_index) const {

		return *(vector.operator[](p_index));
	};

	void delete_element(int p_index)  {

		if (!(p_index >=0 && p_index < vector.size())) {
			return;
		};

		delete vector[p_index];
//		(*this)[p_index] = NULL;
		vector.erase(vector.begin() + p_index);
	}


	const VectorPointer<T>& operator=(const VectorPointer<T> &rvalue)  {

		copy_from(&rvalue);

		return *this;
	};

	T& operator[](int p_index) {

		return get_element(p_index);

	};

	const T& operator[](int p_index) const {

		return get_element(p_index);
	};

	size_t size() const {

		return vector.size();
	};


	VectorPointer(int p_size=0) {
		resize(p_size);
//		printf("***************** vectorpointer %x constructor\n", this);
	};

	//copy constructor
	VectorPointer(const VectorPointer<T> &rvalue)  {

		copy_from(&rvalue);

	};

	~VectorPointer()  {

		clear();

	};

//	void VectorPointer::clear() {
	void clear() {

		for (unsigned int i=0;i<vector.size();i++) {

			delete vector[i];
//			vector[i] = NULL;

		};
		vector.clear();
	};

	void resize(int p_size) {

		if (p_size>(int)vector.size()) {
			
			int from=size();
			vector.resize(p_size);			
			for (int i=from;i<p_size;i++)
				vector[i]=new T;
			
		} else if (p_size<(int)vector.size()) {
					
			for (int i=p_size;i<(int)size();i++)
				delete vector[i];
			
			vector.resize(p_size);			
		
		}					
	}
	
private:

	void copy_from(const VectorPointer<T> *rvalue) {

		typename std::vector<T*>::const_iterator I;
		clear();

		for (I = rvalue->vector.begin(); I != rvalue->vector.end(); ++I) {

			add_element(**I);
		};

	};

};

#endif
