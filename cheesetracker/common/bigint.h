#ifndef BIGINT_H
#define BIGINT_H
#include <cassert>
#include <gmp.h>

class bigint
{
	public:
		bigint();
		template<class T> bigint(T src) {
			mpz_init(value);
			*this = src;
		}
		inline operator unsigned long() const {
			return mpz_get_ui(value);
		}
		inline operator size_t() const {
			assert(*this >= bigint((size_t)0));
			return mpz_get_ui(value);
		}
		inline operator signed long() const {
			return mpz_get_si(value);
		}
		bigint(const bigint &rhs);
		~bigint();
		bigint operator=(bigint rhs);
		bigint operator=(unsigned long rhs);
		bigint operator=(size_t rhs);
		bigint operator=(signed long rhs);
		bigint operator += (bigint rhs);
		bigint operator += (size_t rhs);
		bigint operator += (signed long rhs);

		bigint& operator++(int nothing);
		bigint & operator--(int nothing);

		template <class T>
		bigint operator + (T rhs)
		{
			bigint retval;
			retval = *this;
			retval += rhs;
			return retval;
		}


		template <class T>
		bigint operator - (T rhs)
		{
			bigint retval;
			retval = *this;
			retval -= rhs;
			return retval;
		}

		template <class T>
		bigint operator * (T rhs)
		{
			bigint retval;
			retval = *this;
			retval *= rhs;
			return retval;
		}

		bigint operator -= (bigint rhs);
		bigint operator -= (size_t rhs);
		bigint operator -= (signed long rhs);

		bigint operator *= (bigint rhs);
		bigint operator *= (size_t rhs);
		bigint operator *= (signed long rhs);

		bigint operator /= (bigint rhs);
		bigint operator/=(size_t rhs);
		bigint operator /(bigint rhs);
		bigint operator /(size_t rhs);
		bigint operator /(signed long rhs);

		bigint operator % (bigint rhs);
		bigint operator %= (bigint rhs);

		bool operator>(bigint rhs) const;
		bool operator == (bigint rhs) const;
		bool operator<(bigint rhs) const;

		bool operator >= (bigint rhs) const;
		bool operator <= (bigint rhs) const;
	private:
		mpz_t value;
};

#endif
