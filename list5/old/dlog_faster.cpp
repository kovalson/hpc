/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 3
 */

#include <omp.h>
#include <NTL/ZZ.h>
#include <iostream>
#include <unordered_map>

#define NO_ERRORS		0
#define THREADS_NUM		4
#define BITS_NUM_MAX	256

using namespace std;
NTL_CLIENT

// Klasa punktu rozróżnialnego
class DPoint
{
public:
	ZZ x;
	ZZ a;
	ZZ b;
};

// Funkcja z zadania 1.
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

// Sprawdzanie, czy liczba jest "rozróżnialna" (distinguished)
bool is_distinguished( ZZ num )
{
	// Sprawdź, czy liczba bitów potrzebnych
	// do zapisu podanej wartości jest mniejsza niż
	// pewna stała BITS_NUM
	long num_bits = NumBits( num );
	return num_bits < BITS_NUM_MAX;
}

ZZ pollard_rho( ZZ p, ZZ g, ZZ h )
{
	ZZ q;
	ZZ x, a, b;
	ZZ X, A, B;
	ZZ seed;
	ZZ final_x = conv <ZZ> (-1);
	int tid;

	// Obliczenie q
	q = (p - 1) / 2;

	// Ustaw seed do generowania losowych liczb
	long int t = time( NULL );
	conv( seed, t );
	SetSeed( seed );

	// Wektor punktów rozróżnialnych
	// vector<DPoint> distinguished;
	unordered_map <long, DPoint> distinguished;

	// Flaga informująca o skończeniu obliczeń (znalezieniu dloga)
	bool flag = false;

	#pragma omp parallel private( x, a, b, X, A, B, tid ) shared( distinguished, flag, final_x ) num_threads( THREADS_NUM )
	{
		a = RandomBnd( p );
		b = RandomBnd( p );
		x = PowerMod( g, a, p ) * PowerMod( h, b, p );

		X = x;
		A = a;
		B = b;

		while( true )
		{
			cycle_vals( x, a, b, g, h, p, q );

			cycle_vals( X, A, B, g, h, p, q );
			cycle_vals( X, A, B, g, h, p, q );

			// Jeśli znaleziono logarytm - przerwij tę pętlę
			if( flag )
				break;

			// Skończ przebieg, gdy wystąpi kolizja
			if( x == X )
			{
				ZZ r = (b - B) % q;
				if( r != 0 )
				{
					final_x = InvMod( r, q ) * (A - a) % q;
					flag = true;
				}
				break;
			}

			// Jeżeli wyliczony punkt jest rozróżnialny
			if( !is_distinguished( x ) )
				continue;

			DPoint dpoint;
			dpoint.x = x;
			dpoint.a = a;
			dpoint.b = b;

			#pragma omp critical
			{
				long at = conv <long> ( x );
				if( distinguished.find( at ) == distinguished.end() )
				{
					distinguished[ at ] = dpoint;
				}
				else
				{
					ZZ d_min_b = (dpoint.b - distinguished[ at ].b) % q;
					if( d_min_b != 0 )
					{
						final_x = InvMod( d_min_b, q ) * (distinguished[ at ].a - dpoint.a) % q;
						flag = true;
					}
				}
			}

			a = RandomBnd( p );
			b = RandomBnd( p );
			x = PowerMod( g, a, p ) * PowerMod( h, b, p );

			X = x;
			A = a;
			B = b;
		}
	}

	return final_x;
}

/*
int main( int argc, char *argv[] )
{
	ZZ p, g, h;

	cout << "p g h" << endl;
	cin >> p >> g >> h;

	// Znajdź x takie, że g^x = h
	ZZ x = find_x( p, g, h );
	cout << "x = " << x << endl;

	// Jeśli brak błedów, to brak błędów
	return NO_ERRORS;
}
*/