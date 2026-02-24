#include "rational_numbers.h"

integer gcd(integer a, integer b)
{
	integer c;

	do
	{
		c = a % b;
		a = b;
		b = c;
	} while (b);

	return (a);
}

integer lcm(integer a, integer b)
{
	return (a / gcd(a,b) * b);
}