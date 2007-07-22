#include "bigint.h"

bigint::bigint()
{
	mpz_init(value);
}

bigint::~bigint()
{
	mpz_clear(value);
}

bigint::bigint(const bigint &rhs)
{
	mpz_init(value);
	// Using operator= causes infinite
	// recursion.
	mpz_set(value, rhs.value);
}

bigint
bigint::operator=(bigint rhs)
{
	mpz_set(value, rhs.value);
	return *this;
}

bigint
bigint::operator=(unsigned long rhs)
{
	mpz_set_ui(value, rhs);
	return *this;
}

bigint
bigint::operator=(size_t rhs)
{
	mpz_set_ui(value, rhs);
	return *this;
}

bigint
bigint::operator=(signed long rhs)
{
	mpz_set_si(value, rhs);
	return *this;
} 

bigint
bigint::operator+=(bigint rhs)
{
	mpz_add(value, value, rhs.value);
	return *this;
}

bigint
bigint::operator+=(size_t rhs)
{
	mpz_add_ui(value, value, rhs);
	return *this;
}

 
bigint
bigint::operator+=(signed long rhs)
{
	return (*this) + bigint(rhs);
}


bigint
bigint::operator-=(bigint rhs)
{
	mpz_sub(value, value, rhs.value);
	return *this;
}

bigint
bigint::operator-=(size_t rhs)
{
	mpz_sub_ui(value, value, rhs);
	return *this;
}


bigint
bigint::operator-=(signed long rhs)
{
	return (*this) + bigint(rhs);
}


bigint
bigint::operator*=(bigint rhs)
{
	mpz_mul(value, value, rhs.value);
	return *this;
}

bigint
bigint::operator*=(size_t rhs)
{
	mpz_mul_ui(value, value, rhs);
	return *this;
}


bigint
bigint::operator*=(signed long rhs)
{
	mpz_mul_si(value, value, rhs);
	return *this;
}


bigint
bigint::operator/=(bigint rhs)
{
	mpz_tdiv_q(value, value, rhs.value);
	return *this;
}

bigint
bigint::operator/=(size_t rhs)
{
	mpz_tdiv_ui(value, rhs);
	return *this;
}

bigint
bigint::operator /(bigint rhs)
{
	bigint retval;
	mpz_tdiv_q(retval.value, value, rhs.value);
	return retval;
}

// No templates because then the compiler will be too
// stupid to do the right thing for bigint / bigint
// divisions.

bigint
bigint::operator /(size_t rhs)
{
	return *this / bigint(rhs);
}

bigint
bigint::operator /(signed long rhs)
{
	return *this / bigint(rhs);
}

bigint
bigint::operator % (bigint rhs)
{
	bigint retval;
	mpz_mod(retval.value, value, rhs.value);
	return retval;
}

bigint
bigint::operator %= (bigint rhs)
{
	return (*this = *this % rhs);
}


bool
bigint::operator> (bigint rhs) const
{
	return mpz_cmp(value, rhs.value) > 0;
}

bool
bigint::operator == (bigint rhs) const
{
	return mpz_cmp(value, rhs.value) == 0;
}

bool
bigint::operator< (bigint rhs) const
{
	return mpz_cmp(value, rhs.value) < 0;
}

bool
bigint::operator >= (bigint rhs) const
{
	return mpz_cmp(value, rhs.value) >= 0;
}

bool
bigint::operator <= (bigint rhs) const
{
	return mpz_cmp(value, rhs.value) <= 0;
}

bigint &
bigint::operator++ (int nothing)
{
	*this += (size_t)1;
	return *this;
}

bigint &
bigint::operator-- (int nothing)
{
	*this -= (size_t)1;
	return *this;
}
