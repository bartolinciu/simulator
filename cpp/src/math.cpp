#include <iomanip>
#include <iostream>
#include "math.h"
#include  <gmpxx.h>

mpf_class _pow( mpf_class base, int exponent ){
	if( exponent == 0 )
		return 0;

	if( exponent < 0 ){
		base = 1/base;
		exponent = -exponent;
	}

	mpf_class result = base;

	exponent -= 1;

	for( int i = 0; i < exponent; i++ ){
		result *= base;
	}
	return result;
}

namespace sim{
	namespace math{
		vector::vector(){
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		vector::vector( mpf_class x, mpf_class y, mpf_class z ){
			this->x = x;
			this->y = y;
			this->z = z;
		}

		vector::vector( point a, point b ){
			this->x = b.x - a.x;
			this->y = b.y - a.y;
			this->z = b.z - a.z;
		}

		mpf_class vector::length(){
			return sqrt( _pow(this->x, 2) + _pow(this->y, 2) + _pow(this->z, 2) );
		}

		void vector::normalize(){
			mpf_class length = this->length();
			this->x /= length;
			this->y /= length;
			this->y /= length;
		}

		vector vector::operator+( vector w ){
			return vector( this->x + w.x, this->y + w.y, this->z + w.z );
		}

		vector vector::operator-( vector w ){
			return vector( this->x - w.x, this->y - w.y, this->z - w.z );
		}

		vector vector::operator*( mpf_class alpha ){
			return vector( this->x * alpha, this->y * alpha, this->z * alpha );
		}

		vector vector::operator/( mpf_class alpha ){
			return vector( this->x / alpha, this->y / alpha, this->z / alpha );
		}

		vector vector::operator-(){
			return vector( - this->x, - this->y, - this->z );
		}

		vector& vector::operator+=( vector v ){
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}

		vector& vector::operator-=( vector v ){
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}

		vector& vector::operator*=( mpf_class alpha ){
			this->x *= alpha;
			this->y *= alpha;
			this->z *= alpha;
			return *this;
		}

		vector& vector::operator/=( mpf_class alpha ){
			this->x /= alpha;
			this->y /= alpha;
			this->z /= alpha;
			return *this;
		}

		bool vector::operator!=( vector v ){
			return !(this->x == v.x && this->y == v.y && this->z == v.z);
		}

		vector operator*( mpf_class alpha, vector v ){
			return vector( v.x * alpha, v.y * alpha, v.z * alpha );
		}

		std::ostream& operator<<( std::ostream& ostream, vector v ){
			ostream<<std::setprecision(20);
			ostream<<"["<<v.x<<", "<<v.y<<", "<<v.z<<"] ";
			return ostream;
		}

		point::point(){
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		point::point( mpf_class x, mpf_class y, mpf_class z ){
			this->x = x;
			this->y = y;
			this->z = z;
		}

		point point::operator+( vector v ){
			return point( this->x + v.x, this->y + v.y, this->z + v.z );
		}

		point point::operator-( vector v ){
			return point( this->x - v.x, this->y - v.y, this->z - v.z );
		}

		point& point::operator+=( vector v ){
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}

		point& point::operator-=( vector v ){
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}

		bool point::operator!=( point v ){
			return !(this->x == v.x && this->y == v.y && this->z == v.z);
		}

		std::ostream& operator<<( std::ostream& ostream, point A ){
			ostream<<std::setprecision(20);
			ostream<<"("<<A.x<<", "<<A.y<<", "<<A.z<<")";
			return ostream;
		}


	}
}