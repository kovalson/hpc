/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 3
 */

#include <omp.h>
#include <NTL/ZZ.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <time.h>

#define NO_ERRORS		0
#define THREADS_NUM		2
#define BITS_NUM		11

using namespace std;
NTL_CLIENT

// Klasa punktu rozróżnialnego
class DPoint
{
public:
	ZZ x;
	ZZ a;
	ZZ b;

	bool equals( DPoint point );
};

bool DPoint::equals( DPoint point )
{
	// Sprawdź, czy wszystkie atrybuty mają taką samą wartość
	return (
		this->x == point.x &&
		this->a == point.a &&
		this->b == point.b
	);
}

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
	return NumBits( num ) < BITS_NUM;
}

bool add_distinguished( vector<DPoint> *distinguished, DPoint point, ZZ g, ZZ h, ZZ n )
{
	// Sprawdź, czy jest już taki punkt
	for( int i = 0; i < distinguished->size(); i++ )
	{
		// Jeśli jest
		if( distinguished->at( i ).x == point.x &&
			(distinguished->at( i ).a != point.a ||
			 distinguished->at( i ).b != point.b )
		)
		{
			// Oblicz logarytm
			// d - b
			ZZ d_min_b = (point.b - distinguished->at( i ).b) % n;
			if( d_min_b == 0 )
			{
				cout << "Nie ma rozwiązania..." << endl;
			}
			else
			{
				cout << "x = " << InvMod( d_min_b, n ) * (distinguished->at( i ).a - point.a) % n << endl;
			}


			// Zwróć informację, że była kolizja
			return true;
		}
	}

	// Jak nie ma kolizji - dodaj punkt
	distinguished->push_back( point );

	// Zwróć informację, że nie było kolizji
	return false;
}

void find_x( ZZ p, ZZ g, ZZ h )
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

	#pragma omp parallel for private( x, a, b, X, A, B, tid ) firstprivate( counter ) shared( distinguished, flag ) num_threads( THREADS_NUM )
	for( int i = 0; i < 100000; i++ )
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

		ZZ n;
		for( n = 0; n < q; n++ )
		{
			cycle_vals( x, a, b, g, h, p, q );

			cycle_vals( X, A, B, g, h, p, q );
			cycle_vals( X, A, B, g, h, p, q );


			// Jeżeli wyliczony punkt jest rozróżnialny
			if( is_distinguished( x ) )
			{
				DPoint dpoint;
				dpoint.x = x;
				dpoint.a = a;
				dpoint.b = b;

				// Jeśli znaleziono logarytm - przerwij tę pętlę
				if( flag ) break;

				#pragma omp critical
				{
					if( add_distinguished( &distinguished, dpoint, g, h, q ) )
					{
						flag = true;
					}
				}
			}
		}

	}

}

int main()
{
	
	ZZ p, g, h;

	cout << "Podaj p (strong-prime): ";
	cin >> p;

	cout << "Podaj g (generator): ";
	cin >> g;

	cout << "Podaj h (element): ";
	cin >> h;

	// Znajdź x takie, że g^x = h
	find_x( p, g, h );

	// Jeśli brak błedów, to brak błędów
	return NO_ERRORS;

}