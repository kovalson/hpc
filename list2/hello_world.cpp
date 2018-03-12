/**
 * Krzysztof Tatarynowicz
 * 221497
 *
 * OpenMP Hello World example program
 *
 * Kompilacja:
 * 		g++ hello_world.cpp -o hello_world -fopenmp -lpthread
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main( int argc, char *argv[] )
{
	int nthreads, tid;

	/* Fork a team of threads giving them their own copies of variables */
	#pragma omp parallel private( nthreads, tid )
	{
		/* Obtain thread number */
		tid = omp_get_thread_num();
		cout << "Hello World from thread =" << tid << endl;

		/* Only master thread does this */
		if( tid == 0 )
		{
			nthreads = omp_get_num_threads();
			cout << "Number of threads = " << nthreads << endl;
		}
	} /* All threads join master thread and disband */
}