#ifndef NS_AUTOPTR_H
#define NS_AUTOPTR_H
#ifndef size_t
#include <new>
#endif

// ns_autoptr -- Nonstandard autopointer.
//
// Store your pointers in an ns_autoptr to have your
// pointer be deleted when the ns_autoptr goes out
// of scope. You must still use the original pointer
// to access the data, so you don't need to rewrite
// your whole program to take ns_autoptr arguments. 
//


template <class ANY>
class ns_autoptr
{
	public:
		ns_autoptr(void) { n=0; arr=0; }
		~ns_autoptr() { if (n) delete n; if(arr) delete[] arr; }
		void ptr_delete() { if(n) delete n; n=0;
		                    if(arr) delete[] arr; arr=0; }
		void ptr_new(ANY *unit) { n=unit; }
		void arr_new(ANY *unit) { arr=unit; }
		void clear() { n=0; arr=0; }
	protected:
		ANY *n;
		ANY *arr;
};

#endif
