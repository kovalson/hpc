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

#define	NUM_THREADS		2
#define BITS_NUM_MAX	40
#define BITS_NUM_MIN	20
#define	k				20
#define	KANGAROO_TAME	0
#define	KANGAROO_WILD	1
#define LAST_BITS_NUM	1
using namespace std;

class DPoint
{
public:
	int tag;
	int idx;
	ZZ distance;
	ZZ start;
	ZZ start_offset;
};

/**
 * Sprawdzanie, czy podana liczba jest "rozróżnialna".
 * W tym przypadku funkcja sprawdza, czy liczba bitów potrzebnych
 * do zapisu podanej liczby mieści się w pewnym zdefiniowanym przedziale.
 * 
 * @param	[ZZ] num	Liczba do sprawdzenia
 * @return	[bool] 		true jeśli się mieści, false w przeciwnym przypadku
 */
bool is_distinguished( ZZ num )
{
	long num_bits = NumBits( num );
	return (num_bits > BITS_NUM_MIN && num_bits < BITS_NUM_MAX);
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
	bool found_flag = false;
	int idx, tag;
	ZZ v, pos, d, start, exponent;
	v = ((beta( a, b ) / (NUM_THREADS / 2)));
	unordered_map<long long, DPoint> dpoints;
	DPoint dpoint;

	#pragma omp parallel private( pos, d, idx, tag, start, dpoint ) num_threads( NUM_THREADS )
	{
		// Pobierz id wątku
		idx = omp_get_thread_num();

		// Wyznacz typ kangura
		tag = (idx % 2 == 0) ? KANGAROO_TAME : KANGAROO_WILD;

		// Dopuszczaj tylko indeksy 0, 1 dla obu typów kangurów
		idx /= 2;

		// Początkowe położenie kangurów
		if( tag == KANGAROO_TAME )
		{
			start = ((a + b) / 2) + (idx * v);
			pos = PowerMod( g, start, p );
		}
		else
		{
			start = (idx * v);
			pos = MulMod( h, PowerMod( g, start, p ), p );
		}

		// Początkowa całkowita długość podróży kangura
		d = idx * v;

		// W nieskończoność
		while( true )
		{
			// Zwiększ dystans i wykonaj skok
			d += f( pos );
			pos = MulMod( pos, PowerMod( g, f( pos ), p ), p );

			// Jeżeli znaleziono już rozwiązanie - przerwij
			if( found_flag )
				break;

			// Sekcja krytyczna punktów rozróżnialnych
			#pragma omp critical
			{
				if( is_distinguished( pos ) )
				{
					// Stwórz obiekt punktu rozróżnialnego
					dpoint.tag = tag;
					dpoint.distance = d;
					dpoint.idx = idx;
					dpoint.start = start;
					dpoint.start_offset = idx * v;

					// Konwersja pozycji na long
					long at = conv<long>( pos );

					// Jeśli taki punkt jeszcze nie istnieje - dopisz do zbioru
					if( dpoints.find( at ) == dpoints.end() )
					{
						dpoints[ at ] = dpoint;
					}

					// A jeśli istnieje, to występuje kolizja
					else
					{
						// Interesuje nas kolizja dwóch różnych typów kangurów (wild vs. tame)
						if( dpoints[ at ].tag != tag && !found_flag && dpoints[ at ].idx == idx )
						{
							ZZ tame_d, tame_idx, tame_start, tame_start_offset;
							ZZ wild_d, wild_idx, wild_start, wild_start_offset;

							if( tag == KANGAROO_WILD )
							{
								wild_d = d;
								wild_idx = idx;
								wild_start = start;
								wild_start_offset = idx * v;
								tame_d = dpoints[ at ].distance;
								tame_idx = dpoints[ at ].idx;
								tame_start = dpoints[ at ].start;
								tame_start_offset = dpoints[ at ].start_offset;
							}
							else
							{
								tame_d = d;
								tame_idx = idx;
								tame_start = start;
								tame_start_offset = idx * v;
								wild_d = dpoints[ at ].distance;
								wild_idx = dpoints[ at ].idx;
								wild_start = dpoints[ at ].start;
								wild_start_offset = dpoints[ at ].start_offset;
							}

							ZZ modulus = (p - 1) / 2;
							// ZZ x = (((a + b) / 2) % modulus) + (tame_d % modulus) - (wild_d % modulus);
							// ZZ x = ((a + b) / 2) - tame_start_offset + tame_d - wild_d + wild_start_offset;
							ZZ x = ((((a + b) / 2))) + (tame_d) - (wild_d);

							cout << x % modulus << endl;
							// cout << "at: " << at << endl;
							// cout << "x = " << x << endl;
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
	
	// test( p, g, h, a );
	find_x( p, g, h, a, b );

	return 0;
}