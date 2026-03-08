#include "../includes/calculator.h"

integer gcd(integer a, integer b)
{
	integer c;

	if (!a || !b)
		return (1);
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

integer	int_sqrt(integer n)
{
	integer s = 0;

	if (n < 0)
		return (-1);
	while (s < n)
	{
		if (s*s == n)
			return (s);
		s++;
	}
	return (-1);
}
