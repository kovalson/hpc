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

void cycle_vals( ZZ &x, ZZ &a, ZZ &b, ZZ v1, ZZ v2, ZZ p, ZZ q )
{
	switch( x % 3 )
	{
		case 0:
			x = x * x % p;
			a = a * 2 % q;
			b = b * 2 % q;
			break;

		case 1:
			x = x * v1 % p;
			a = (a + 1) % q;
			break;

		case 2:
			x = x * v2 % p;
			b = (b + 1) % q;
			break;
	}
}

ZZ find_x( ZZ p, ZZ q, ZZ g, ZZ h )
{
	if( h == (ZZ) 1 )
		return (ZZ) 0;
	if( g == h )
		return (ZZ) 1;
	
	ZZ x, a, b;
	ZZ X, A, B;
	ZZ i;

	x = 1; a = 0; b = 0;
	X = x; A = a; B = b;

	for( i = 0; i < q; i++ )
	{
		cycle_vals( x, a, b, g, h, p, q );

		// Wartości X, A, B rosną dwa razy szybciej
		cycle_vals( X, A, B, g, h, p, q );
		cycle_vals( X, A, B, g, h, p, q );

		// Skończ przebieg, gdy wystąpi kolizja
		if( x == X )
		{
			ZZ r = (b - B) % q;
			if( r != 0 )
				return InvMod( r, q ) * (A - a) % q;
			break;
		}
	}

	cout << "Brak rozwiązania!" << endl;
	exit( 1 );
	// return (ZZ) -1;
}

/*
int main()
{
	ZZ p, q, g, h, x;

	cout << "Podaj p (strong-prime): ";
	cin >> p;
	cout << "Podaj q: ";
	cin >> q;
	cout << "Podaj g (generator): ";
	cin >> g;
	cout << "Podaj h (element): ";
	cin >> h;

	// ZZ q = (p - 1) / 2;
	x = find_x( p, q, g, h );

	cout << "x = " << x << endl;
	return 0;
}
*/