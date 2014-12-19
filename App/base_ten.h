#pragma once
#include <algorithm>
#include <boost/regex.hpp>
template <class intType>
class num_b10
{
public:
	num_b10(void) : base(0), exponent(0)
	{

	}
	num_b10(intType base, intType exponent) : base(base), exponent(exponent)
	{
		while (this->base / 10 * 10 == this->base && this->base != 0)
		{
			this->base /= 10;
			this->exponent++;
		}
	}
	num_b10(long double input) : num_b10(0, 0)
	{
		while (input != std::floor(input))
		{
			this->exponent--;
			input *= 10;
		}
		this->base = (intType)std::floor(input);

		while (this->base / 10 * 10 != this->base && this->base != 0)
		{
			this->base /= 10;
			this->exponent++;
		}
	}
	~num_b10()
	{

	}

	num_b10 operator -(void) const
	{
		return num_b10(-this->base, this->exponent);
	}

	num_b10 operator +(num_b10 arg0) const
	{
		intType newBase = (intType)pow(10, abs(arg0.exponent - this->exponent));
		intType newExponent = 0;
		if (arg0.exponent > this->exponent)
		{
			newBase *= arg0.base;
			newBase += this->base;
			newExponent = this->exponent;
		}
		else
		{
			newBase *= this->base;
			newBase += arg0.base;
			newExponent = arg0.exponent;
		}
		return num_b10(newBase, newExponent);
	}
	num_b10 operator -(num_b10 arg0) const
	{
		return *this + (-arg0);
	}
	num_b10 operator *(num_b10 arg0) const
	{
		return num_b10(arg0.base * this->base, arg0.exponent + this->exponent);
	}
	num_b10 operator /(num_b10 arg0) const
	{
		return num_b10(arg0.base / this->base, this->exponent - arg0.exponent);
	}

	num_b10& operator +=(const num_b10& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	num_b10& operator -=(const num_b10& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	num_b10& operator *=(const num_b10& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	num_b10& operator /=(const num_b10& rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	operator long double() const
	{
		long double base = (long double)this->base;
		long double exponent = pow(10.0l, this->exponent);
		return base * exponent;
	}

	long long asExponent(intType arg0) const
	{
		return (long long)std::floor(this->base * pow(10.0l, this->exponent - arg0));
	}
	intType numerator(void)const
	{
		return this->base;
	}
	intType denominator(void) const
	{
		return (intType)pow(10, this->exponent);
	}
private:
	intType base;
	intType exponent;
};


template <class intType>
num_b10<intType> stor(std::string input)
{
	intType base = 0;
	intType exponent = 0;
	boost::smatch results;
	boost::regex numberParse("(\\+|\\-)?(\\.)?([\\d]+)((\\.)([\\d]*))?");

	boost::regex_match(input, results, numberParse);

	if (results[2].str() == ".")
	{

		base = std::stoi(results[3].str());
		exponent = -(int)results[3].str().size();
	}
	else
	{
		base = std::stoi(results[3].str());

		if (results[6].str().size() > 0)
		{
			intType powerOfTen = (intType)pow(10, results[6].str().size());
			base *= powerOfTen;
			base += std::stoi(results[6]);
			exponent = -(int)results[6].str().size();
		}
	}



	if (results[1].str() == "-")
		base *= -1;

	return num_b10<intType>(base, exponent);
}