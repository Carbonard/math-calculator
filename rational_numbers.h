#ifndef RATIONALS_H
# define RATIONALS_H

# include <stdio.h>
# include <stdlib.h>

typedef int integer;
typedef integer* rational;

// Integers
integer gcd(integer a, integer b);
integer lcm(integer a, integer b);
// Rationals
rational fraction(integer numerator, integer denominator);
rational itor(integer number);
rational simplify(rational q);
rational Q_sum(rational q, rational p);
rational Q_subs(rational q, rational p);
rational Q_prod(rational q, rational p);
rational Q_div(rational a, rational b);
void print_rational(rational q);

#endif