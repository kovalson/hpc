/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm baby-step-giant-step wspomagający
 * 			 algorytm Pohliga-Hellmana
 */

#include <NTL/ZZ.h>
#include <iostream>
#include <math.h>
#include <unordered_map>
NTL_CLIENT

using namespace std;

/**
 * Algorytm baby-step-giant-step
 * @param  p	Z_p - liczba pierwsza
 * @param  g	Generator grupy
 * @param  h	Element grupy
 * @return x	g^x = h (mod p)
 */
ZZ bsgs( ZZ p, ZZ q, ZZ g, ZZ h )
{;
	if( g == h ) return (ZZ) 1;
	if( g == (ZZ) 1 ) return (ZZ) 1;
	// ZZ N = ceil( SqrRoot( p - 1 ) );
	long long_N = conv <long> ( p - 1 );
	long_N = ceil( sqrt( long_N ) );
	ZZ N = conv <ZZ> ( long_N );
	N = q;
	unordered_map <long, long> tbl;

	for( ZZ i = (ZZ) 0; i < N; i++ )
	{
		ZZ pow = PowerMod( g, i, p );
		tbl[ conv <long> (pow) ] = conv <long> (i);
	}

	ZZ c = PowerMod( g, N * (p - 2), p );

	for( ZZ j = (ZZ) 0; j < N; j++ )
	{
		ZZ y = h * PowerMod( c, j, p ) % p;
		long long_y = conv <long> (y);
		if( tbl.find( long_y ) != tbl.end() )
			return j * N + tbl[ long_y ];
	}

	cout << "Brak rozwiązania :C" << endl;
	exit( 1 );
	return (ZZ) -1;
}