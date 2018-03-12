/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * OpenMP - firstprivate, lastprivate, ordered
 *
 * Kompilacja:
 * 		g++ flo.cpp -o flo -fopenmp -lpthread
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void _private()
{
	int i;
	int x = 13;

	cout << "> private ---" << endl;
	cout << "x = " << x << endl;
	#pragma omp parallel for private( x )
	for( i = 0; i <= 10; i++ )
	{
		if( i == 0 )
		{
			cout << "x inside = " << x << endl;
		}
		x = i;
		cout << "Thread number: " << omp_get_thread_num() << ":\t" << x << endl;
	}
	cout << "x = " << x << endl << "-------------" << endl << endl;
}

void lastprivate()
{
	int i;
	int x = 13;

	cout << "> lastprivate ---" << endl;
	cout << "x = " << x << endl;
	#pragma omp parallel for lastprivate( x )
	for( i = 0; i <= 10; i++ )
	{
		if( i == 0 )
		{
			cout << "x inside = " << x << endl;
		}
		x = i;
		cout << "Thread number: " << omp_get_thread_num() << ":\t" << x << endl;
	}
	cout << "x = " << x << endl << "-----------------" << endl << endl;
}

void firstprivate()
{
	int i;
	int x = 13;

	cout << "> firstprivate ---" << endl;
	cout << "x = " << x << endl;
	#pragma omp parallel for firstprivate( x )
	for( i = 0; i <= 10; i++ )
	{
		if( i == 0 )
		{
			cout << "x inside = " << x << endl;
		}
		x = i;
		cout << "Thread number: " << omp_get_thread_num() << ":\t" << x << endl;
	}
	cout << "x = " << x << endl << "------------------" << endl << endl;
}

int calc( int i )
{
	int c = i;
	for( int j = 0; j < 1000000; j++ )
	{
		i = c + j;;
	}

	return i;
}

void ordered()
{
	int val = 13;
	int i;

	cout << "> ordered -----" << endl;
	#pragma omp parallel for private( val ) ordered
	for( i = 1; i <= 10; i++ )
	{
		val = calc( i );
		cout << "Thread " << omp_get_thread_num() << " got val " << val << endl;
		#pragma omp ordered
		{
			cout << i << " " << val << endl;
		}
	}
	cout << "---------------" << endl << endl;
}

int main()
{
	_private();
	lastprivate();
	firstprivate();
	ordered();
	return 0;
}