#include <cstdio>
#include <gmpxx.h> 
#include <iostream>
#include <iomanip>
#include <map>
#include "simulator.h"
#include <string>
#include <thread>
#include <vector>



std::vector<sim::atom*> atoms;
std::map<std::string, mpf_class> scale;

mpf_class round( mpf_class a ){
	mpf_class c = trunc(a);
	mpf_class b = a - c;
	if( cmp( b, 0.5 ) >= 0 )
		return c + 1;
	else
		return c;
}

mpf_class operator%( mpf_class a, mpf_class b ){
	return a - trunc(a/b)*b;
}

mpf_class& operator%=( mpf_class& a, mpf_class b ){
	a -= trunc(a/b)*b;
	return a;
}

namespace sim{

	std::map<std::string, type_t*> types;
	Context ctx;

	bool init( mpf_class mass_number, mpf_class temperature, mpf_class dt, int thread_count ){
		mpf_set_default_prec(PRECISION);
		

		FILE* f = fopen( "sim.ini", "rt" );
		if( f == NULL ){
			printf("error: couldn't open configuration file for reading\n");
			return false;
		}
		int n;

		fscanf( f, "%i\n", &n );
		fseek( f, -1, SEEK_CUR );
		for ( int i=0; i < n; i++ ){
			char *ID = new char[6];
			memset( ID, 0, 6 );
			char *mass = new char[9];
			memset( mass, 0, 6 );
			fread( ID, 1, 5, f );
			fread( mass, 1, 8, f );
			type_t *type = new type_t;
			type->id = ID;
			type->mass = mpf_class( mass );
			types[ std::string(ID) ] = type;
			fgetc(f);
		}


		char ID1[6]={0};
		char ID2[6]={0};
		char value[9] = {0};


		while( !feof( f ) ){
			fread( ID1, 1, 5, f );
			fread( ID2, 1, 5, f );
			fread( value, 1, 8, f );
			char *stripped_value = value;
			while( stripped_value[0] == ' ' )
				stripped_value += 1;
			type_t *type1 = types[ std::string(ID1) ];
			type_t *type2 = types[ std::string(ID2) ];

			type1->sigmas[ type2 ] = mpf_class(stripped_value);
			type2->sigmas[ type1 ] = mpf_class(stripped_value);
			
			fgetc(f);
		}
		


		ctx.R = 8.314459848;
		ctx.k = 1;
		ctx.Epsilon = 1.5;
		ctx.Sigma = 1;
		ctx.thread_count = thread_count;

		scale[ "Length" ] = 0.375 * _pow(0.1, 9);
		scale["Energy"] = 72.953E-23 * 1.5;
		scale["Mass"] = 1.66053892173E-27 * mass_number;
		scale["k"] = 1.3806485279E-23;
		scale["Speed"] = sqrt( scale["Energy"]/scale["Mass"]);
		scale["Temperature"] = scale["Energy"] / scale["k"];
		scale["Time"] = scale["Length"] * sqrt( scale["Mass"]  / scale["Energy"]  );
		ctx.T = temperature / scale["Temperature"];
		ctx.dt = (dt * _pow( 0.1, 12 )) / scale["Time"];
		return true;
	}

	void clear(){
		for( auto t : types ){
			delete t.second;
		}
		types.clear();
	}

