#include "rational_numbers.h"

rational fraction(integer numerator, integer denominator)
{
	rational q = malloc (2 * sizeof(integer));

	q[0] = numerator;
	q[1] = denominator;

	return (q);
}

rational itor(integer number)
{
	return (fraction(number, 1));
}

rational simplify(rational q)
{
	integer div = gcd(q[0], q[1]);

	q[0] /= div;
	q[1] /= div;
	if (q[1] < 0)
	{
		q[0] *= -1;
		q[1] *= -1;
	}

	return (q);
}

rational Q_sum(rational q, rational p)
{
	integer		mult;
	rational	result;

	if (q[1] == p[1])
		return (fraction(q[0] + p[0], p[1]));
	mult = lcm(p[1], q[1]);
	result = fraction(q[0] * (mult / q[1]) + p[0] * (mult / p[1]), mult);

	return (simplify(result));
}

rational Q_subs(rational q, rational p)
{
	rational result;

	p[0] *= -1;
	result = Q_sum(q,p);
	p[0] *= -1;

	return (result);
}

rational Q_prod(rational q, rational p)
{
	return(simplify(fraction(q[0] * p[0], q[1] * p[1])));
}

rational Q_div(rational a, rational b)
{
	rational result;
	rational inverse;

	inverse = fraction(b[1], b[0]);
	result = Q_prod(a, inverse);
	free(inverse);

	return (result);
}

void print_rational(rational q)
{
	if (q[1] == 1)
		printf("%d", q[0]);
	else
		printf("%d/%d", q[0], q[1]);
}
