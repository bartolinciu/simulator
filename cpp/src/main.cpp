#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "simulator.h"
#include <vector>
#include <gmpxx.h>

extern std::vector<sim::atom> atoms;


int main( int argc, char **argv ){

	if( argc != 8 ){
		printf( "usage: sim molar_mass temperature time_step step_count thread_count input_file output_file\n" );
		return 1;
	}

	sim::init( mpf_class(argv[1], PRECISION), mpf_class(argv[2], PRECISION), mpf_class(argv[3], PRECISION), atoi( argv[5] ) );

	sim::read(argv[6]);
	
	int step_count = atoi( argv[4] );

	for( int i=0; i < step_count; i++ ){

		if( i%10 == 0 ){
			sim::control_momentum();
			sim::control_temperature();
		}
		sim::step();
		
		if( i%40 == 0 )
			printf( "\r%i%%", (i*100)/step_count );
		else
			printf(".");
		fflush( stdout );
	}
	printf("\n");

	sim::dump( argv[7] );

	return 0;
}
