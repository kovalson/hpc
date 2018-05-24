/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm Pohliga-Hellmana
 */

#include "bsgs.cpp"
#include "dlog_faster.cpp"
#include "input_parser.cpp"

#define THREADS_NUM		4
#define	NO_ERRORS		0
#define	SMALL_NUM		4294967296

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

	long exp = conv<long>( PowerMod( p, e - 1, modulus ) );
	ZZ gamma = PowerMod( g, exp, modulus );
	for( long k = 0; k < e; k++ )
	{
		long exp2 = conv<long>( power( p, e - 1 - k ) );
		ZZ h_k = PowerMod( MulMod( h, ultra_power( g, conv <long> (-x), modulus ), modulus ), exp2, modulus );
		ZZ d_k;
		if( r_i >= SMALL_NUM )
			d_k = pollard_rho( modulus, p, gamma, h_k );
		else
			d_k = bsgs( modulus, p, gamma, h_k );
		x = x + PowerMod( p, k, modulus ) * d_k;
	}

	return x;
}

/**
 * Właściwy algorytm Pohliga-Hellmana
 * 
 * @param	[InputData] input_data	Obiekt danych wejściowych
 */
ZZ pohlig_hellman( InputData input_data )
{
	ZZ p = input_data.p;
	ZZ n = p - 1;
	ZZ Q = input_data.Q;
	ZZ g = input_data.g;
	ZZ h = input_data.h;
	vector <PrimesPair> factors = input_data.factors;
	vector <ZZ> xs;
	int factors_number = factors.size();
	int i;

	// #pragma omp scheduled( dynamic ) parallel for private( i ) ordered num_threads( THREADS_NUM )
	for( i = 0; i < factors_number; i++ )
	{
		PrimesPair pair = factors.at( i );

		long e_i = pair.exponent;
		ZZ p_i = pair.prime;
		ZZ r_i = PowerMod( p_i, e_i, p );

		ZZ g_i = PowerMod( g, (n / r_i), p );
		ZZ h_i = PowerMod( h, (n / r_i), p );
		ZZ x_i = find_xi( r_i, p_i, e_i, g_i, h_i, p );

		cout << "Done " << i << " out of " << factors_number << ": " << x_i << endl;

		// #pragma omp ordered
		xs.push_back( x_i );
	}

	return calc_CRT( factors, xs );
}

ZZ find_order( InputData input_data )
{
	ZZ p = input_data.p;
	ZZ g = input_data.g;
	ZZ order = p;
	vector <PrimesPair> factors = input_data.factors;
	int factors_number = factors.size();
	int i;
	for( i = 0; i < factors_number; i++ )
	{
		PrimesPair pair = factors.at( i );
		long e_i = pair.exponent;
		ZZ p_i = pair.prime;
		ZZ r_i = PowerMod( p_i, conv<ZZ>( e_i ), p );

		order = order / (r_i);
		ZZ g1 = PowerMod( g, order, p );
		while( g1 != 1 )
		{
			g1 = PowerMod( g1, p_i, p );
			order = order * p_i;
		}
	}

	return order;
}

int main()
{
	// Pobierz dane wejściowe
	InputData input_data = parse_input();

	ZZ order = find_order( input_data );
	ZZ x = pohlig_hellman( input_data );

	// cout << PowerMod( x, input_data.Q, input_data.p ) << endl;

	// cout << "order of g: " << find_order( input_data ) << endl;

	// Uruchom właściwy algorytm
	// ZZ x = pohlig_hellman( input_data );

	// Wyświetl rozwiązanie
	// cout << "x = " << x << endl;
	// cout << ((input_data.p - 1) / input_data.Q) << endl;


	// 223979678950374436354438859208510927230717948972589583830341554824642727110733039301478963579501877124941517433246464906794936956987500434689
	// 204189442450419809757997196974543950854716761374143349301316507700344079100070328292462738425325278082363635064354279104914245305880972871696
	// 48397682908817790582566739043048997579932668321988369872234627118367668816836
	// 9393097394439830831873023502537651254674392030431019064234494701908233765286953450857869726075753556805436262255803877086528504125500827703404358055788342337625523108298117664201675856687713363228576714811136889951896
	// 9244648548005050119577028427992210643787607528573644958525367031828421292478021704474573269814779917394388564021352667785416048006957786100091104666166779172777649165015098516189647966842576714788857171399244194249394

	// ZZ x, y;
	// cin >> x >> y;
	// cout << (x % y) << endl;

	// Jeśli nie ma błędów, to nie ma błędów
	return NO_ERRORS;
}