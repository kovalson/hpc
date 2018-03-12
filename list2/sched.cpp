/**
 * Krzysztof Tatarynowicz
 * 221497
 * 
 * OpenMP - loop schedulling examples
 *
 * Kompilacja:
 * 		g++ sched.cpp -o sched -lpthread -fopenmp
 */

#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#define	THREADS	4
#define	N		16
#define GREAT	10000000

using namespace std;

void sched_static()
{
	int i;

	cout << "static scheduling" << endl;
	clock_t begin = clock();
	#pragma omp parallel for schedule( static ) num_threads( THREADS )
	for( i = 0; i < N; i++ )
	{
		sleep( i );
		cout << "Thread " << omp_get_thread_num() << " has completed iteration " << i << endl;
	}
	cout << "All done! (" << ((double) (clock() - begin) / CLOCKS_PER_SEC) << " sec)" << endl << endl;
}

void sched_static2()
{
	int i;


	cout << "static scheduling 2" << endl;
	clock_t begin = clock();
	#pragma omp parallel for schedule( static ) num_threads( THREADS )
	for( i = 0; i < GREAT; i++ );
	cout << "All done! (" << ((double) (clock() - begin) / CLOCKS_PER_SEC) << " sec)" << endl << endl;
}

void sched_dynamic()
{
	int i;

	cout << "dynamic scheduling" << endl;
	clock_t begin = clock();
	#pragma omp parallel for schedule( dynamic ) num_threads( THREADS )
	for( i = 0; i < N; i++ )
	{
		sleep( i );
		cout << "Thread " << omp_get_thread_num() << " has completed iteration " << i << endl;
	}

	cout << "All done! (" << ((double) (clock() - begin) / CLOCKS_PER_SEC) << " sec)" << endl << endl;
}

void sched_dynamic2()
{
	int i;

	cout << "dynamic scheduling 2" << endl;
	clock_t begin = clock();
	#pragma omp parallel for schedule( dynamic ) num_threads( THREADS )
	for( i = 0; i < GREAT; i++ );
	cout << "All done! (" << ((double) (clock() - begin) / CLOCKS_PER_SEC) << " sec)" << endl << endl;
}

int main()
{
	sched_static2();
	sched_dynamic2();
	return 0;
}