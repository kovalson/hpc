/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Algorytm Pollard-rho
 * Dane wejściowe:
 * 		a	- generator grupy G
 * 		b	- element grupy G
 * Dane wyjściowe:
 * 		x	- taki, źe a^x = b
 */

#include <NTL/ZZ.h>
#include <stdio.h>
#include <iostream>

NTL_CLIENT

const ZZ n = (ZZ) 1018;
const ZZ N = n + 1;

void cycle_vals( ZZ &x, ZZ &a, ZZ &b, ZZ v1, ZZ v2 )
{
	switch( x % 3 )
	{
		case 0:
			x = x * x % N;
			a = a * 2 % n;
			b = b * 2 % n;
			break;

		case 1:
			x = x * v1 % N;
			a = (a + 1) % n;
			break;

		case 2:
			x = x * v2 % N;
			b = (b + 1) % n;
			break;
	}
}

ZZ find_x( ZZ v1, ZZ v2 )
{
	ZZ x, a, b;
	ZZ X, A, B;
	ZZ i;

	x = 1; a = 0; b = 0;
	X = x; A = a; B = b;

	for( i = 0; i < n; i++ )
	{
		cycle_vals( x, a, b, v1, v2 );

		// Wartości X, A, B rosną dwa razy szybciej
		cycle_vals( X, A, B, v1, v2 );
		cycle_vals( X, A, B, v1, v2 );

		// Skończ przebieg, gdy wystąpi kolizja
		if( x == X )
		{
			break;
		}
	}

	return (a - A) / (B - b);
}

int main()
{
	ZZ a, b, x;

	cout << "Podaj a (generator):" << endl;
	cin >> a;
	cout << "Podaj b (element):" << endl;
	cin >> b;

	x = find_x( a, b );
	cout << x << endl;

	return 0;
}