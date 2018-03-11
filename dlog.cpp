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

/*
 	Drugi przykład z laboratorium:
		h = 473567883536982
		g = 4
		p = 1263154214185307

	Trzeci przykład z laboratorium:
		h = 262635754439740
*/

NTL_CLIENT

ZZ N = (ZZ) 23;
ZZ n = (N - 1) / 2;

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

void find_x( ZZ v1, ZZ v2 )
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
			ZZ r = (b - B) % n;
			if( r == 0 )
			{
				cout << "Nie znaleziono rozwiazania!" << endl;
			}
			else
			{
				cout << "x = " << InvMod( r, n ) * (A - a) % n << endl;
			}
			break;
		}
	}

}

int main()
{
	ZZ a, b, x;

	cout << "Podaj p (strong-prime): ";
	cin >> N;
	cout << "Podaj g (generator): ";
	cin >> a;
	cout << "Podaj h (element): ";
	cin >> b;

	n = (N - 1) / 2;
	cout << "q = " << n << endl << endl;

	find_x( a, b );

	return 0;
}