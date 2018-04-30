/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm Pohliga-Hellmana
 */

#include "kangaroo.cpp"
#include "input_parser.cpp"

#define	NO_ERRORS	0
#define endl2		"\n\n"

/**
 * Właściwy algorytm Pohliga-Hellmana
 * 
 * @param	[InputData] input_data	Obiekt danych wejściowych
 */
void pohlig_hellman( InputData input_data )
{
	ZZ p = input_data.p;
	ZZ Q = input_data.Q;
	ZZ g = input_data.g;
	ZZ h = input_data.h;
	ZZ order = (p - 1) / 2;
	
	vector <ZZ> alphas;
	vector <PrimesPair> factors = input_data.factors;
	
	// Dla każdej pary czynników
	for( PrimesPair pair : factors )
	{
		ZZ a_i = (ZZ) 0;
		ZZ p_i = pair.prime;
		long e_i = pair.exponent;
		ZZ p_e = power( pair.prime, pair.exponent );
		ZZ order_i = p_e - 1;
		
		vector <ZZ> l;
		for( long k = 0; k < pair.exponent; k++ )
		{
			ZZ base = PowerMod( g, order / e_i, p );
			ZZ tmp_exp = (ZZ) 0;

			for( long j = 0; j < k; j++ )
			{
				if( l.size() > j )
					tmp_exp += (l.at( j ) * power( p_i, j ));
			}

			ZZ tmp_base = PowerMod( g, order / power( p_i , (k + 1) ), p );
			ZZ tmp_power = PowerMod( tmp_base, tmp_exp, p );
			tmp_power = InvMod( tmp_power, p );
			ZZ powr = (PowerMod( h, order / power( p_i, (k + 1) ), p ) * tmp_power ) % p

			ZZ lk = kangaroo_x(  );
		}
	}
}

int main()
{
	// Uruchom właściwy algorytm
	pohlig_hellman( parse_input() );

	// Jeśli nie ma błędów, to nie ma błędów
	return NO_ERRORS;
}