	void read( char * fname ){

		FILE *f;
		f = fopen( fname, "rt" );
		if( f == NULL )
			return;

		char c = 0;
		while( c != '\n' )
			c = fgetc(f);
		size_t length;
		char buf[9] = {0};
		fscanf( f, "%s", &buf );
		length = atoi( buf );
		mpf_class pos_x;
		mpf_class pos_y;
		mpf_class pos_z;
		mpf_class speed_x;
		mpf_class speed_y;
		mpf_class speed_z;
		type_t* type;
		fgetc(f);

		for( size_t i = 0; i < length; i++ ){
			fseek( f, 10, SEEK_CUR );
			fread( buf, 1, 5, f );
			buf[5] = 0;
			type = types[ std::string(buf) ];
			fseek( f, 5, SEEK_CUR );
			fread( buf, 1, 8, f );
			pos_x = mpf_class( buf );
			fread( buf, 1, 8, f );
			pos_y = mpf_class( buf );
			fread( buf, 1, 8, f );
			pos_z = mpf_class( buf );
			fread( buf, 1, 8, f );
			speed_x = mpf_class( buf );
			fread( buf, 1, 8, f );
			speed_y = mpf_class( buf );
			fread( buf, 1, 8, f );
			speed_z = mpf_class( buf );
			fgetc(f);

			insert( new atom( math::point( pos_x, pos_y, pos_z ), math::vector( speed_x, speed_y, speed_z ), type ) );
		}
		
		char size_x[10]={0};
		char size_y[10]={0};
		char size_z[10]={0};

		fscanf( f, "%s %s %s\n", size_x, size_y, size_z );
		ctx.box_size = math::point( mpf_class(size_x), mpf_class(size_y), mpf_class(size_z) );

		fclose(f);

		mpf_class n = atoms.size();
		mpf_class s = n - 1;
		mpf_class t = ctx.thread_count;
		mpf_class operations_per_thread = (n * (n-1))/(2*t);

		for( int i = 0; i < ctx.thread_count-1; i++ ){
			mpf_class x = round( s - 0.5 - sqrt( 4*s*(s+1) + 1 - operations_per_thread*8 )/2 );
			s -= x;
			ctx.atoms_per_thread.push_back( x.get_ui() );
		}
		ctx.atoms_per_thread.push_back(s.get_ui());
	}

	void insert( atom *a ){
		atoms.push_back(a);
	}

	math::vector calculate_force( atom *i, atom *j ){
		math::point a = i->position;
		math::point b = j->position;
		math::vector r = math::vector( b, a );

		if( cmp( abs(r.x),  1.5 ) > 0 ){
			b.x += ctx.box_size.x * sgn( r.x ); 
		}
		if( cmp( abs(r.y),  1.5 ) > 0 ){
			b.y += ctx.box_size.y * sgn( r.y ); 
		}
		if( cmp( abs(r.z),  1.5 ) > 0 ){
			b.z += ctx.box_size.z * sgn( r.z ); 
		}

		mpf_class sigma = i->type->sigmas[j->type];

		r = math::vector( b, a );
		mpf_class length = r.length();
		if( cmp(length, 1.5) > 0 ){
			return ::v0;
		}
		math::vector f = (r * 24 * 1.5 * ( 2 * _pow( sigma/length, 12) - _pow( sigma/length, 6) ))/(length*length);

		return f;
	}

	mpf_class calculate_temperature( mpf_class* kinetic_energy ){
		mpf_class Ek = 0;
		for( size_t i = 0; i < atoms.size(); i++ )
			Ek += _pow(atoms[i]->speed.length(), 2) * atoms[i]->type->mass;
		
		if( kinetic_energy != NULL )
			*kinetic_energy = Ek;

		return Ek/( 3 * (atoms.size()-1) );
	}

	math::vector calculate_momentum(){
		math::vector momentum = ::v0;
		for( size_t i = 0; i < atoms.size(); i++ ){
			momentum += atoms[i]->speed * atoms[i]->type->mass;
		}
		return momentum;	
	}

	void scale_temperature( mpf_class *kinetic_energy ){
		for( size_t i = 0; i < atoms.size(); i++ ){
			atoms[i]->speed *= sqrt( ( 3 * ( atoms.size() - 1 ) * ctx.T ) / *kinetic_energy );
		}
	}

	void reduce_momentum( math::vector momentum ){
		for( size_t i = 0; i < atoms.size(); i++ ){
			atoms[i]->speed -= momentum/( atoms.size() * atoms[i]->type->mass );
		}
	}

	void control_temperature(){
		mpf_class *kinetic_energy = new mpf_class();
		mpf_class temperature = calculate_temperature( kinetic_energy );
		if( cmp(abs(temperature - ctx.T) , (0.1 * ctx.T)) > 0 ){
			scale_temperature(kinetic_energy);
		}
		delete kinetic_energy;
	}

	void control_momentum(){
		math::vector momentum = calculate_momentum();
		if( cmp( momentum.length() , _pow(10.0, -15) ) > 0 ){
			reduce_momentum( momentum );
		}
	}

