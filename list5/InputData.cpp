/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * Wysoko wydajne obliczenia
 * Lista 5 - Algorytm Pohliga-Hellmana
 *
 * Klasa danych wejściowych dla algorytmu
 */

#include <vector>
#include <NTL/ZZ.h>
NTL_CLIENT
using namespace std;

class PrimesPair
{
public:
	ZZ prime;
	long exponent;
};

class InputData
{
public:
	ZZ p;
	ZZ Q;
	ZZ r;
	vector <PrimesPair> factors;
	ZZ g;
	ZZ h;
};