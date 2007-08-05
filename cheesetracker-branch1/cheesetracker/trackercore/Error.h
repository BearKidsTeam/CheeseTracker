//	error - A set of classes for carrying over system errors.
//
//	The classes in this directory that use Unix system calls
//      throw exceptions derived from Error. Error::what() in
//	these exceptions will give a string in the form:
//
//		"class::function: unix_call: Some error message"
//
//	where class::function is the name of the class function the
//	error occured in, unix_call is the name of the Unix system
//	call that caused the error, and "Some error message" is
//	a description of the error consistent with what you would
//	get from strerror(errno). Some class procedures that throw
//	exceptions may generate additional information. For example,
//	file I/O exceptions would return the name of the file that
//	caused the error.
//

#ifndef _ERROR_H
#define _ERROR_H

#include <exception>
#include <cstring>
#include <string>

#ifdef _WINDOWS
# include <windows.h>
#endif

using std::exception;
using std::string;

#ifndef _WINDOWS
  using std::strcpy;
  using std::strcat;
#endif

#define DERIVE_EMPTY(parent, child) class child : public parent { public: }
#define GENERIC_ERROR(name) DERIVE_EMPTY(Error, name)
#define FATAL_ERROR(name) \
	class name : public Error {								\
		public:										\
			name(const char *file, int line, const char *why=NULL) : Error() {	\
				set_error_pfx(file);						\
				if (why == NULL) {						\
					eprintf("%i: %s", line, #name);				\
				} else {							\
					eprintf("%i: %s: %s", line, #name, why);		\
				}								\
			}									\
	}

#define THROWF(type, pfx, format, args...) { \
	type E;				\
	E.set_error_pfx(pfx);		\
	E.eprintf(format , ##args);\
	throw E;			\
}

class Error : public exception
{
	public: 
		char error_buf[2049];
		char error_func[2051];
		char error_msg[5000];
		int _errno;
#ifdef _WINDOWS
		DWORD winerr;
#endif
		void set_error_pfx(const char*);
		void set_error(const char*);
		void set_error_pfx(string);
		void set_error(string);
#ifdef _WINDOWS
		void report_winerr(DWORD);
#endif
		void report_errno(int);
		void eprintf(const char *fmt, ...);
		void fatal_error();
		void qt_fatal_error();
		virtual const char* what()
		{
			strcpy(error_msg, error_func);
			strcat(error_msg, ": ");
			strcat(error_msg, error_buf);
			return error_msg;
		}
		Error() { error_buf[0] = '\0'; error_func[0]='\0'; _errno=0; }
		Error(int err, const char *pfx) {
			Error();
			report_errno(err);
			set_error_pfx(pfx);
		}

		virtual ~Error() throw() { return; }
};

class bug : public Error
{
	public:
		virtual const char *what()
		{
			strcpy(error_msg, error_func);
			strcat(error_msg, ": BUG: ");
			strcat(error_msg, error_buf);
			return error_msg;
		}
};

FATAL_ERROR(Out_Of_Bounds);
GENERIC_ERROR(File_Error);
DERIVE_EMPTY(File_Error, File_Corrupt);
#endif
