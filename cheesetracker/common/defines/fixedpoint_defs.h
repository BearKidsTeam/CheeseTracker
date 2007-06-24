#ifndef FIXEDPOINT_DEFS_H
#define FIXEDPOINT_DEFS_H

/* What follows is an explanation of fixed-point math, as it is used
 * in this program.

 * A "fixed-point" number is an integer that represents a non-integer
 * number. Unlike floating-point numbers, which contain a "mantissa"
 * that indicates where the decimal point is at the moment, fixed-point
 * numbers have a set number of bits after the decimal point.

 * Any integer can be converted into a fixed-point number with a simple
 * bit shift:
 *
 *     int my_fixedpoint = my_int << FIXED_POINT_INT_PART_BITS
 *
 * Conversely, a fixedpoint number can be converted back to a
 * normal integer with the opposite bit shift:
 *
 *    int my_int = my_fixedpoint >> FIXED_POINT_INT_PART_BITS
 *
 * We define two convenient macros for performing these operations:
 *
 *    int my_fixedpoint = INT_TO_FIXED(my_int);
 *
 *    int my_int = FIXED_TO_INT(my_fixedpoint);
 *
 * Also defined are two macros that convert a variable in-place:
 *
 *    int xx = SOMETHING;
 *
 *    IP_INT_TO_FIXED(xx);
 *    IP_FIXED_TO_INT(xx);
 *
 * The arguments to the IP_ prefixed macros must be lvalues.
 *
 * Doing math on fixed-point numbers is tricky because bit-shifting
 * by FIXEDPOINT_INT_PART_BITS is equivalent to multiplying by (1 <<
 * FIXDPOINT_INT_PART_BITS).  As a result, sometimes the result of an
 * operation involving fixed-point numbers will be a normal integer.
 *
 * For example, let F equal (1 << FIXEDPOINT_INT_PART_BITS):
 *
 *   int result = 20*F / 10*F;
 *
 * In the above case, 'F' cancels of the division, so the result is a
 * normal integer.  Therefore, if you want to divide the fixed-point
 * number 20 by 10 and have the result be another fixed-point number,
 * you should divide by a normal integer:
 *
 *   int result = 20*F / 10;
 *
 * In that case, the result is the fixed-point value.
 *
 * There are also operations for which the result is not likely to be
 * useful. Consider this:
 *
 *   int result = 10*F * 2*F;
 *
 * The result is 20*F*F, which is neither fixed-point nor integer.
 * Therefore, the result is considered to be meaningless. 
 *
 * While it is possible to convert such an expression to integer:
 *
 *   int result = FIXED_TO_INT(FIXED_TO_INT(10*F * 2*F));
 *
 * ...this would make the program difficult to understand, plus, you're
 * bit-shifting by 22 bits (only 10 bits of precision left for a 32-bit
 * int!).
 *
 * The following list shows all possible ways in which integers and
 * fixed-point numbers can be used with the four basic math operators,
 * and whether the results will be fixed-point, integer, or meaningless:
 * 
 * fixed / fixed 	= int
 * fixed / int		= fixed
 * int / fixed		= meaningless
 *
 * fixed * fixed	= meaningless
 * fixed * int		= fixed
 *
 * fixed + int		= meaningless
 * fixed + fixed	= fixed
 *
 * fixed - int		= meaningless
 * int - fixed		= meaningless
 * fixed - fixed	= fixed
 */

#define FIXEDPOINT_INT_PART_BITS 11
#define FIXED_TO_INT(ff) ((ff) >> FIXEDPOINT_INT_PART_BITS)
#define INT_TO_FIXED(ii) ((ii) << FIXEDPOINT_INT_PART_BITS)
#define IP_FIXED_TO_INT(ff) (ff) >>= FIXEDPOINT_INT_PART_BITS
#define IP_INT_TO_FIXED(ii) (ii) <<= FIXEDPOINT_INT_PART_BITS
#define FRACTIONAL_MASK ((1 << FIXEDPOINT_INT_PART_BITS)-1)
#define FIXEDPOINT_FLOOR(number) ((number) & (~0 << FIXEDPOINT_INT_PART_BITS))
#define COSINE_LEN (1 << FIXEDPOINT_INT_PART_BITS)

#endif
