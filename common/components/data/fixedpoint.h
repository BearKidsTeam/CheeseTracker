#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H
#include <new>

class fixedpoint_index_t {
	size_t lower;
	size_t upper;
public:
	fixedpoint_index_t();
	operator += (fixedpoint &rhs);
	operator -= (fixedpoint &rhs);
	operator *= (fixedpoint &rhs);
	operator /= (fixedpoint &rhs);
	
};

#endif
