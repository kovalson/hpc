/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Obliczanie logarytmu dyskretnego przy użyciu
 * metody lambda (kangaroo) równolegle (OpenMP)
 * i dla dużych liczb (NTL).
 *
 * Dane wejściowe:
 * 	p	- grupa Z_p, p - liczba pierwsza
 * 	q	- rząd grupy (zakładamy q = (p - 1) / 2)
 * 	g	- generator grupy
 * 	h	- element grupy
 *
 * Dane wyjściowe (szukane):
 * 	x	- taki, że g^x = h
 */

#include <omp.h>
#include <NTL/ZZ.h>
#include <iostream>
#include <unordered_map>
#include "DPoint.cpp"
NTL_CLIENT

#define	NUM_THREADS		2	// Liczba dostępnych procesorów
#define	K				20	// Liczba różnych skoków
#define BITS_NUM_MIN	0	// Minimalna liczba bitów zapisu liczby rozróżnialnej
#define BITS_NUM_MAX	50	// Maksymalna liczba bitów zapisu liczby rozróżnialnej
#define KANGAROO_TAME	0	// Tag kangura domowego
#define KANGAROO_WILD	1	// Tag kangura dzikiego
using namespace std;

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

/**
 * Pewna wartość beta zależna od liczby dostępnych procesorów
 * i podanego zakresu szukanego logarytmu [a, b]

 * @param  [ZZ] a	Dolny zakres szukanego logarytmu
 * @param  [ZZ] b	Górny zakres szukanego logarytmu
 * @return [ZZ]		Wartość beta
 */
ZZ beta( ZZ a, ZZ b )
{
	return ((NUM_THREADS * b) - (a / 4));
}

/**
 * Funkcja f:G -> S, gdzie S to zbiór rozmiarów skoków kangura,
 * w tym przypadku - potęgi dwójki
 * 
 * @param	[ZZ] pos	Pozycja kangura
 * @return	[ZZ]		Wartość skoku
 */
ZZ f( ZZ pos )
{
	return power2_ZZ( pos % K );
}

/**
 * Metoda lambda (kangaroo) dla znalezienia logarytmu dyskretnego
 * @param	[ZZ] p	Liczba pierwsza, Z_p
 * @param	[ZZ] q	Rząd grupy (zakładamy q = (p - 1) / 2)
 * @param	[ZZ] g	Generator grupy
 * @param	[ZZ] h	Element grupy
 * @param	[ZZ] a	Dolny zakres szukanego logarytmu
 * @param	[ZZ] b	Górny zakres szukanego logarytmu
 * @return	[ZZ] x	Logarytm dyskretny
 */
ZZ kangaroo_x( ZZ p, ZZ q, ZZ g, ZZ h, ZZ a, ZZ b )
{
	// Rozwiązanie do zwrócenia
	ZZ x;

	// Czy znaleziono już rozwiązanie?
	bool x_found = false;

	// Indeks i tag kangura
	int index, tag;

	// Liczba v z artykułu
	ZZ v = ((beta( a, b ) / (NUM_THREADS / 2)));

	// Pozycja i dystans kangura
	ZZ pos, d;

	// Zbiór punktów rozróżnialnych
	unordered_map <long long, DPoint> dpoints;

	// Dodatkowe, pomocnicze zmienne
	ZZ start, exponent;
	DPoint dpoint;

	// Sekcja równoległa
	#pragma omp parallel private( pos, d, index, tag, start, dpoint ) num_threads( NUM_THREADS )
	{
		// Pobierz id wątku/procesora
		index = omp_get_thread_num();

		// Wyznacz typ kangura
		tag = (index % 2 == 0) ? KANGAROO_TAME : KANGAROO_WILD;

		// Dopuszczaj tylko indeksy 0, 1 dla opu typów kangurów
		index = index / 2;

		// Początkowe położenie kangurów
		if( tag == KANGAROO_TAME )
		{
			start = ((a + b) / 2 + (index * v));
			pos = PowerMod( g, start, p );
		}
		else
		{
			start = (index * v);
			pos = MulMod( h, PowerMod( g, start, p ), p );
		}

		// Początkowa całkowita długość podróży kangura
		d = index * v;

		// W nieskończoność
		while( true )
		{
			// Zwiększ dystans i wykonaj skok
			d = d + f( pos );
			pos = MulMod( pos, PowerMod( g, f( pos ), p ), p );

			// Jeżeli znaleziono już rozwiązanie - przerwij pętlę
			if( x_found )
				break;

			// Sprawdź, czy punkt jest rozróżnialny
			bool distinguished = is_distinguished( pos );
			if( !distinguished )
				continue;

			// Stwórz obiekt punktu rozróżnialnego
			dpoint.tag		= tag;
			dpoint.index	= index;
			dpoint.distance	= d;

			// Konwersja pozycji na typ long
			long at = conv <long> ( pos );

			#pragma omp critical
			{
				// Jeśli punkt nie istnieje w zbiorze - dodaj go
				if( dpoints.find( at ) == dpoints.end() )
				{
					dpoints[ at ] = dpoint;
				}

				// A jeśli istnieje, to występuje kolizja
				else
				{
					// Interesuje nas kolizja dwóch różnych typów kangurów
					if( !x_found && dpoints[ at ].tag != tag )
					{
						// Pobierz wartości dystansów kangurów
						ZZ tame_d, wild_d;
						tame_d = (tag == KANGAROO_TAME) ? d : dpoints[ at ].distance;
						wild_d = (tag == KANGAROO_WILD) ? d : dpoints[ at ].distance;

						// Oblicz logarytm dyskretny
						x = ((a + b) / 2) + tame_d - wild_d;
						x = x % q;
						
						// Ustaw flagę znalezionego rozwiązania
						x_found = true;
					}
				}
			}
		}
	}

	// Zwróć wyliczony logarytm
	return x;
}

int main()
{
	ZZ p, q, g, h, a, b, x;
	cout << "p q g h a b" << endl;
	cin >> p >> q >> g >> h >> a >> b;
	
	x = kangaroo_x( p, q, g, h, a, b );
	cout << x << endl;

	return 0;
}