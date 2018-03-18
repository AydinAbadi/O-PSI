#include<iostream>
using namespace std;
#include<string>
#include<gmp.h>
#include <gmpxx.h>
#include<cstring>
#include<fstream>
#include <stdlib.h>
//#include"Polynomial.h"
typedef mpz_t bigint;


extern "C"{//XXXXX remove them on linux

 #include<paillier.h>

}//XXXX remove them on linux

class Random{

public:
	Random(){};
	~Random(){};
	
	void get_rand_devurandom( char* buf, int len );
	void init_rand3( gmp_randstate_t& rand, paillier_random_seed *ran, int bytes );
	void get_rand_file( char* buf, int len, char* file );
	
	
};




