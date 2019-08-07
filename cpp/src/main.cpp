#include <cstdio>
#include <cstdlib>
#include <gmpxx.h>
#include <iomanip>
#include <iostream>
#include "simulator.h"
#include <vector>


extern std::vector<sim::atom> atoms;

int main( int argc, char **argv ){

	if( argc != 9 ){
		printf( "usage: sim mass_number temperature time_step step_count thread_count input_file output_file dump_course\n" );
		return 1;
	}

	if(!sim::init( mpf_class(argv[1], PRECISION), mpf_class(argv[2], PRECISION), mpf_class(argv[3], PRECISION), atoi( argv[5] ) )){
		printf("error: couldn't initialise simulation\n");
		return 2;
	}

	sim::read(argv[6]);
	
	int step_count = atoi( argv[4] );
	FILE *f;
	f = fopen(argv[7], "wt");
	if( f == NULL ){
		printf( "error: couldn't open file \"%s\" for writing", argv[7] );
		return 3;
	}


	for( int i=0; i < step_count; i++ ){

		if( i%10 == 0 ){
			sim::control_momentum();
			sim::control_temperature();
		}
		if( atoi(argv[8])!=0 && (i-5)%10 == 0 ){
			sim::dump( f );
		}
		sim::step();
		
		if( i%40 == 0 )
			printf( "\r%i%%", (i*100)/step_count );
		else
			printf(".");
		fflush( stdout );
	}
	printf("\n");

	sim::dump( f );
	sim::dump( (char*) (std::string("end_")+std::string(argv[7])).c_str() );

	return 0;
}
