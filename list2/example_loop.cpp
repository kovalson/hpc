/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * OpenMP example foor loop program
 *
 * Kompilacja:
 * 		g++ example_loop.cpp -o example_loop -fopenmp -lpthread
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define	N	100

using namespace std;

int main()
{
	int nthreads, tid, i;
	float a[ N ], b[ N ], c[ N ];

	/* Some initializations */
	for( i = 0; i < N; i++ )
	{
		a[ i ] = b[ i ] = i;
	}

	#pragma omp parallel shared( a, b, c, nthreads ) private( i, tid )
	{
		tid = omp_get_thread_num();
		if( tid == 0 )
		{
			nthreads = omp_get_num_threads();
			cout << "Number of threads = " << nthreads << endl;
		}

		cout << "Thread " << tid << " starting..." << endl;

		#pragma omp for
		for( i = 0; i < N; i++ )
		{
			c[ i ] = a[ i ] + b[ i ];
			cout << "Thread " << tid << ": c[" << i << "] = " << c[ i ] << endl;
		}
	} /* end of parallel section */
}