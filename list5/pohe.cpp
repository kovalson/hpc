/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm Pohliga-Hellmana
 */

#include "bsgs.cpp"
#include "dlog.cpp"
#include "kangaroo.cpp"
#include "input_parser.cpp"

#define THREADS_NUM		4
#define	NO_ERRORS		0

ZZ calc_CRT( vector <PrimesPair> factors, vector <ZZ> xs )
{
	long k = factors.size();
	ZZ a, p, A, P;

	// Pierwsza iteracja
	a = xs.at( 0 );
	p = power( factors.at( 0 ).prime, factors.at( 0 ).exponent );

	// Kolejne iteracje
	for( long i = 1; i < k; i++ )
	{
		A = xs.at( i );
		P = power( factors.at( i ).prime, factors.at( i ).exponent );
		CRT( a, p, A, P );
	}

	return (a + p);
}

/**
 * Funkcja sprawdza, czy wykładnik potęgi jest większy czy mniejszy od zera.
 * W pierwszym przypadku zwraca zwyczajną potęgę elementu w grupie, jeśli
 * zaś wykładnik jest mniejszy od zera - liczy odwrotność, zamienia ujemny wykładnik
 * na dodatni i dopiero wtedy potęguje.
 * 
 * @param  x       Element grupy
 * @param  e       Wykładnik elementu
 * @param  modulus Modulo
 * @return         Wynik potęgowania w grupie
 */
ZZ ultra_power( ZZ x, long e, ZZ modulus )
{
	if( e >= 0 )
		return PowerMod( x, e, modulus );
	else
		return PowerMod( InvMod( x, modulus ), -e, modulus );
}

ZZ find_xi( ZZ r_i, ZZ p, long e, ZZ g, ZZ h, ZZ modulus )
{
	ZZ x = (ZZ) 0;

	long exp = conv<long>( power( p, e - 1 ) );
	ZZ gamma = PowerMod( g, exp, modulus );
	for( long k = 0; k < e; k++ )
	{
		long exp2 = conv<long>( power( p, e - 1 - k ) );
		ZZ h_k = PowerMod( MulMod( h, ultra_power( g, conv <long> (-x), modulus ), modulus ), exp2, modulus );
		ZZ d_k = bsgs( modulus, gamma, h_k ) % p;
		// ZZ d_k = find_x( modulus, r_i, gamma, h_k ) % p;
		// ZZ d_k = kangaroo_x( modulus, p, gamma, h_k, (ZZ) 0, p );
		// cout << d_k << endl;
		x = x + power( p, k ) * d_k;
	}

	return x;
}

/**
 * Właściwy algorytm Pohliga-Hellmana
 * 
 * @param	[InputData] input_data	Obiekt danych wejściowych
 */
void pohlig_hellman( InputData input_data )
{
	ZZ p = input_data.p;
	ZZ n = p - 1;
	ZZ Q = input_data.Q;
	ZZ g = input_data.g;
	ZZ h = input_data.h;
	vector <PrimesPair> factors = input_data.factors;
	int factors_number = factors.size();
	int i;
	vector <ZZ> xs;

	#pragma omp parallel for private( i ) ordered num_threads( THREADS_NUM )
	for( i = 0; i < factors_number; i++ )
	{
		PrimesPair pair = factors.at( i );

		long e_i = pair.exponent;
		ZZ p_i = pair.prime;
		ZZ r_i = power( p_i, e_i );

		ZZ g_i = PowerMod( g, n / r_i, p );
		ZZ h_i = PowerMod( h, n / r_i, p );
		ZZ x_i = find_xi( r_i, p_i, e_i, g_i, h_i, p );

		cout << "Done " << i << " out of " << factors_number << "." << endl;

		#pragma omp ordered
		xs.push_back( x_i );
	}

	cout << calc_CRT( factors, xs ) << endl;
}

int main()
{
	// Uruchom właściwy algorytm
	pohlig_hellman( parse_input() );

	// Jeśli nie ma błędów, to nie ma błędów
	return NO_ERRORS;
}