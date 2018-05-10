/**
 * Klasa punktu rozróżnialnego dla metody lambda
 * dla problemu znalezienia algorytmu dyskretnego
 */
#include <NTL/ZZ.h>
NTL_CLIENT

class DPoint
{
public:

	// Rodzaj kangura
	int tag;

	// Indeks kangura (wątku)
	int index;

	// Aktualny przebyty dystans kangura
	ZZ distance;

};