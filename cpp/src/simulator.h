#pragma once
#include "math.h"
#include <vector>
#include <mutex>
#include <map>

#define PRECISION 2048

namespace sim{

	struct Context{
		mpf_class R;
		mpf_class k;
		mpf_class T;
		mpf_class Epsilon;
		mpf_class Sigma;
		math::point box_size;
		mpf_class dt;
		int thread_count;
		std::vector<int> atoms_per_thread;
	};

	struct type_t{
		char *id;
		std::map<type_t*, mpf_class> sigmas;
		mpf_class mass;
	};

	extern Context ctx;

	class atom{
		public:
		type_t *type;
		math::vector speed;
		math::vector speed_half_dt;
		math::vector resultant;
		std::mutex mtx;
		math::point position;
		atom( math::point position = O, math::vector speed = v0, type_t* type = NULL );
		void update_position( mpf_class dt );
		void update_speed( mpf_class dt );
		void apply_force( math::vector force );
	};
	bool init( mpf_class molar_mass, mpf_class temperature, mpf_class dt, int thread_count );
	void read( char* fname );
	void insert( atom *a );
	mpf_class calculate_temperature( mpf_class *kinetic_energy = 0 );
	math::vector calculate_momentum();
	void scale_temperature( mpf_class *kinetic_energy );
	void reduce_momentum( math::vector momentum );
	void control_temperature();
	void control_momentum();
	void step();
	void dump( char* fname );
	void dump( FILE *f );
	void clear();
}