	void update_forces( size_t start, size_t end ){
		for( size_t i = start; i < end; i++ )
			for( size_t j = i+1; j < atoms.size(); j++ ){
				math::vector f = calculate_force( atoms[i], atoms[j] );
				std::lock_guard<std::mutex> lck_i( atoms[i]->mtx ), lck_j( atoms[j]->mtx );
				atoms[i]->apply_force(f);
				atoms[j]->apply_force(-f);
			}
	}

	void step(){
		for( size_t i = 0; i < atoms.size(); i++ )
			atoms[i]->update_position(ctx.dt);
		
		int start = 0;

		std::vector<std::thread> threads;

		for( int i : ctx.atoms_per_thread ){
			threads.push_back( std::thread( update_forces, start, start + i ) );
			start += i;
		}

		for( size_t i=0; i < threads.size(); i++ ){
			threads[i].join();
		}
		
		for( size_t i = 0; i < atoms.size(); i++ )
			atoms[i]->update_speed(ctx.dt);
		
	}

	void dump( char* fname ){
		FILE *f;
		f = fopen( fname, "wt" );
		if( f == NULL )
			return;

		fprintf( f, "Simulation\n%lu\n", atoms.size() );
		for( size_t i = 0; i < atoms.size(); i++ ){			
			fprintf( f, "%5lu ATOM%5s%5lu%8.3f%8.3f%8.3f%8.4f%8.4f%8.4f\n", i, atoms[i]->type->id, i, atoms[i]->position.x.get_d(), atoms[i]->position.y.get_d(), atoms[i]->position.z.get_d(),
																					 atoms[i]->speed.x.get_d(), atoms[i]->speed.y.get_d(), atoms[i]->speed.z.get_d() );
		}
		fprintf( f, "%f %f %f\n", ctx.box_size.x.get_d(), ctx.box_size.y.get_d(), ctx.box_size.z.get_d() );

		fclose(f);
	}

	void dump( FILE *f ){
		if( f == NULL )
			return;

		fprintf( f, "Simulation\n%lu\n", atoms.size() );
		for( size_t i = 0; i < atoms.size(); i++ ){			
			fprintf( f, "%5lu ATOM%5s%5lu%8.3f%8.3f%8.3f%8.4f%8.4f%8.4f\n", i, atoms[i]->type->id, i, atoms[i]->position.x.get_d(), atoms[i]->position.y.get_d(), atoms[i]->position.z.get_d(),
																					 atoms[i]->speed.x.get_d(), atoms[i]->speed.y.get_d(), atoms[i]->speed.z.get_d() );
		}
		fprintf( f, "%f %f %f\n", ctx.box_size.x.get_d(), ctx.box_size.y.get_d(), ctx.box_size.z.get_d() );
	}



	atom::atom( math::point position, math::vector speed, type_t *type ){
		this->position = position;
		this->speed = speed;
		this->resultant = ::v0;
		this->type = type;
	}

	void atom::update_position( mpf_class dt ){
		math::vector a = this->resultant / this->type->mass;
		math::vector dr = this->speed * dt + a * _pow( dt, 2 ) * 0.5;
		if( cmp(abs(dr.x), ctx.box_size.x) > 0 || cmp(abs(dr.y), ctx.box_size.y) > 0 || cmp(abs(dr.z), ctx.box_size.z) > 0 ){
			printf("error: particle skipped the box\n");
		}
		this->position += dr;

		if( cmp( this->position.x, 0.0) < 0 ){
			this->position.x = ctx.box_size.x + this->position.x % ctx.box_size.x;
		}
		else
			this->position.x %= ctx.box_size.x;

		if( cmp( this->position.y, 0.0) < 0 ){
			this->position.y = ctx.box_size.y + this->position.y % ctx.box_size.y;
		}
		else
			this->position.y %= ctx.box_size.y;

		if( cmp( this->position.z, 0.0) < 0 ){
			this->position.z = ctx.box_size.z + this->position.z % ctx.box_size.z;
		}
		else
			this->position.z %= ctx.box_size.z;
		this->speed_half_dt = this->speed + a * dt * 0.5;
		this->resultant = ::v0;
	}

	void atom::update_speed( mpf_class dt ){
		math::vector a = this->resultant / this->type->mass;
		this->speed = this->speed_half_dt + a * dt * 0.5;
	}

	void atom::apply_force( math::vector force ){
		this->resultant += force;
	}
}
