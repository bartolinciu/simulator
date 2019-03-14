#pragma once
#include <iostream>
#include <gmpxx.h>

#define v0 sim::math::vector(0,0,0)
#define O sim::math::point(0,0,0)

mpf_class _pow( mpf_class base, int exponent );

namespace sim{

	namespace math{

		class point;

		class vector{
		public:
			mpf_class x;
			mpf_class y;
			mpf_class z;
			vector();
			vector( mpf_class x, mpf_class y, mpf_class z );
			vector( point a, point b );

			mpf_class length();
			void normalize();
			
			vector operator+( vector w );
			vector operator-( vector w );
			vector operator*( mpf_class alpha );
			vector operator/( mpf_class alpha );
			vector operator-();
			vector& operator+=( vector v );
			vector& operator-=( vector v );
			vector& operator*=( mpf_class alpha );
			vector& operator/=( mpf_class alpha );
			bool operator!=( vector v ); 
		};

		class point{
		public:
			mpf_class x;
			mpf_class y;
			mpf_class z;
			point();
			point( mpf_class x, mpf_class y, mpf_class z );
			point operator+( vector v );
			point operator-( vector v );
			point& operator+=( vector v );
			point& operator-=( vector v );
			bool operator!=( point a );
		};

		vector operator*( mpf_class alpha, vector v );
		
		std::ostream& operator<<( std::ostream& ostream, point A );
		std::ostream& operator<<( std::ostream& ostream, vector v );

	}
}