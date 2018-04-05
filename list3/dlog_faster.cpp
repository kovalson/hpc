/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 3
 */

/*
 	Drugi przykład z laboratorium:
		p = 1263154214185307
		g = 4
		h = 473567883536982

	Trzeci przykład z laboratorium:
		h = 262635754439740
*/

#include <omp.h>
#include <NTL/ZZ.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include <time.h>

#define NO_ERRORS		0
#define THREADS_NUM		4

using namespace std;
NTL_CLIENT

int BITS_NUM_MIN	= 41;
int BITS_NUM_MAX	= 41;

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
	return num_bits == BITS_NUM_MAX;
}

bool add_distinguished( vector<DPoint> *distinguished, DPoint point, ZZ g, ZZ h, ZZ n )
{
	// Sprawdź, czy jest już taki punkt
	for( int i = 0; i < distinguished->size(); i++ )
	{
		if( distinguished->at( i ).x == point.x )
		{
			// Oblicz logarytm
			// d - b
			ZZ d_min_b = (point.b - distinguished->at( i ).b) % n;
			if( d_min_b == 0 )
			{
				return false;
				// cout << "Nie ma rozwiązania..." << endl;
			}
			else
			{
				cout << "x = " << InvMod( d_min_b, n ) * (distinguished->at( i ).a - point.a) % n << endl;
				return true;
			}
		}
	}

	distinguished->push_back( point );

	// Zwróć informację, że nie było kolizji
	return false;
}

bool find_x( ZZ p, ZZ g, ZZ h )
{
	ZZ q;
	ZZ x, a, b;
	ZZ X, A, B;
	ZZ seed;

	int counter = 0;
	int tid;

	// Obliczenie q
	q = (p - 1) / 2;

	// Ustaw seed do generowania losowych liczb
	long int t = time( NULL );
	conv( seed, t );
	SetSeed( seed );

	// Wektor punktów rozróżnialnych
	vector<DPoint> distinguished;

	// Flaga informująca o skończeniu obliczeń (znalezieniu dloga)
	bool flag = false;

	int max = 100000;

	#pragma omp parallel for private( x, a, b, X, A, B, tid ) firstprivate( counter ) shared( distinguished, flag ) num_threads( THREADS_NUM )
	for( int i = 0; i < max; i++)
	{

		// Jeśli znaleziono logarytm - skończ iteracje
		if( flag ) continue;

		// Sekcja krytyczna losowania liczb, bo inaczej
		// świat płonie i pożoga narusza pamięć
		#pragma omp critical
		{
			a = RandomBnd( p );
			b = RandomBnd( p );
		}

		// Obliczenie x
		x = PowerMod( g, a, p ) * PowerMod( h, b, p );

		// Zwiększanie wartości jak w zadaniu 1.
		X = x;
		A = a;
		B = b;

		for( int n = 0; n < 1000; n++ )
		{
			cycle_vals( x, a, b, g, h, p, q );

			cycle_vals( X, A, B, g, h, p, q );
			cycle_vals( X, A, B, g, h, p, q );

			// Jeśli znaleziono logarytm - przerwij tę pętlę
			if( flag ) break;

			// Jeżeli wyliczony punkt jest rozróżnialny
			if( is_distinguished( x ) )
			{
				counter = 0;
				DPoint dpoint;
				dpoint.x = x;
				dpoint.a = a;
				dpoint.b = b;

				#pragma omp critical
				{
					if( add_distinguished( &distinguished, dpoint, g, h, q ) )
					{
						flag = true;
					}
				}
			}


			if( x == X )
			{
				ZZ d_min_b = (B - b) % q;
				if( d_min_b == 0 )
				{
					// cout << "Nie ma rozwiązania..." << endl;
				}
				else
				{
					// Znaleziono rozwiązanie
					flag = true;

					cout << "x = " << InvMod( d_min_b, q ) * (a - A) % q << endl;
				}
			}

		}

	}

	if( flag ) return true;
	return find_x( p, g, h );

}

int main( int argc, char *argv[] )
{
	
	ZZ p, g, h;

	if( argc > 1 )
	{
		p = 1263154214185307;
		g = 4;
		int ver = atoi( argv[ 1 ] );
		if( ver == 1 ) h = 473567883536982;
		else h = 262635754439740;
	}
	else
	{
		cout << "Podaj p (strong-prime): ";
		cin >> p;

		cout << "Podaj g (generator): ";
		cin >> g;

		cout << "Podaj h (element): ";
		cin >> h;
	}

	// Ustaw liczbę bitów, które muszą być równe 0
	// N_BITS = sqrt( NumBits( p ) );
	// cout << N_BITS << endl;

	// Znajdź x takie, że g^x = h
	find_x( p, g, h );

	// Jeśli brak błedów, to brak błędów
	return NO_ERRORS;

}