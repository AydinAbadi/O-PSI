#include<iostream>
#include<gmp.h>
#include <gmpxx.h>
#include<sstream>
using namespace std;
#include<iomanip>
#include <string>
#include<fstream>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <assert.h>
typedef mpz_t bigint;
#include <omp.h>
#include <time.h>
#include "cryptopp/cryptlib.h"
#include <cryptopp/pwdbased.h>
#include"cryptopp/hmac.h"
#include"cryptopp/secblock.h"
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
// #include<paillier.h>
#include <NTL/ZZ.h>
#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/ZZ_pEX.h>
#include <NTL/RR.h>
#include <NTL/tools.h>
using namespace NTL;
#include <NTL/vec_ZZVec.h>
#include <NTL/fileio.h>
#include <NTL/FacVec.h>
#include <NTL/new.h>
#include <vector>

class Random{

public:
	Random(){};
	~Random(){};
	
	void get_rand_devurandom( char* buf, int len );
	void init_rand3( gmp_randstate_t& rand, bigint ran, int bytes );
	void get_rand_file( char* buf, int len, char* file );
	
	
};




