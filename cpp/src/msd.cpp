#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "math.h"


struct conf{
	sim::math::point box;
	std::vector<sim::math::point>* points;
};

std::vector< conf > confs;

int co = 0;

bool verbose = true;

sim::math::point readPoint( FILE *f ){
	char buf[9] = {0};
	double x, y, z;

	#ifdef WIN32
	fseek( f, 20, SEEK_CUR );
	#else
	fseek( f, 21, SEEK_CUR );
	#endif

	fread( buf, 1, 8, f );
	x = atof( buf );
	fread( buf, 1, 8, f );
	y = atof( buf );
	fread( buf, 1, 8, f );
	z = atof( buf );

	#ifdef WIN32
	fseek( f, 25, SEEK_CUR );
	#else
	fseek( f, 24, SEEK_CUR );
	#endif

	return sim::math::point( x, y, z );
}

conf readConf( FILE *f ){
	char c = 0;
	while( c != '\n' )
		c = fgetc(f);
	size_t length;
	char buf[9] = {0};
	fscanf( f, "%s", &buf );
	length = atoi( buf );
	#ifdef WIN32
	if( co != 0 )
		fseek( f, -1, SEEK_CUR );
	#endif
	conf cf;
	std::vector<sim::math::point> *points = new std::vector<sim::math::point>;
	cf.points = points;

	for( size_t i = 0; i < length; i++ ){
		points->push_back( readPoint( f ) );
	}
	
	char buff[30]={0};

	fscanf( f, "%s %s %s\n", buff, buff + 10, buff + 20 ) ;
	cf.box = sim::math::point(atof(buff), atof(buff+10), atof(buff+20) );
	return cf;
}

bool read( char * fname ){
	FILE *f;
	f = fopen( fname, "rt" );
	if( f == NULL )
		return false;

	while( !feof(f) ){
		confs.push_back( readConf(f) );
		co++;
	}

	fclose(f);
	return true;
}

double _round( double a, double d ){
	return round(a/d)*d;
}

sim::math::point box(0,0,0);

struct particle{
	sim::math::point prev;
	sim::math::vector r;
};

std::vector<particle> dsp;
std::vector<mpf_class> msds;

int main( int argc, char** argv ){

	if( argc != 3 ){
		printf( "usage: ./rdf trace_file init_file\n" );
		return 1;
	}

	if(!read( argv[1] )){
		printf("reading trace_file failed\n");
		return 2;
	}

	FILE *base;

	if(NULL == (base = fopen(argv[2], "rt" ))){
		printf("reading init_file failed\n");
		fclose( base );
		return 3;
	}

	conf baseConf = readConf( base );
	fclose(base);

	for( sim::math::point p : *(baseConf.points) ){
		dsp.push_back( { p, sim::math::vector(0, 0, 0) } );
	}

	confs.pop_back();
	confs.pop_back();

	size_t t = 0;
	bool first = true;
	for( conf c:confs ){
		if( first ){
			first = false;
			continue;
		}
		if( t%40==0 )
			printf("\r%u%%", (t*100)/confs.size());
		printf(".");
		fflush(stdout);
		t+=1;


		for(  size_t j = 0; j < c.points->size(); j++ ){
			sim::math::point a = dsp[j].prev;
			sim::math::point b = (*(c.points))[j];
			dsp[j].prev = b;
			if(a==b)
				continue;
			sim::math::vector v(a,b);

			if( v.x >= c.box.x/2 )
				v.x -= c.box.x;
			else if( v.x <= -c.box.x/2 )
				v.x += c.box.x;

			if( v.y >= c.box.y/2 )
				v.y -= c.box.y;
			else if( v.y <= -c.box.y/2 )
				v.y += c.box.y;

			if( v.z >= c.box.z/2 )
				v.z -= c.box.z;
			else if( v.z <= -c.box.z/2 )
				v.z += c.box.z;
			
			dsp[j].r += v;
		}


		mpf_class sum = 0;
		for( particle part: dsp ){
			sum += part.r.length() * part.r.length();
		}
		mpf_class msd = sum/dsp.size();
		msds.push_back( msd );
	}


	printf("\n");

	size_t l = strlen(argv[1]);
	memcpy( argv[1]+l-4, ".msd", 4 );

	FILE *o = fopen( argv[1], "wt" );

	fprintf( o, "d\tn\n" );

	for( size_t i = 0; i < msds.size(); i++ ){
		fprintf( o, "%u\t%f\n", i, msds[i].get_d() );
	}

	fclose(o);

	return 0;
}

//4/3 * pi * ( (r + 1/2 * dr)^3 - (r - 1/2*dr)^3 )