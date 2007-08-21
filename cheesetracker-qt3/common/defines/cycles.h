#ifndef CYCLES_H
#define CYCLES_H

#ifdef _USINGCL

#include "typedefs.h"

typedef Uint64 cycles_t;
extern cycles_t cacheflush_time;

static inline cycles_t get_cycles (void)
{
	unsigned long ret;
	__asm rdtsc
	__asm mov ret, eax
	return ret;
}
#else // _USINGCL

#ifdef __i386__

#include "typedefs.h"

typedef Uint64 cycles_t;

extern cycles_t cacheflush_time;

#define rdtscll(val) \
     __asm__ __volatile__("rdtsc" : "=A" (val))

static inline cycles_t get_cycles (void)
{
	unsigned long long ret;

	rdtscll(ret);
	return ret;
}

#else  /* !i386 */

#ifdef __powerpc__
#define CPU_FTR_601			0x00000100

typedef unsigned long cycles_t;

/*
 * For the "cycle" counter we use the timebase lower half.
 * Currently only used on SMP.
 */

extern cycles_t cacheflush_time;

static inline cycles_t get_cycles(void)
{
	cycles_t ret = 0;

	__asm__ __volatile__(
		"98:	mftb %0\n"
		"99:\n"
		".section __ftr_fixup,\"a\"\n"
		"	.long %1\n"
		"	.long 0\n"
		"	.long 98b\n"
		"	.long 99b\n"
		".previous"
		: "=r" (ret) : "i" (CPU_FTR_601));
	return ret;
}

#else
typedef Uint64 cycles_t;

static inline cycles_t get_cycles (void) {

	return 0;
}

#endif /* PPC */
#endif /* i386 */
#endif // _WIN32
#endif /* cycles_h__ */
