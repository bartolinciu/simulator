#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

struct point{
	double x;
	double y;
	double z;
};

bool operator==(point a, point b){
	return a.x==b.x && a.y==b.y && a.z == b.z;
}

struct vector{
	double x;
	double y;
	double z;
};

struct conf{
	point box;
	std::vector<point>* points;
};

vector ptov( point a, point b ){
	return (vector){ b.x-a.x, b.y-a.y, b.z-a.z };
}

double vectorLength( vector v ){
	return sqrt(  pow(v.x, 2.0) + pow( v.y, 2.0 ) + pow( v.z, 2.0 )  );
}

std::vector< conf > confs;

int co = 0;

bool verbose = true;

point readPoint( FILE *f ){
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

	return (point){ x, y, z };
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
	std::vector<point> *points = new std::vector<point>;
	cf.points = points;

	for( size_t i = 0; i < length; i++ ){
		points->push_back( readPoint( f ) );
	}
	
	char buff[30]={0};

	//#ifdef

	//#endif

	fscanf( f, "%s %s %s\n", buff, buff + 10, buff + 20 ) ;
	cf.box = (point){ atof(buff), atof(buff+10), atof(buff+20) };
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

point box={0,0,0};

int main( int argc, char** argv ){

	if( argc != 3 ){
		printf( "usage: ./rdf filename resolution\n" );
		return 1;
	}

	double dr = atof( argv[2] );

	if(!read( argv[1] )){
		printf("reading file failed\n");
		return 2;
	}

	confs.pop_back();
	confs.pop_back();

	std::map<double, double> master;

	size_t t = 0;
	printf("calculating rdf's:\n");
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
		box.x = fmax( box.x, c.box.x );
		box.y = fmax( box.y, c.box.y );
		box.z = fmax( box.z, c.box.z );

		std::map<double, double> *hist = new std::map<double, double>;
		size_t n = c.points->size();
		double d = ((double)(n-1))/(c.box.x*c.box.y*c.box.z);
		for(  size_t j = 0; j < c.points->size(); j++ )
			for( size_t k = j+1; k < c.points->size(); k++ ){
				point a = (*(c.points))[j];
				point b = (*(c.points))[k];
				if(a==b)
					continue;
				vector v = ptov(a,b);

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

				double r = _round(vectorLength(v), dr);
				if( hist->find(r) == hist->end() )
					hist->emplace(r, 2);
				else{
					hist->at(r) += 2.0;
				}
			}

		for( std::map<double, double>::iterator i = hist->begin(); i!= hist->end(); ++i  ){
			//printf("%f * %f = ", i->second, (3.0 * c.box.x*c.box.y*c.box.z)  );
			i->second *= (3.0 * c.box.x*c.box.y*c.box.z) / (4*3.14 * (pow( i->first + dr/2, 3.0 ) - pow( i->first - dr/2, 3.0 ) ) * (double) n  * (double)(n-1));
			//printf("%f\n", i->second);
		}

		for( auto i = hist->begin(); i != hist->end(); i++ ){
			if( master.find( i->first ) != master.end() )
				master.at( i->first ) += i->second;
			else
				master.emplace( i->first, i->second);
		}

		delete hist;
	}

	double R = fmax( box.x, fmax(box.y, box.z) );
	printf("\nnormalizing\n");
	std::map<double, double> rdf;
	size_t N = confs.size();

	for( auto i = master.begin(); i != master.end(); i++  ){
		if(i->first > R/2)
				continue;
		rdf.emplace(  i->first, master.at(i->first)/(double)N );
	}

	printf("\n");

	size_t l = strlen(argv[1]);
	memcpy( argv[1]+l-4, ".rdf", 4 );

	FILE *o = fopen( argv[1], "wt" );

	fprintf( o, "d\tn\n" );

	for( auto i: rdf ){
		fprintf( o, "%f\t%f\n", i.first, i.second );
	}

	fclose(o);

	return 0;
}

//4/3 * pi * ( (r + 1/2 * dr)^3 - (r - 1/2*dr)^3 )