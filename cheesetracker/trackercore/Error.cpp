#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stdarg.h>
#include <QMessageBox>
#include "Error.h"

#ifdef _WINDOWS
#  ifdef __CYGWIN__
#  elif !defined(snprintf)
#    define snprintf _snprintf
#  endif
#else
  using std::strlen;
  using std::strncpy;
#endif

namespace Error_cc {


void
string_to_char(char *dest, string source, size_t len)
{
	string::iterator ix;
	size_t jx;

	for(ix=source.begin(), jx=0;
	    ix != source.end() && jx < len;
	    ix++, jx++)
		dest[jx] = *ix;
	dest[jx]=0;
		
}

// count_digits - calculate how many digits are in a number
// 
// return       - The number of digits.

size_t
count_digits(int num)
{
	size_t counter = 0;
	if(num == 0) return 1;

	if(num < 0)
	{
		counter++;
		num = -num;
	}

	while(num != 0)
	{
		num /= 10;
		counter++;
	}
	
	return counter;
}

}

void
Error::fatal_error()
{
	std::cerr << what() << std::endl;
	abort();
}

void
Error::qt_fatal_error()
{
	QMessageBox::critical(NULL, "Fatal Error", what(), "Exit");
	abort();
}

void
Error::set_error_pfx(const char *pfx)
{
	strncpy(error_func, pfx, sizeof(error_func));
}

void
Error::set_error_pfx(string pfx)
{
	Error_cc::string_to_char(error_func, pfx, sizeof(error_func));
}

void
Error::set_error(const char *err)
{
	strncpy(error_buf, err, sizeof(error_buf));
}

void
Error::set_error(string err)
{
	Error_cc::string_to_char(error_buf, err, sizeof(error_buf));
}

#ifdef _WINDOWS

void
Error::report_winerr(DWORD errcode)
{
	char buffer[2049];

	winerr = errcode;

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode, 0, buffer,
	              2049, NULL);
	set_error(buffer);
}

#endif

void
Error::report_errno(int errnum)
{
	Error E;

	set_error(strerror(errnum));
	_errno = errnum;
}

void
Error::eprintf(const char *fmt, ...)
{
	va_list args; 
	const char *fptr; // A pointer into {fmt}.
	size_t len; // The current length of {error_buf}.

	// These variable names below correspond to the
	// letter in the format string that represents them.  
	int i;     // An integer.
	char *s;   // A pointer to a string.

	fptr=fmt;

	va_start(args, fmt);
	for(len=0; *fptr && len < 2047; fptr++)
	{
		if(*fptr == '%')
		{
			fptr++;
			switch(*fptr)
			{
				case 'c':
				{
					i = va_arg(args, int);
					if(len < 2048)
						error_buf[len++] = i;
					break;
				}
				case 's':
				{
					size_t s_ix = 0;
					size_t s_len;

					s = va_arg(args, char*);

					s_len = strlen(s);
					// Loop ends either when
					// the string is copied
					// or when the end of
					// {error_buf} is reached.

					for(; s_ix < s_len && s_ix + len < 2047; s_ix++)
					{
						error_buf[len++] =
						s[s_ix];
					} 
					break;
				}
				case 'e':
				{
					// Put the error message here.

					size_t proposed_len;
					
					char buffer[2049];
					buffer[2048]=0;
#ifdef _WINDOWS
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, winerr, 0, buffer,	2048,
	              			, NULL);
#else
					strncpy(buffer, strerror(_errno), 2048);
#endif

					snprintf(error_buf+len, 2047-len, "%s", buffer);

					proposed_len=strlen(buffer);
					len += (proposed_len > 2047-len ? 2047-len : proposed_len);
				}
				case 'i':
				{
					size_t digits;
					size_t remaining;


					i = va_arg(args, int);
					digits = Error_cc::count_digits(i);
					remaining = 2047-len;

					if(remaining < digits)
						digits = remaining;

					snprintf(error_buf+len, digits+1,
						 "%d", i);
					error_buf[2048] = '\0';
					len += digits;
					break;
				} 
				case 'f':
				{
					double f = va_arg(args, double);

					// C99 behavior assumed. In
					// environments where snprintf
					// returns -1 when it truncates
					// the string, SNPRINTF_TRUNC_NEG_RET
					// must be defined, or incorrect
					// behavior will result.

					len +=
					snprintf(error_buf+len, 2047-len,
					         "%f", f);
#ifdef SNPRINTF_TRUNC_NEG_RET
					if(len == -1)
						len=2047;
#else
					if(len > 2047)
						len=2047;
#endif
					error_buf[2048] = '\0';
					break;
				}
				case '%':
					// "%%" in a format string
					// is copied as a single '%'.
					// This is much, much different
					// from the behavior that results
					// from using a single '%' in a
					// format string (see below).

					error_buf[len++] = '%';
					break;
				default:
				{
					// '%' DOES NOT belong by itself
					// in a format string.
					//
					// This code replaces all invalid
					// '%'s with a message that makes
					// the bug more obvious. 

					size_t remaining;
					const char *format_bug="{Format string bug brought to you by another stupid programmer}"; 

					size_t buglen = strlen(format_bug);

					remaining = 2047 - len;

					// The issue of strncpy leaving
					// the string unterminated is
					// addressed at the end of this
					// function.
					strncpy(error_buf+(len-1), format_bug,
					             remaining);
					// The issue of strncpy leaving
					// the string unterminated is
					// addressed at the end of this
					// function.
					if(buglen > remaining)
						buglen = remaining;

					len += buglen;

				}
			}
		}
		else
		{
			error_buf[len++] = *fptr;
		}
	}
	va_end(args);
	error_buf[len] = 0;
	error_buf[2048]=0;
}
