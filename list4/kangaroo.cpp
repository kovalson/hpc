/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 4
 */

#include <omp.h>
#include <NTL/ZZ.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
NTL_CLIENT;

#define	NUM_THREADS		4
#define BITS_NUM_MAX	32
#define	k				20
#define	KANGAROO_TAME	0
#define	KANGAROO_WILD	1
#define LAST_BITS_NUM	4
using namespace std;

class DPoint
{
public:
	int type;
	int idx;
	ZZ distance;
};

// long k = 20;

// Sprawdzanie, czy liczba jest "rozróżnialna" (distinguished)
bool is_distinguished( ZZ num )
{
	// Sprawdź, czy liczba bitów potrzebnych
	// do zapisu podanej wartości jest mniejsza niż
	// pewna stała BITS_NUM
	long num_bits = NumBits( num );
	return num_bits < BITS_NUM_MAX;
}

bool is_distinguished2( ZZ num )
{
	// Sprawdź czy ostatnie LAST_BITS_NUM
	// jest 0
	long b = 0;
	while( b < LAST_BITS_NUM )
	{
		if( bit( num, b ) != 0 )
			return false;
		b++;
	}
	return true;
}

ZZ beta( ZZ a, ZZ b )
{
	return ((NUM_THREADS * b) - (a / 4));
}

ZZ f( ZZ x )
{
	return power2_ZZ( x % k );
}

void find_x( ZZ p, ZZ g, ZZ h, ZZ a, ZZ b )
{
	long N = conv<long>( (f( (ZZ) 0 ) + f( b )) / 2 * 2 );
	bool found_flag = false;
	int idx, type;
	ZZ v, pos, d;
	v = ((beta( a, b ) / (NUM_THREADS / 2)));
	unordered_map<long, DPoint> dpoints;

	#pragma omp parallel num_threads( NUM_THREADS ) private( pos, d, idx, type ) shared( p, v, dpoints, found_flag )
	{
		int idx = omp_get_thread_num();
		int type;
		ZZ pos, d;

		// Wyznaczanie typu kangura
		if( idx % 2 == 0 )
			type = KANGAROO_TAME;
		else
			type = KANGAROO_WILD;

		// Dopuszczaj tylko indeksy 0, 1 dla obu typów kangurów
		idx /= 2;

		// Początkowy dystans i położenie kangura
		d = idx * v;
		if( type == KANGAROO_TAME )
			pos = PowerMod( g, ((a + b) / 2) + (idx * v), p );
		else
			pos = MulMod( h, PowerMod( g, idx * v, p ), p );

		// W nieskończoność
		while( true )
		{
			// Jeżeli znaleziono już rozwiązanie - przerwij
			if( found_flag )
				break;

			// Wykonaj kolejny skok i zaktualizuj dystans
			pos = MulMod( pos, PowerMod( g, f( pos ), p ), p );
			d += f( pos );

			// Sekcja krytyczna punktów rozróżnialnych
			#pragma omp critical
			{
				if( is_distinguished2( pos ) )
				{
					DPoint dpoint;
					dpoint.type = type;
					dpoint.distance = d;
					dpoint.idx = idx;
					long at = conv<long>( pos );

					// Jeśli taki punkt jeszcze nie istnieje - dopisz do zbioru
					if( dpoints.find( at ) == dpoints.end() )
					{
						dpoints[ conv<long>( pos ) ] = dpoint;
					}

					// A jeśli istnieje, to występuje kolizja
					else
					{
						// Interesuje nas kolizja dwóch różnych typów kangurów (wild vs. tame)
						if( type != dpoints[ at ].type && !found_flag )
						{
							ZZ tame_d, tame_idx;
							ZZ wild_d, wild_idx;

							if( type == KANGAROO_WILD )
							{
								wild_d = d;
								wild_idx = idx;
								tame_d = dpoints[ at ].distance;
								tame_idx = dpoints[ at ].idx;
							}
							else
							{
								tame_d = d;
								tame_idx = idx;
								wild_d = dpoints[ at ].distance;
								wild_idx = dpoints[ at ].idx;
							}

							ZZ x = ((a + b) / 2) + tame_d - wild_d;

							cout << "x = " << x << endl;
							found_flag = true;
						}
					}
				}
			}
		}
	}
}

int main()
{
	ZZ p, g, h, a, b, x;
	cout << "Podaj p, g, h, a, b: ";
	cin >> p >> g >> h >> a >> b;
	
	find_x( p, g, h, a, b );

	return 0;
}