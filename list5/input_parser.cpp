/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm Pohliga-Hellmana
 *
 * Parser danych wejściowych
 */

#include "InputData.cpp"
#include <iostream>
#include <NTL/ZZ.h>
NTL_CLIENT
using namespace std;

/**
 * Funkcja przyjmuje dane wejściowe dla listy, odpowiednio
 * je preparuje i zwraca obiekt
 * 
 * @return [InputData]	Obiekt z danymi dla algorytmu
 */
InputData parse_input()
{
	InputData input_data;
	int factors;
	char sign;
	ZZ curr_prime;
	long curr_exponent;
	PrimesPair primes_pair;

	cin >> input_data.p;
	cin >> input_data.Q;
	input_data.r = 0;
	if( input_data.Q != 0 )
		input_data.r = (input_data.p - 1) / input_data.Q;
	cin >> factors;
	
	while( factors-- )
	{
		cin >> curr_prime >> sign >> curr_exponent;
		primes_pair.prime = curr_prime;
		primes_pair.exponent = curr_exponent;
		input_data.factors.push_back( primes_pair );
	}

	cin >> input_data.g;
	cin >> input_data.h;

	// Zwróć obiekt
	return input_data;
}